#ifndef _genericTracePktGen_cc_INC
#define _genericTracePktGen_cc_INC

#include "genericTracePktGen.h"
#include "../../../memctrl/mshr.h"



extern unsigned int MC_ADDR_BITS;
//unsigned int NUM_OF_CONTROLLERS = 8;
GenericTracePktGen::GenericTracePktGen ()
{
    name = "GenericTracePktGen";
    sending = false;
    /* Do this in setup with the address or node_ip appended */
    out_filename = "traceOut.tr";
    last_vc = 0;
//    interface_connections.resize(1);	
    ni_recv = false;
} /* ----- end of function GenericTracePktGen::GenericTracePktGen ----- */

GenericTracePktGen::~GenericTracePktGen ()
{
    out_file.close();
    mshrHandler->trace_filename.close();
    mshrHandler->mshr.clear();
    delete mshrHandler;

} /* ----- end of function GenericTracePktGen::~GenericTracePktGen ----- */

void
GenericTracePktGen::set_trace_filename( string filename )
{
    trace_name = filename;
    cout<<"TraceName: "<<trace_name<<endl;
}

void
GenericTracePktGen::set_no_vcs ( uint v)
{
    vcs = v;
}

void
GenericTracePktGen::setup (uint n, uint v, uint time)
{
    vcs =v;
    no_nodes = n;
    max_sim_time = time;
    address = myId();
    node_ip = address/3;
    last_vc = 0;
    
    packets = 0;
    packets_in = 0;
    min_pkt_latency = 999999999;
    last_packet_out_cycle = 0;
    fwd_path_delay = 0;
    roundTripLat = 0;
    stat_round_trip_hop_count = 0;
    stat_round_trip_network_latency = 0;
    stat_round_trip_memory_latency = 0;
    stat_waiting_in_ni = 0;
    TotalBLP = 0;	

    ready.resize( vcs );
    ready.insert( ready.begin(), ready.size(), false );
    for(unsigned int i = 0; i < ready.size(); i++)
        ready[i] = false;
    lastSentTime = 0;
    Addr_t addr;
    uint thread_id;
    Time time2;
    uint cmd;	

//    InitMSHR();
#ifdef USE_MSHR    
    mshrHandler = new MSHR_H();	
    mshrHandler->id = node_ip;
    mshrHandler->parent = this; 
#endif    
 //   mc->stats->doneOnce[i] = &mshrHandler[i].done; 
    trace_filename = &mshrHandler->trace_filename; 
    mshrHandler->filename = const_cast<char*>(trace_name.c_str());	

    trace_filename->open(trace_name.c_str(),ios::in);
    if(!trace_filename->is_open())
    {
        cout << "Err opening trace " << trace_name << endl;
        exit(1);
    }

#ifdef USE_MSHR
    if (!trace_filename->eof())
    {
	(*trace_filename) >> hex >> addr;
	(*trace_filename) >> thread_id;	
	(*trace_filename) >> dec >> time2;
	(*trace_filename) >> dec >> cmd;

	addr= addr & 0xffffffc0;
	
	Request *req2 = new Request();
	req2->cmdType = (cache_command)cmd;
	req2->address = addr;
	req2->arrivalTime = time2;
	req2->threadId = node_ip;
	req2->address = mshrHandler->GlobalAddrMap(req2->address,req2->threadId);

	IrisEvent *event = new IrisEvent();
    	event->src = this;
    	event->dst = (Component*)(mshrHandler);
        event->type = 0;
    	event->event_data.push_back(req2);	
#ifdef DEEP_DEBUG
    	cout << dec << Simulator::Now() << ": " << hex << req2->address << ": First Request of each trace to be send to mshrs" << endl;	
#endif
    	Simulator::Schedule(time2, &MSHR_H::process_event, (MSHR_H*)event->dst, event);   
    }
    /* send ready events for each virtual channel*/
#endif

    for( unsigned int i = 0; i < vcs; i++ )
    {
        IrisEvent* event = new IrisEvent();
        event->type = READY_EVENT;
        event->vc = i;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,this, event);
// interface_connections[0], event);
    }


    return ;
} /* ----- end of function GenericTracePktGen::setup ----- */

