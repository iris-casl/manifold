#ifndef _genericflatmc_cc_INC
#define _genericflatmc_cc_INC

#include "genericFlatMc.h"

using namespace std;

GenericFlatMc::GenericFlatMc ()
{
    name = "GenericFlatMc";
    sending = false;
    interface_connections.resize(1);	
} /* ----- end of function GenericFlatMc::GenericFlatMc ----- */

GenericFlatMc::~GenericFlatMc ()
{
    pending_packets.clear();
    pending_packets_time.clear();
    out_file.close();
    return ;
} /* ----- end of function GenericFlatMc::~GenericFlatMc ----- */

void
GenericFlatMc::setup (uint n, uint v, uint time)
{
    vcs =v;
    no_nodes = n;
    max_sim_time = time;
    address = myId();
    
    packets = 0;
    min_pkt_latency = 999999999;

    ready.resize( vcs );
    ready.insert( ready.begin(), ready.size(), false );
    for(unsigned int i = 0; i < ready.size(); i++)
        ready[i] = false;
    packets_pending = 0;

    /* send ready events for each virtual channel*/
    for( unsigned int i = 0; i < vcs; i++ )
    {
        IrisEvent* event = new IrisEvent();
        event->type = READY_EVENT;
        event->vc = 0;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,this, event);
    }

    return ;
} /* ----- end of function GenericFlatMc::setup ----- */

void
GenericFlatMc::set_output_path( string name)
{
    // open the output trace file
    stringstream str;
    str << name << "/flat_mc_" << address << "_trace_out.tr";
    out_filename = str.str();
    out_file.open(out_filename.c_str());
    if( !out_file.is_open() )
    {
        cout << "Could not open output trace file " << out_filename << ".\n";
    }
}

void
GenericFlatMc::finish ()
{
} /* ----- end of function GenericFlatMc::finish ----- */

void
GenericFlatMc::process_event (IrisEvent* e)
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
} /* ----- end of function GenericFlatMc::process_event ----- */

void
GenericFlatMc::handle_new_packet_event ( IrisEvent* e)
{
    // get the packet data
    HighLevelPacket* hlp = static_cast<HighLevelPacket*>(e->event_data.at(0));
    if(outstanding_hlp != NULL )
    {
        hlp = outstanding_hlp;
        outstanding_hlp = static_cast<HighLevelPacket*>(e->event_data.at(0));
    }

    if ( packets_pending<(0.9*MSHR_SIZE*56) && hlp)
    {

	Request* req = new Request();
	convertFromBitStream(req, hlp);
        double lat = Simulator::Now() - hlp->sent_time;
        if( min_pkt_latency > lat)
            min_pkt_latency = lat;

        // write out the packet data to the output trace file
        if( !out_file.is_open() )
            out_file.open( out_filename.c_str(), std::ios_base::app );

        if( !out_file.is_open() )
        {
            cout << "Could not open output trace file " << out_filename << ".\n";
        }
        
        out_file << hlp->toString();
        out_file << "\tPkt latency: " << lat << endl;

        // send back a ready event
        IrisEvent* event2 = new IrisEvent();
        event2->type = READY_EVENT;
        event2->vc = 0; //hlp->virtual_channel;
        Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event2);

        //Increment the response packet counter and update the sending packet time
        if ( req->cmdType != CACHE_WRITEBACK)
        {
            packets_pending++;
            hlp->sent_time = (ullint)Simulator::Now()+200;
            pending_packets.push_back(hlp);

            if (!sending)
            {
                sending = true;
                IrisEvent* event = new IrisEvent();
                event->type = OUT_PULL_EVENT;
                event->vc = 0;
                Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
            }
        }
        else
        {
            delete req;
            delete hlp;
        }
/* 
        if ( packets_pending > 0 )
        {
        _DBG(" GOT NEW PKT: can take in %d front:%d addr:%llx"
             ,packets_pending, pending_packets.front()->sent_time, pending_packets.front()->addr);
        }
        else
        {
        _DBG(" GOT NEW PKT: can take in %d ",packets_pending);
        }
 * */
        outstanding_hlp = NULL;
    }
    else
    {
        /* 
        if ( packets_pending > 0 )
        {
            _DBG(" GOT NEW PKT: cannot take in %d front:%d",packets_pending, pending_packets.front()->sent_time);
        }
        else
        {
            _DBG(" GOT NEW PKT: cannot take in %d ",packets_pending);
        }
         * */

        if(hlp)
            outstanding_hlp = hlp; //static_cast<HighLevelPacket*>(e->event_data.at(0));
        else
            outstanding_hlp = NULL;
    }

    delete e;
    return ;
} /* ----- end of function GenericFlatMc::handle_new_packet_event ----- */

