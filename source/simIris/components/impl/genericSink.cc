#ifndef  _genericsink_cc_INC
#define  _genericsink_cc_INC

#include	"genericSink.h"

/*
 *--------------------------------------------------------------------------------------
 *       Class:  GenericSink
 *      Method:  GenericSink
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
GenericSink::GenericSink ()
{
    out_filename = "sink_trace_.tr";
    name = "GenericSink";
}  /* -----  end of method GenericSink::GenericSink  (constructor)  ----- */

void
GenericSink::setup (uint n, uint v, uint time)
{
    vcs =v;
    no_nodes = n;
    max_sim_time = time;
    address = myId();
    node_ip = address/3;
    last_vc = 0;

    stat_packets_in = 0;
    stat_min_pkt_latency = 9999999;
    stat_last_packet_out_cycle = 0;
    stat_total_lat = 0;
    stat_hop_count = 0;

    ready.resize(vcs);

    ready.resize( vcs );
    ready.insert( ready.begin(), ready.size(), false );

    for( uint i = 0; i < vcs; i++ )
    {
        IrisEvent* e = new IrisEvent();
        e->type = READY_EVENT;
        e->vc = i;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,this, e);
    }
        return;
}		/* -----  end of function GenericSink  ----- */

void
GenericSink::set_output_path( string name)
{
    stringstream str;
    str << name << "/tpg_" << node_ip << "_trace_out.tr";
    out_filename = str.str();
}

void
GenericSink::process_event (IrisEvent* e)
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
            cout << "\nSINK: Unk event type" <<endl;
            break;
    }

    return ;
}		/* -----  end of function GenericSink::process_event  ----- */

void
GenericSink::handle_new_packet_event ( IrisEvent* e )
{
    ni_recv = false;
    // get the packet data
    HighLevelPacket* hlp = static_cast< HighLevelPacket* >( e->event_data.at(0));
    ullint lat = Simulator::Now() - hlp->sent_time;
    if( stat_min_pkt_latency > lat)
        stat_min_pkt_latency = lat;

    stat_last_packet_in_cycle = Simulator::Now();
    stat_packets_in++;
    stat_hop_count += hlp->hop_count;
    stat_total_lat += lat;

#ifdef DEBUG
    _DBG( "-------------- GENERIC_SINK GOT NEW PACKET --------------- pkt_latency: %f", lat);
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
}		/* -----  end of function GenericSink::handle_new_packet_event  ----- */

void
GenericSink::handle_out_pull_event ( IrisEvent* e )
{

    delete e;
    return ;
}		/* -----  end of function GenericSink::handle_outpull_event  ----- */

void
GenericSink::handle_ready_event ( IrisEvent* e )
{
    if ( e->vc > vcs )
    {
        _DBG(" Got ready for vc %d no_vcs %d ", e->vc, vcs);
        exit(1);
    }

    ready[e->vc] = true;


    delete e;
    return ;
}		/* -----  end of function GenericSink::handle_ready_event  ----- */

string
GenericSink::toString () const
{
    stringstream str;
    str << "GenericSink" 
        << "\t addr: " << address
        << "\toutput file = " << out_filename;
    return str.str();
}		/* -----  end of function GenericSink::toString  ----- */

string
GenericSink::print_stats() const
{
    stringstream str;
    str << "\n sink[" << node_ip << "] packets_out: " << stat_packets_out
        << "\n sink[" << node_ip << "] packets_in: " << stat_packets_in
        << "\n sink[" << node_ip << "] min_pkt_latency: " << stat_min_pkt_latency
        << "\n sink[" << node_ip << "] last_packet_out_cycle: " << stat_last_packet_out_cycle
        << "\n sink[" << node_ip << "] last_packet_in_cycle: " << stat_last_packet_in_cycle
        << "\n sink[" << node_ip << "] avg_latency: " << (stat_total_lat+0.0)/stat_packets_in
        << "\n sink[" << node_ip << "] avg_hop_count: " << (stat_hop_count+0.0)/stat_packets_in
        << endl;
    return str.str();
} /* ----- end of function GenericGenericSink::toString ----- */
#endif   /* ----- #ifndef _genericsink_cc_INC  ----- */