void
GenericTracePktGen::set_output_path( string name)
{
    // open the output trace file
    stringstream str;
    str << name << "/tpg_" << node_ip << "_trace_out.tr";
    out_filename = str.str();
    out_file.open(out_filename.c_str());
/*
    if( !out_file.is_open() )
    {
        stringstream stream;
        stream << "Could not open output trace file " << out_filename << ".\n";
        timed_cout(stream.str());
    }
*/
}

void
GenericTracePktGen::finish ()
{
    out_file.close();
    trace_filename->close();
    return ;
} /* ----- end of function GenericTracePktGen::finish ----- */

void
GenericTracePktGen::process_event (IrisEvent* e)
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
} /* ----- end of function GenericTracePktGen::process_event ----- */

void
GenericTracePktGen::handle_new_packet_event ( IrisEvent* e)
{
    ni_recv = false;
    // get the packet data
    HighLevelPacket* hlp = static_cast< HighLevelPacket* >( e->event_data.at(0));
    double lat = Simulator::Now() - hlp->sent_time;
    last_packet_out_cycle = Simulator::Now();
    if( min_pkt_latency > lat)
        min_pkt_latency = lat;
    packets_in++;
    stat_round_trip_hop_count += hlp->hop_count;
    stat_round_trip_network_latency += (ullint)hlp->avg_network_latency;
    stat_round_trip_memory_latency += hlp->stat_memory_serviced_time;
    stat_waiting_in_ni += hlp->waiting_in_ni;
#ifdef DEBUG
    _DBG( "-------------- TPG GOT NEW PACKET ---------------\n pkt_latency: %f", lat);
    // write out the packet data to the output trace file
    if( !out_file.is_open() )
        out_file.open( out_filename.c_str(), std::ios_base::app );
      
        out_file << hlp->toString();
        out_file << "\tPkt latency: " << lat << endl;
#endif

#ifdef USE_MSHR
    Request * req = new Request();
    convertFromBitStream(req,hlp);		
    req->startTime = hlp->req_start_time;
    IrisEvent *event = new IrisEvent();
    event->src = (Component*)this;
    event->dst = (Component*)mshrHandler;
    event->type = MSHR_DELETE;	
    event->event_data.push_back(req);	
#ifdef DEBUG   
    _DBG( " Got at TPG newpkt: %llx",req->address);
#endif

#ifdef _DEEP_DEBUG
    	cout << dec << Simulator::Now() << ": " << hex << req->address << ": Sending to MSHR for Delete" << endl;	
#endif
    Simulator::Schedule(Simulator::Now()+1, &MSHR_H::process_event, (MSHR_H*)event->dst, event);  	
#endif

    delete hlp;

    // send back a ready event
    IrisEvent* event2 = new IrisEvent();
    event2->type = READY_EVENT;
    event2->vc = hlp->virtual_channel;
    Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event2);


    delete e;
    return ;
} /* ----- end of function GenericTracePktGen::handle_new_packet_event ----- */