void
GenericFlatMc::handle_out_pull_event ( IrisEvent* e )
{
    sending = false;
    if( ready[0] && packets_pending > 0 && pending_packets.front()->sent_time <= Simulator::Now())
    {
        /* 
        if ( packets_pending > 0)
        {
        _DBG(" Flat MC won out_pull %d front: %d src: %d",packets_pending,pending_packets.front()->sent_time, pending_packets.front()->source/3);
        }
        else
        {
            _DBG(" Flat MC won out_pull %d ",packets_pending);
        }
         * */
        packets++;
        packets_pending--;
        HighLevelPacket* hlp = pending_packets.front();
        hlp->virtual_channel = 0;

        hlp->transaction_id = 1000;
        hlp->msg_class = RESPONSE_PKT;
        hlp->destination = hlp->source/3;//pending_packets_source.front();
        hlp->source = address;
        pending_packets.pop_front();

	for ( uint i=hlp->data.size(); i < 1*max_phy_link_bits ; i++ )
            hlp->data.push_back(true);

        hlp->data_payload_length = hlp->data.size();
        hlp->stat_memory_serviced_time = 200;
        if ( Simulator::Now() > hlp->sent_time)
            hlp->waiting_in_ni = (ullint)Simulator::Now()-hlp->sent_time;
        else
            hlp->waiting_in_ni = 0;

        /* Stats */
        waiting_at_injection += hlp->waiting_in_ni;
        ready[0] = false;
        IrisEvent* event = new IrisEvent();
        event->type = NEW_PACKET_EVENT;
        event->event_data.push_back(hlp);
        Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event );
                
    }
    /*  Could have failed out pull because 1. it was blocked by the interface in
     *  which case the interface will call out pull later. 2. It has no
     *  outstanding packets in which case new packet will call out pull when
     *  it fills it. 3. Sent time is not reached. */
    else if ( !sending )
    {
        long long int time_to_send = (long long int)Simulator::Now();
        if ( packets_pending > 0 )
        if ( (pending_packets.front()->sent_time < Simulator::Now()))
        {
            /* 
        _DBG(" Failed out pull and have pending pkt to send %d front: %d src:%d ", 
                   packets_pending, pending_packets.front()->sent_time, pending_packets.front()->source/3);
        cout << " ready: " << ready[0];
             * */
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        Simulator::Schedule( pending_packets.front()->sent_time, &NetworkComponent::process_event, this, event );
        sending = true;
        }
        /* 
        else
        {
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        Simulator::Schedule( Simulator::Now()+1,&NetworkComponent::process_event, this, event );
        sending = true;

        }
         * */
    }

    delete e;
    return ;
} /* ----- end of function GenericFlatMc::handle_out_pull_event ----- */

void
GenericFlatMc::handle_ready_event ( IrisEvent* e)
{

    // send the next packet if it is less than the current time
    ready[0] = true;
#ifdef _DEBUG_TPG
    _DBG_NOARG(" handle_ready_event ");
#endif

    if( !sending && Simulator::Now() < max_sim_time )
    {
//        _DBG_NOARG(" OUT PULL since you got a ready ");
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        event->vc = 0;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
        sending = true;
    }

    if ( packets_pending>0 && (packets_pending < (0.9*MSHR_SIZE*56)) )
    {
//        _DBG_NOARG(" Gen new pkt since less there are packets_pending ");
        IrisEvent* event = new IrisEvent();
        event->type = NEW_PACKET_EVENT;
        event->vc = 0;
        event->event_data.push_back(outstanding_hlp);
        outstanding_hlp = NULL;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
    }
    delete e;
    return ;
} /* ----- end of function GenericFlatMc::handle_ready_event ----- */

string
GenericFlatMc::toString () const
{
    stringstream str;
    str << "\nGenericFlatMc: "
        << "\t vcs: " << ready.size()
        << "\t address: " <<address
        << "\t node_ip: " << node_ip
        ;
    return str.str();
} /* ----- end of function GenericFlatMc::toString ----- */

void 
GenericFlatMc::convertFromBitStream(Request* req, HighLevelPacket *hlp)
{
    req->address = 0;	
    for (unsigned int i=0; i < NETWORK_ADDRESS_BITS; i++)
    {
	req->address = req->address | (hlp->data[i] << i);
    }
    unsigned int temp = 0;
    for (unsigned int i = 0; i < NETWORK_COMMAND_BITS; i++)
    {
	temp = temp | (hlp->data[i+NETWORK_ADDRESS_BITS] << i);
    }
    req->cmdType = (cache_command)temp;

    req->threadId = 0;
    for (unsigned int i=0; i < NETWORK_THREADID_BITS; i++)
    {
	req->threadId = req->threadId | (hlp->data[i+NETWORK_ADDRESS_BITS+NETWORK_COMMAND_BITS] << i);
    }
/*     req->data.value = 0;
    if (req->cmdType == CACHE_WRITEBACK)
	for ( uint i=0 ; i < 8*WRITEBACK_SIZE ; i++ )
	{
	    req->data.value = req->data.value | (hlp->data[i+NETWORK_ADDRESS_BITS+NETWORK_COMMAND_BITS+NETWORK_THREADID_BITS] << i);
	    req->data.size = WRITEBACK_SIZE;
        }*/
}

string
GenericFlatMc::print_stats() const
{
    stringstream str;
    str << "\n\nFlatMC [" << node_ip << "] " << " packets:\t " << packets
        << "\nFlatMC [" << node_ip << "] " << " min_pkt_latency:\t" << min_pkt_latency
        << "\nFlatMC [" << node_ip << "] " << " Avg waiting_at_injection:\t" <<(waiting_at_injection+0.0)/packets 
        << endl;
        ;
    return str.str();
} /* ----- end of function GenericFlatMc::toString ----- */

#endif /* ----- #ifndef _genericflatmc_cc_INC ----- */
