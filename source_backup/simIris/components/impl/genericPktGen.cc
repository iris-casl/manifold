/*!
 * =====================================================================================
 *
 *       Filename:  genericPktGen.cc
 *
 *    Description:  Implements the stochastic pkt gen class.
 *
 *        Version:  1.0
 *        Created:  02/20/2010 02:09:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef _genericPktGen_cc_INC
#define _genericPktGen_cc_INC

#include "genericPktGen.h"



GenericPktGen::GenericPktGen ()
{
    name = "GenericPktGen";
    sending = false;
    out_filename = "traceOut.tr";
    last_vc = 0;
    ni_recv = false;
    srand( time(NULL) );
} /* ----- end of function GenericPktGen::GenericPktGen ----- */

GenericPktGen::~GenericPktGen ()
{
    out_file.close();
    gsl_rng_free(dest_gen);
    gsl_rng_free(arate_gen);
    gsl_rng_free(plen_gen);
} /* ----- end of function GenericPktGen::~GenericPktGen ----- */

void
GenericPktGen::set_trace_filename( string filename )
{
    trace_name = filename;
    cout<<"TraceName: "<<trace_name<<endl;
}

void
GenericPktGen::set_no_vcs ( uint v)
{
    vcs = v;
}

void
GenericPktGen::setup (uint n, uint v, uint time)
{
    vcs =v;
    no_nodes = n;
    max_sim_time = time;
    address = myId();
    node_ip = address/3;
    last_vc = 0;
    
    stat_packets_in = 0;
    stat_packets_out = 0;
    stat_min_pkt_latency = 999999999;
    stat_last_packet_out_cycle = 0;
    stat_last_packet_in_cycle = 0;
    stat_total_lat = 0;
    stat_hop_count= 0;

    gsl_rng_env_setup();
    gsl_rng_default_seed = 2^25;
    T = gsl_rng_default;
    dest_gen = gsl_rng_alloc (T);
    plen_gen = gsl_rng_alloc (T);
    arate_gen = gsl_rng_alloc (T);

    ready.resize( vcs );
    ready.insert( ready.begin(), ready.size(), false );
    for(unsigned int i = 0; i < ready.size(); i++)
        ready[i] = false;
    lastSentTime = 0;
    irt = 0;


    for( unsigned int i = 0; i < vcs; i++ )
    {
        IrisEvent* event = new IrisEvent();
        event->type = READY_EVENT;
        event->vc = i;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,this, event);
    }


    return ;
} /* ----- end of function GenericPktGen::setup ----- */

void
GenericPktGen::set_output_path( string name)
{
    // open the output trace file
    stringstream str;
    str << name << "/tpg_" << node_ip << "_trace_out.tr";
    out_filename = str.str();
    out_file.open(out_filename.c_str());
    if( !out_file.is_open() )
    {
        _DBG_NOARG("Could not open output trace file ");
        cout << out_filename << endl;
    }
}

void
GenericPktGen::finish ()
{
    out_file.close();
    trace_filename->close();
    return ;
} /* ----- end of function GenericPktGen::finish ----- */

void
GenericPktGen::process_event (IrisEvent* e)
{
    switch(e->type)
    {
        case NEW_PACKET_EVENT:
            handle_new_packet_event(e);
            break;
        case OUT_PULL_EVENT:
            handle_out_pull_event(e);
            break;
        case READY_EVENT:
            handle_ready_event(e);
            break;
        default:
            cout << "\nTPG: " << address << "process_event: UNK event type" << endl;
            break;
    }
    return ;
} /* ----- end of function GenericPktGen::process_event ----- */

void
GenericPktGen::handle_new_packet_event ( IrisEvent* e)
{
    ni_recv = false;
    // get the packet data
    HighLevelPacket* hlp = static_cast< HighLevelPacket* >( e->event_data.at(0));
    double lat = Simulator::Now() - hlp->sent_time;
    if( stat_min_pkt_latency > lat)
        stat_min_pkt_latency = lat;

    stat_last_packet_in_cycle = Simulator::Now();
    stat_packets_in++;
    stat_hop_count += hlp->hop_count;
    stat_total_lat += lat;

#ifdef DEBUG
    _DBG( "-------------- TPG GOT NEW PACKET --------------- pkt_latency: %f", lat);
    // write out the packet data to the output trace file
    if( !out_file.is_open() )
        out_file.open( out_filename.c_str(), std::ios_base::app );

    out_file << hlp->toString();
    out_file << "\tPkt latency: " << lat << endl;
#endif

    delete hlp;

    // send back a ready event
    IrisEvent* event2 = new IrisEvent();
    event2->type = READY_EVENT;
    event2->vc = hlp->virtual_channel;
    Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event2);


    delete e;
    return ;
} /* ----- end of function GenericPktGen::handle_new_packet_event ----- */

