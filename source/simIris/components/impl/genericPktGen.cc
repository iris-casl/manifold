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
    stat_total_lat = 0;
    stat_hop_count= 0;

    ready.resize( vcs );
    ready.insert( ready.begin(), ready.size(), false );
    for(unsigned int i = 0; i < ready.size(); i++)
        ready[i] = false;
    lastSentTime = 0;


    for( unsigned int i = 0; i < vcs; i++ )
    {
        IrisEvent* event = new IrisEvent();
        event->type = READY_EVENT;
        event->vc = i;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,this, event);
        // interface_connections[0], event);
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
    /*
       out_file.open(out_filename.c_str());
       if( !out_file.is_open() )
       {
       stringstream stream;
       stream << "Could not open output trace file " << out_filename << ".\n";
       timed_cout(stream.str());
       }
     */
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
    stat_last_packet_out_cycle = Simulator::Now();
    if( stat_min_pkt_latency > lat)
        stat_min_pkt_latency = lat;

    stat_packets_in++;
    stat_hop_count += hlp->hop_count;

#ifdef DEBUG
    _DBG( "-------------- TPG GOT NEW PACKET ---------------\n pkt_latency: %f", lat);
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
    for( uint i=last_vc+1; i<vcs; i++)
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
    if( found )
    {
        ullint interval = rand() % 5 ;
        if( interval == 4 )
        {
            stat_packets_out++;
            HighLevelPacket* hlp = new HighLevelPacket();
            hlp->virtual_channel = sending_vc;
            hlp->source = node_ip;
            hlp->destination = ( no_nodes - node_ip - 1 ) % ( no_nodes ); //(rand() % 1000) % no_nodes ;
            hlp->addr = 0xfff;
            hlp->transaction_id = 1000;
            if( hlp->destination == node_ip )
                hlp->destination = (hlp->destination + 1) % no_nodes ;

            hlp->sent_time = (ullint)Simulator::Now()+1;
            hlp->data_payload_length = 4*max_phy_link_bits;	
            hlp->msg_class = RESPONSE_PKT;	
            for ( uint i=0 ; i < 4*max_phy_link_bits ; i++ )
            {
                hlp->data.push_back(true);
            }
            ready[sending_vc] = false;

#ifdef _DEEP_DEBUG
            assert ( hlp->destination < no_nodes);
            _DBG( " Sending at TPG out pull: VC %d dest:%d ", sending_vc, hlp->destination );
            cout << "HLP: " << hlp->toString() << endl;
            _DBG( " Sending pkt: no of packets %d ", stat_packets_out);
#endif

            IrisEvent* event = new IrisEvent();
            event->type = NEW_PACKET_EVENT;
            event->event_data.push_back(hlp);
            event->vc = sending_vc;
            Simulator::Schedule( hlp->sent_time, &NetworkComponent::process_event, interface_connections[0], event );

            lastSentTime = (ullint)Simulator::Now();
        }
        sending = true;

    }
    else
    {
        sending = true;
    }

    /* 
       if( Simulator::Now()-lastSentTime > 1000000 )
       {
       cout << " ******* Deaslock detected ****** " << endl;
       cout << " TPG " << node_ip << endl;
       cout << " last_packet_out_cycle: " << last_packet_out_cycle << endl;
       cout << " SimNow: " << Simulator::Now() << endl;
       cout << " mshrsize: " << mshrHandler->mshr.size() << endl;

       for (unsigned int i=0; i<mshrHandler->mshr.size(); i++)
       cout << mshrHandler->mshr[i].address << endl;
    //        for ( uint i=0; i<64; i++)
    //            static_cast<GenericRouterVcs*>(mesh->routers[i])->dump_ib_state();
    exit(1);
    }
     * */
    if( sending)
    {
        IrisEvent* event2 = new IrisEvent();
        event2->type = OUT_PULL_EVENT;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event2);
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
    if( !sending && Simulator::Now() < max_sim_time )
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
    str << "\nPktGen [" << node_ip << "] packets_out:\t" << stat_packets_out
        << "\nPktGen [" << node_ip << "] packets_in:\t" << stat_packets_in
        << "\nPktGen [" << node_ip << "] min_pkt_latency:\t" << stat_min_pkt_latency
        << "\nPktGen [" << node_ip << "] last_packet_out_cycle:\t" << stat_last_packet_out_cycle
        << "\nPktGen [" << node_ip << "] avg_latency:\t" << (stat_total_lat+0.0)/stat_packets_in
        << "\nPktGen [" << node_ip << "] avg_hop_count:\t" << (stat_hop_count+0.0)/stat_packets_in
        ;
    return str.str();
} /* ----- end of function GenericPktGen::toString ----- */

#endif /* ----- #ifndef _genericPktGen_cc_INC ----- */