void
GenericTracePktGen::handle_out_pull_event ( IrisEvent* e )
{
    sending =false;
    bool found = false;
    uint sending_vc = -1;
    uint old_last_vc = last_vc;
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
#ifdef USE_MSHR
        Request* next_req = GetRequest();
#else
        Request* next_req = GetNextRequest();
#endif
        if( next_req)
        {
            packets++;
            fwd_path_delay += (ullint)(Simulator::Now() - next_req->arrivalTime);
            HighLevelPacket* hlp = new HighLevelPacket();
            hlp->virtual_channel = sending_vc;
            hlp->source = node_ip;
            hlp->destination = next_req->mcNo;
            hlp->addr = next_req->address;
            hlp->transaction_id = 1000;
            if( hlp->destination == node_ip )
                hlp->destination = 3; //(hlp->destination + 1) % max_nodes;

            convertToBitStream(next_req, hlp);
            if(hlp->sent_time < Simulator::Now())
                hlp->sent_time = (ullint)Simulator::Now()+1;
            hlp->req_start_time = next_req->startTime;

#ifdef _DEEP_DEBUG
            assert ( hlp->destination < no_nodes);
            _DBG( " Sending at TPG out pull: VC %d dest:%d ad:%llx mshr size: %d", sending_vc, hlp->destination
                  ,next_req->address, mshrHandler->mshr.size());
            cout << "HLP: " << hlp->toString() << endl;
            _DBG( " Sending pkt: no of packets %d %llx", packets, next_req->address );
#endif
            ready[sending_vc] = false;
            IrisEvent* event = new IrisEvent();
            event->type = NEW_PACKET_EVENT;
            event->event_data.push_back(hlp);
            event->vc = sending_vc;
            Simulator::Schedule( hlp->sent_time, &NetworkComponent::process_event, interface_connections[0], event );
            sending = true;
            delete next_req;
            lastSentTime = (ullint)Simulator::Now();

        }
        else
        {
            last_vc = old_last_vc;
            sending = true;
        }
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
GenericTracePktGen::handle_ready_event ( IrisEvent* e)
{

    if ( e->vc > vcs )
    {
        _DBG(" Got ready for vc %d no_vcs %d ", e->vc, vcs);
        exit(1);
    }
    // send the next packet if it is less than the current time
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
} /* ----- end of function GenericTracePktGen::handle_ready_event ----- */
Request*
GenericTracePktGen::GetNextRequest()
{
    Addr_t addr =0;
    uint thread_id = 0;
    Time time = 0;
    uint cmd;

    if (trace_filename->eof())
        return NULL;

    else
    {
        Request* tempRequest = new Request();
        (*trace_filename) >> hex >> addr;
        (*trace_filename) >> thread_id;
        (*trace_filename) >>dec>> time;
        (*trace_filename) >>dec>> cmd;

        tempRequest->cmdType = (cache_command)cmd;
        tempRequest->address = addr;
        tempRequest->arrivalTime = time;	
        tempRequest->threadId = node_ip;
        tempRequest->address = mshrHandler->GlobalAddrMap(tempRequest->address,tempRequest->threadId);
#ifdef _DEEP_DEBUG
        cout << "\n Created Req: "
            << "\t cmd: " << cmd
            << "\t thr_id: " << thread_id
            << "\t time: " << time
            << "\t addr: " << addr
            << endl;
#endif

        return tempRequest;
    }

}

bool
GenericTracePktGen::GetNewRequest(Request *req)
{
    if (!mshrHandler->writeQueue.empty())
    {
        if (mshrHandler->writeQueue[0].arrivalTime <= Simulator::Now())
        {
            vector<Request>::iterator bufferIndex = mshrHandler->writeQueue.begin();
            *req = mshrHandler->writeQueue[0];
            //req->startTime = Simulator::Now();	
            mshrHandler->writeQueue.erase(bufferIndex);
            return true;
        }
    }
    else  if (!mshrHandler->mshr.empty() && mshrHandler->lastScheduledIndex < mshrHandler->mshr.size())
    {
        if (mshrHandler->mshr[mshrHandler->lastScheduledIndex].arrivalTime <= Simulator::Now())
        {
            *req = mshrHandler->mshr[mshrHandler->lastScheduledIndex];
            mshrHandler->lastScheduledIndex++;
//            TotalBLP += mshrHandler->countBLP(*req);
            return true;
        }
    }
    if (mshrHandler->lastScheduledIndex > mshrHandler->mshr.size())
    {
        cout << "\nERROR: lastIndex greater" << mshrHandler->lastScheduledIndex << " " << mshrHandler->mshr.size() << endl;
        exit(1);
    }
    return false;
}

Request*
GenericTracePktGen::GetRequest()
{
    Request* req = new Request();
    if (GetNewRequest(req))
    {
        return req;
    }
    delete req;
    return NULL;
}

void
GenericTracePktGen::convertToBitStream(Request* req, HighLevelPacket* hlp)
{
    for ( uint i=0 ; i < NETWORK_ADDRESS_BITS ; i++ )
    {
        bool bit = (bool)((req->address >> i) & 0x1);
        hlp->data.push_back(bit);
    }
    for ( uint i=0 ; i < NETWORK_COMMAND_BITS ; i++ )
    {
        bool bit = (bool)((req->cmdType >> i) & 0x1);
        hlp->data.push_back(bit);
    }
    for ( uint i=0 ; i < NETWORK_THREADID_BITS ; i++ )
    {
        bool bit = (bool)((req->threadId >> i) & 0x1);
        hlp->data.push_back(bit);
    }
    if (req->cmdType == CACHE_WRITEBACK)
    {	for ( uint i = hlp->data.size() ; i < (8*CACHE_BLOCK_SIZE - max_phy_link_bits); i++ )
        {
            hlp->data.push_back(true);
        }
        hlp->msg_class = WRITE_REQ;
    }
    else
        hlp->msg_class = ONE_FLIT_REQ;


    hlp->data_payload_length = uint(ceil(hlp->data.size() *1.0 / max_phy_link_bits));	
    hlp->data_payload_length = hlp->data_payload_length * max_phy_link_bits;
    hlp->sent_time = req->arrivalTime;
    for ( uint i=hlp->data.size() ; i < hlp->data_payload_length; i++ )
        hlp->data.push_back(false);

}


void 
GenericTracePktGen::convertFromBitStream(Request* req, HighLevelPacket *hlp)
{
    req->address = 0;	
    req->mcNo=hlp->source;
    for (unsigned int i=0; i < NETWORK_ADDRESS_BITS; i++)
    {
        req->address = req->address | (hlp->data[i] << i);
    }
    /*    unsigned int temp = 0;
          for (unsigned int i = 0; i < NETWORK_COMMAND_BITS; i++)
          {
          temp = temp | (hlp->data[i+NETWORK_ADDRESS_BITS] << i);
          }
          req->cmdType = (Command_t)temp;

          req->threadId = 0;
          for (unsigned int i=0; i < NETWORK_THREADID_BITS; i++)
          {
          req->threadId = req->threadId | (hlp->data[i+NETWORK_ADDRESS_BITS+NETWORK_COMMAND_BITS] << i);
          }
     */

    req->data.value = 0;
    if (req->cmdType == CACHE_READ || req->cmdType == CACHE_WRITE || req->cmdType == CACHE_PREFETCH)
        for ( uint i = NETWORK_ADDRESS_BITS; i < (8*CACHE_BLOCK_SIZE - max_phy_link_bits); i++ )
        {
            req->data.value = req->data.value | (hlp->data[i+NETWORK_ADDRESS_BITS] << i);
            req->data.size = CACHE_BLOCK_SIZE;
        }
}

string
GenericTracePktGen::toString () const
{
    stringstream str;
    str << "\nGenericTracePktGen: "
        << "\t trace: " << trace_name
        << "\t vcs: " << ready.size()
        << "\t address: " <<address
        << "\t node_ip: " << node_ip
        ;
    return str.str();
} /*  ----- end of function GenericTracePktGen::toString ----- */

string
GenericTracePktGen::print_stats() const
{
    stringstream str;
    str << "\nTPG [" << node_ip << "] packets_out:\t" << packets
        << "\nTPG [" << node_ip << "] avg_fwd_path_delay:\t" << (fwd_path_delay+0.0)/packets
        << "\nTPG [" << node_ip << "] min_pkt_latency:\t" << min_pkt_latency
        << "\nTPG [" << node_ip << "] last_packet_out_cycle:\t" << last_packet_out_cycle
        << "\nTPG [" << node_ip << "] packet_in:\t" << packets_in
        << "\nTPG [" << node_ip << "] avg_round_trip_latency:\t" << (roundTripLat+0.0)/packets_in
        << "\nTPG [" << node_ip << "] avg_blp:\t" << (TotalBLP+0.0)/packets
        << "\nTPG [" << node_ip << "] total_unsink:\t" << mshrHandler->unsink
        << "\nTPG [" << node_ip << "] avg_round_trip_hop_count:\t" << (stat_round_trip_hop_count+0.0)/packets_in
        << "\nTPG [" << node_ip << "] avg_round_trip_network_latency:\t" << (stat_round_trip_network_latency+0.0)/packets_in
        << "\nTPG [" << node_ip << "] avg_round_trip_memory_latency:\t" << (stat_round_trip_memory_latency+0.0)/packets_in
        << "\nTPG [" << node_ip << "] avg_round_trip_waiting_in_ni:\t" << (stat_waiting_in_ni+0.0)/packets_in
        ;
    return str.str();
} /* ----- end of function GenericTracePktGen::toString ----- */

#endif /* ----- #ifndef _genericTracePktGen_cc_INC ----- */