void
GenericPktGen::handle_out_pull_event ( IrisEvent* e )
{
    sending =false;
    bool found = false;
    uint sending_vc = -1;
    uint max_vc = vcs;
    if(do_request_reply_network)
        max_vc = vcs/2;

    for( uint i=last_vc+1; i<max_vc; i++)
    {
        if(ready[i])
        {
            found = true;
            sending_vc = i;
            last_vc = i;
            break;
        }
    }
    if ( !found)
    {
        for ( uint i=0; i<=last_vc; i++)
        {
            if(ready[i])
            {
                found = true;
                sending_vc = i;
                last_vc = i;
                break;
            }
        }
    }
    /* Example of how to allow some nodes to inject 
       if( node_ip != (no_nodes-1))
       found = false;
     */
    if( found )
    {
        if( lastSentTime+irt <= Simulator::Now() )
            //        if( rand()%5 == 4 )
        {
            stat_packets_out++;
            HighLevelPacket* hlp = new HighLevelPacket();
            hlp->virtual_channel = sending_vc;
            hlp->source = node_ip;
            hlp->destination = mc_positions[gsl_rng_uniform_int( dest_gen , no_mcs) % ( no_mcs)];
            hlp->addr = node_ip*10000 + stat_packets_out;
            hlp->transaction_id = 1000;
            if( hlp->destination == node_ip )
                hlp->destination = (hlp->destination + 1) % no_nodes ;

            hlp->sent_time = (ullint)Simulator::Now();
            uint pkt_len = pkt_payload_length; //1*max_phy_link_bits; //Static for now but can use gsl_ran
            hlp->data_payload_length = pkt_len;	
            hlp->msg_class = terminal_msg_class;	
            /* 
               for ( uint i=0 ; i < pkt_len ; i++ )
               {
               hlp->data.push_back(true);
               }
             * */
            ready[sending_vc] = false;

            assert ( hlp->destination < no_nodes);
#ifdef _DEEP_DEBUG
            _DBG( " Sending at TPG out pull: VC %d dest:%d ", sending_vc, hlp->destination );
            cout << "HLP: " << hlp->toString() << endl;
            _DBG( " Sending pkt: no of packets %d ", stat_packets_out);
#endif
            out_file << Simulator::Now() << " " << hlp->destination << endl;

            IrisEvent* event = new IrisEvent();
            event->type = NEW_PACKET_EVENT;
            event->event_data.push_back(hlp);
            event->vc = sending_vc;
            Simulator::Schedule( hlp->sent_time, &NetworkComponent::process_event, interface_connections[0], event );

            lastSentTime = (ullint)Simulator::Now();
            irt = gsl_ran_gaussian_tail( arate_gen ,0,mean_irt) ;
            //            irt = gsl_ran_poisson( arate_gen ,mean_irt) ;
            stat_last_packet_out_cycle = Simulator::Now();
            //            if( stat_packets_out >= 10000 )
            //                irt = max_sim_time;
            /* 
               for( uint i=0; i<vcs; i++)
               if(ready[i])
               {
               irt = 10;
               break;
               }
               else
               irt=10;
             * */
            sending = true;
        }

    }
    else
    {
        sending = true;
        irt = gsl_ran_poisson( arate_gen ,mean_irt) ;
    }

    /* Example of how to allow some nodes to inject 
       if( node_ip != (no_nodes-1))
       sending = false;
     * */

    if( sending)
    {
        IrisEvent* event2 = new IrisEvent();
        event2->type = OUT_PULL_EVENT;
        Simulator::Schedule(Simulator::Now()+irt, &NetworkComponent::process_event, this, event2);
        sending = true;
    }		
    delete e; 
}

void
GenericPktGen::handle_ready_event ( IrisEvent* e)
{

    if ( e->vc > vcs )
    {
        _DBG(" Got ready for vc %d no_vcs %d ", e->vc, vcs);
        exit(1);
    }
    // send the first packet and schedule a out pull if ur ready
    ready[e->vc] = true;
    if( !sending && Simulator::Now()==1)// < max_sim_time )
    {
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        event->vc = e->vc; 
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
        sending = true;
    }

    delete e;
    return ;
} /* ----- end of function GenericPktGen::handle_ready_event ----- */

string
GenericPktGen::toString () const
{
    stringstream str;
    str << "\nGenericPktGen: "
        << "\t vcs: " << ready.size()
        << "\t address: " <<address
        << "\t node_ip: " << node_ip
        ;
    return str.str();
} /*  ----- end of function GenericPktGen::toString ----- */

string
GenericPktGen::print_stats() const
{
    stringstream str;
    str << "\n PktGen[" << node_ip << "] packets_out: " << stat_packets_out
        << "\n PktGen[" << node_ip << "] packets_in: " << stat_packets_in
        << "\n PktGen[" << node_ip << "] min_pkt_latency: " << stat_min_pkt_latency
        << "\n PktGen[" << node_ip << "] last_packet_out_cycle: " << stat_last_packet_out_cycle
        << "\n PktGen[" << node_ip << "] last_packet_in_cycle: " << stat_last_packet_in_cycle
        << "\n PktGen[" << node_ip << "] avg_latency: " << (stat_total_lat+0.0)/stat_packets_in
        << "\n PktGen[" << node_ip << "] avg_hop_count: " << (stat_hop_count+0.0)/stat_packets_in
        << endl;
    return str.str();
} /* ----- end of function GenericPktGen::toString ----- */

#endif /* ----- #ifndef _genericPktGen_cc_INC ----- */
