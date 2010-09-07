/*
 * =====================================================================================
 *
 *       Filename:  packetSource.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/11/2010 05:12:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _packetsource_cc_INC
#define  _packetsource_cc_INC

#include	"mcFrontEnd.h"
#include	"../../../memctrl/MC.h"
#include        "../../../memctrl/response_handler.h"  
//#include	"../frontend/impl/mesh.h"

//#define SEND_ONE_PACKET 1
using namespace std;

NI::NI()
{
    name = "NI";
    //interface_connections.resize(1);
    mc = (Component*)(new MC());
    ((MC*)mc)->parent = this;
    ((MC*)mc)->ni = this; 
    ((MC*)mc)->Init();	
    ni_recv = false;
}

NI::~NI()
{
}

void
NI::set_no_vcs ( uint v)
{
    vcs = v;
}

void
NI::setup(uint n, uint v, uint time)
{

    vcs =v;
    max_sim_time = time;
    no_nodes = n;
    packets = 0;
    total_missed_time = 0;
    packets_out = 0;
    total_backward_time = 0;
    missed_time = 0;
    avg_resp_buff_occ = 0;
    resp_buff_occ_cycles = 0;
    address = myId();
    node_ip = address/3;
    last_vc = 0;
    flast_vc = 0;
    ((MC*)mc)->id = node_ip;    
    ready.resize( vcs );
    outstanding_hlp.resize( vcs );
    ready.insert( ready.begin(), ready.size(), false );
    for(unsigned int i = 0; i < vcs; i++)
        ready[i] = true;


   //  send ready events for each virtual channel

      //  IrisEvent* event = new IrisEvent();
      //  event->type = DETECT_DEADLOCK_EVENT;
      //  Simulator::Schedule( floor(Simulator::Now())+1, &NetworkComponent::process_event, this, event);
    return ;
} /* ----- end of function GenericTPG::setup ----- */

void
NI::set_output_path( string name)
{
    // open the output trace file
    stringstream str;
    str << name << "/ni_" << node_ip << "_trace_out.tr";
    out_filename = str.str();
    out_file.open(out_filename.c_str());
/*
    if( !out_file.is_open() )
    {
        cout << "Could not open output trace file " << out_filename << ".\n";
    }
*/
}

void
NI::finish()
{
    out_file.close();
}


void 
NI::process_event(IrisEvent* e)
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
        //case DETECT_DEADLOCK_EVENT:
          //  handle_detect_deadlock_event(e);
            //break;
        default:
            cout << "NI:: Unk event exception" << endl;
            break;
    }
    return ;
} /* ----- end of function GenericTPG::process_event ----- */

void NI::add_mc_bits(Request *req)
{
    //cout << "Address before adding mcbits 0x" << hex << req->address << endl;	
    ullint addr = req->address;
    unsigned int temp = MC_ADDR_BITS;   
    ullint temp2 = temp-(int)log2(no_mcs);   

    ullint lower_mask = (uint)pow(2.0,temp2*1.0)-1;
    ullint upper_mask = ~lower_mask;//(ullint)((0xFFFFFFFF)-(pow(2.0,temp*1.0)-1));

    ullint lower_addr = (addr) & lower_mask;
    ullint upper_addr = ((addr) & upper_mask) << (int)log2(no_mcs);

    vector<unsigned int>::iterator itr;
    itr = find(mc_positions.begin(), mc_positions.end(), node_ip);
    
    ullint mc_addr =((itr-mc_positions.begin()) << temp2); 

    req->address = upper_addr | lower_addr | mc_addr;
#ifdef _DEBUG
cout << endl << hex << "Adding McBits 0x" << req->address << " upper_mask 0x" << upper_mask << " lower_mask 0x" << lower_mask << " Upper Addr 0x" <<  upper_addr <<  " Lower Addr 0x" << lower_addr << " mc_addr 0x" << mc_addr << " 0x" << (itr-mc_positions.begin()) <<  endl;
#endif
}


void NI::strip_mc_bits(Request *req)
{	
    ullint addr = req->address;
    ullint temp = MC_ADDR_BITS;   
    ullint temp2 = temp-(int)log2(no_mcs);   
    ullint lower_mask = (uint)pow(2.0,temp2*1.0)-1;
#ifdef _64BIT
    ullint upper_mask = (ullint)((0xFFFFFFFFFFFF)-(pow(2.0,temp*1.0)-1));
#else
    ullint upper_mask = (ullint)((0xFFFFFFFF)-(pow(2.0,temp*1.0)-1));
#endif
    ullint lower_addr = (addr) & lower_mask;
    ullint upper_addr = ((addr) & upper_mask) >> (int)log2(no_mcs);

#ifdef GLOBAL_XOR
    short int tempBits = (int)log2(NO_OF_THREADS) - (int)log2(no_mcs);
    short int tempFactor = ((id >> tempBits) & (no_mcs-1));
    short int mc_addr = (((addr) >> temp2) & (no_mcs-1)) ^ tempFactor;
#else    
    short int mc_addr = ((addr) >> temp2) & (no_mcs-1);
#endif
    vector<unsigned int>::iterator itr;
    itr = find(mc_positions.begin(), mc_positions.end(), node_ip);
    //cout << "Before stripping address 0x" << hex << req->address;
    req->address = upper_addr | lower_addr;
#ifdef _DEBUG
cout << endl << hex << "Stripping Address 0x" << req->address << " upper_mask 0x" << upper_mask << " lower_mask 0x" << lower_mask << " Upper Addr 0x" <<  upper_addr <<  " Lower Addr 0x" << lower_addr << " mc_addr 0x" << mc_addr << " 0x" << (itr-mc_positions.begin()) <<  endl;
#endif
    assert(mc_addr== (itr-mc_positions.begin() ) );
}



void 
NI::handle_new_packet_event(IrisEvent* e)
{
    ni_recv = false;
	//cout << "Interface size " << interface_connections.size() << endl;
//    _DBG_NOARG("I reached here");
#ifdef _DEBUG
    _DBG(" NI handle_new_packet_event %s \n Int from is %s %d", e->toString().c_str(), interface_connections[0]->toString().c_str(),e->vc);
    cout << endl;	
#endif
    HighLevelPacket* hlp = NULL;
    if(e->src_id == interface_connections[0]->address)
    {   
        hlp = static_cast<HighLevelPacket*>(e->event_data.at(0));
        outstanding_hlp[hlp->virtual_channel] = hlp; //static_cast<HighLevelPacket*>(e->event_data.at(0));
    }
     
   bool found = false;;
   for( uint i=flast_vc+1; i<vcs; i++)
   if( outstanding_hlp[i]!=NULL)
   {
         hlp = outstanding_hlp[i];
         flast_vc = i;
         found = true;
         break;
   }
   if( !found)
        for( uint i=0; i<=flast_vc;i++)
            if( outstanding_hlp[i]!=NULL)
            {
                  hlp = outstanding_hlp[i];
                  flast_vc = i;
                  found = true;
                  break;
            }
 
   
   if (!((MC*)mc)->reqH->oneBufferFull && hlp!=NULL)
    {

        Request* req = new Request();
        convertFromBitStream(req, hlp);
        req->startTime = hlp->req_start_time;
        req->hop_count = hlp->hop_count;
        req->avg_network_latency = hlp->avg_network_latency;
        req->arrivalTime = hlp->recv_time;	
        total_missed_time += (ullint)(ceil(Simulator::Now()) - hlp->recv_time);
        packets++;
        last_pkt_out_cycle = (ullint)ceil(Simulator::Now());

	strip_mc_bits(req);

//	cout << "\n[" << Simulator::Now() << "] NI got packet " << hex << req->address << dec << " from Uncore" << req->threadId << endl;
        IrisEvent *e2 = new IrisEvent();
        e2->src = this;
        e2->dst = ((MC*)mc)->reqH;
        e2->event_data.push_back((void*)req);
        e2->type = START;	
        Simulator::Schedule(ceil(Simulator::Now())+1, &RequestHandler::process_event, (RequestHandler*)e2->dst, e2);	

        // send back a ready event
        IrisEvent* event = new IrisEvent();
        event->type = READY_EVENT;
        event->vc = hlp->virtual_channel;	
        Simulator::Schedule( ceil(Simulator::Now())+1, &NetworkComponent::process_event, interface_connections[0], event);	
        outstanding_hlp[hlp->virtual_channel] = NULL;
        delete hlp;
    }
    else
    {
	if(hlp)
	{
	    IrisEvent* event = new IrisEvent();
            event->type = NEW_PACKET_EVENT;
            event->event_data.push_back(hlp);
            event->src_id = address;
            event->vc = hlp->virtual_channel;
            Simulator::Schedule(floor(Simulator::Now())+ 1, 
                                &NetworkComponent::process_event, this, event);
	    ni_recv = true;
	}

/*        if(hlp!=NULL)
	{
		if (e->src_id != interface_connections[0]->address)
			cout << Simulator::Now() << " minhaj" << endl;	
		else
			cout << Simulator::Now() << " mitch" << endl;
            outstanding_hlp[hlp->virtual_channel] = hlp; //static_cast<HighLevelPacket*>(e->event_data.at(0));
	}
        else
	{
	    cout << Simulator::Now() << " Dhruv" << endl;
	    assert(e->src_id != interface_connections[0]->address);	
            for( uint i=0; i<vcs; i++)
               	outstanding_hlp[i] = NULL;
	}
*/
    }
    delete e;
    return ;
}

void 
NI::handle_out_pull_event(IrisEvent* e)
{
    bool found = false;
    uint sending_vc = -1;
    for( uint i=last_vc+1; i<vcs; i++)
        if( ready[i])
        {
            found = true;
            sending_vc = i;
            last_vc = i;
            break;
        }
    if ( !found)
    {
        for ( uint i=0; i<=last_vc; i++)
            if( ready[i])
            {
                found = true;
                sending_vc = i;
                last_vc = i;
                break;
            }
    }


    Request* req = new Request();
    if ( found && GetFromNIQueue(req))
    {
        avg_resp_buff_occ += ((MC*)mc)->responseH->responseBuffer.size();
        resp_buff_occ_cycles++;
        //        if( ((MC*)mc)->responseH->responseBuffer.size() > 64)
        //            cout << " Resp buffer exceeded Time: " << Simulator::Now() << " size: " << ((MC*)mc)->responseH->responseBuffer.size()<< endl;

        //  Send a high level packet now 
	    vector<Request>::iterator queueIndex = niQueue.begin();	
            IrisEvent* event = new IrisEvent();
            HighLevelPacket* hlp = new HighLevelPacket();
            event->type = NEW_PACKET_EVENT;
            hlp->virtual_channel = sending_vc;
            hlp->source = node_ip;
    	    hlp->recv_time = (ullint)Simulator::Now();	
            add_mc_bits(req);
            hlp->addr = req->address;
            hlp->destination = req->threadId;
            hlp->transaction_id = 1000;
            hlp->msg_class = RESPONSE_PKT;
            hlp->req_start_time = req->startTime;
            hlp->hop_count = req->hop_count;
            hlp->avg_network_latency = req->avg_network_latency;
            hlp->waiting_in_ni = (ullint)Simulator::Now() - req->retireTime;
            hlp->stat_memory_serviced_time = req->retireTime - req->arrivalTime;

            convertToBitStream(req, hlp);      
#ifdef _DEBUG
            cout << dec << "\n[" << Simulator::Now() << "] Sending packet from NI 0x" << hex << req->address << endl;
#endif
            hlp->sent_time = (ullint)Simulator::Now();
            total_backward_time += ((ullint)Simulator::Now() - req->retireTime);
            packets_out++;

            event->event_data.push_back(hlp);
            Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,interface_connections[0], event);

            ready[sending_vc] = false;
            niQueue.erase(queueIndex);

            IrisEvent* event3 = new IrisEvent();
            event3->type = SEND_TO_NI; //ask response handler for more
            event3->dst = ((MC*)mc)->responseH; //e->vc;
            Simulator::Schedule(Simulator::Now()+1, &ResponseHandler::process_event, (ResponseHandler*)event3->dst, event3);

            last_out_pull_cycle = Simulator::Now();
    }

    else
        sending = false;

    delete req;	
    delete e;
}
/*
   void 
   NI::handle_detect_deadlock_event(IrisEvent* e)
   {
   if( (Simulator::Now() - last_out_pull_cycle) > 30000 )
   {
   cout << "ERROR IN NI" << endl;
   exit(1);
   }
   e->type = DETECT_DEADLOCK_EVENT;
   Simulator::Schedule( floor(Simulator::Now())+50, &NetworkComponent::process_event, this, e);
   }*/

void 
NI::handle_ready_event(IrisEvent* e)
{
    if ( e->vc > vcs )
    {
        _DBG(" Got ready for vc %d no_vcs %d ", e->vc, vcs);
        exit(1);
    }
    //    _DBG(" NI GOT READY %d",e->vc);
    if( ready[e->vc] )
    {
        cout << " Error Recv incorrect READY !" << endl;
        exit(1);
    }
    ready[e->vc] = true;
    // send the next packet if it is less than the current time
    if( Simulator::Now() < max_sim_time )
    {
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        event->vc = e->vc;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
        sending = true;
    }
    delete e;
}


/* 
   void 
   NI::handle_old_packet_event(IrisEvent* e)
   {
#ifdef _DEBUG
cout << "NI " << address << " handle_old_packet_event " << e->vc ;
#endif

if (!((MC*)mc)->reqH->oneBufferFull)
{
HighLevelPacket* hlp = static_cast<HighLevelPacket*>(e->event_data.at(0));
Request* req = new Request();
convertFromBitStream(req, hlp);
req->arrivalTime = hlp->recv_time; 
IrisEvent *e2 = new IrisEvent();
e2->src = this;
e2->dst = (Component*)((MC*)mc)->reqH;
e2->event_data.push_back((void*)req);
e2->type = START;	
Simulator::Schedule(Simulator::Now()+1, &RequestHandler::process_event, (RequestHandler*)e2->dst, e2);

IrisEvent* event = new IrisEvent();
event->type = READY_EVENT;
VirtualChannelDescription* vc = new VirtualChannelDescription();
event->vc = 0;
event->event_data.push_back(vc);
Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event);
delete hlp;
}
else
{
IrisEvent* event2 = new IrisEvent();
event2->type = OLD_PACKET_EVENT;
event2->event_data.push_back(e->event_data.at(0));
Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event2);
}

// Send ready event back 
delete e;
}
 * */

string 
NI::toString() const
{
    stringstream str;
    str << "NI"
        << "\t addr: " << address
        << "\tOutput File= " << out_filename
        << "\tInt is " << interface_connections[0]->address
        ;
    return str.str();
}

bool
NI::GetFromNIQueue(Request* req)
{
    //    vector<Request>::iterator queueIndex = niQueue.begin();
    if (!niQueue.empty())
    {	
        *req = niQueue[0];
        //        niQueue.erase(queueIndex);
        return true;
    }
    else
        return false;
}

void 
NI::convertToBitStream(Request* req, HighLevelPacket *hlp)
{
    //cout << "HLP data " << endl;
    for ( uint i=0 ; i < NETWORK_ADDRESS_BITS ; i++ )
    {
        bool bit = (bool)((req->address >> i) & 0x1); 
        hlp->data.push_back(bit);
    }
    //cout << endl;
    /*    for ( uint i=0 ; i < NETWORK_COMMAND_BITS ; i++ )
          {
          bool bit = (bool)((req->cmdType >> i) & 0x1); 
          hlp->data.push_back(bit);
          }
          for( uint i=0 ; i < NETWORK_THREADID_BITS ; i++ )
          {
          bool bit = (bool)((req->threadId >> i) & 0x1); 
          hlp->data.push_back(bit);
          }
     */   

    if (req->cmdType == CACHE_READ || req->cmdType == CACHE_WRITE || req->cmdType == CACHE_PREFETCH)
        for ( uint i= hlp->data.size(); i < (8*CACHE_BLOCK_SIZE - max_phy_link_bits); i++ )
        {
            bool bit = false;	// sending 0's as data 
            hlp->data.push_back(bit);
        }

    hlp->data_payload_length = (ullint)ceil(hlp->data.size() *1.0 / max_phy_link_bits);
    hlp->data_payload_length = hlp->data_payload_length * max_phy_link_bits;

    for ( uint i=hlp->data.size(); i < hlp->data_payload_length; i++ )
    {
        bool bit = false; 
        hlp->data.push_back(bit);
    } 
}

void 
NI::convertFromBitStream(Request* req, HighLevelPacket *hlp)
{
    req->address = 0;	
    for (unsigned int i=0; i < NETWORK_ADDRESS_BITS; i++)
    {
        ullint bit = hlp->data[i];
        req->address = req->address | (bit << i);
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
NI::print_stats() const
{
    stringstream str;
    //    str << "\n NI addr: " << address <<endl;
    ((MC*)mc)->stats->CalculateAggregateStats();
    str << ((MC*)mc)->stats->PrintAggregateStats(node_ip);
    str << "NI [" << node_ip << "] " << "last_pkt_out_cycle:\t" << last_pkt_out_cycle << endl;
    str << "NI [" << node_ip << "] " << "total_missed_time:\t" << total_missed_time<< endl;
    str << "NI [" << node_ip << "] " << "packets_in:\t" << packets << endl;
    str << "NI [" << node_ip << "] " << "packets_out:\t" << packets_out << endl;
    str << "NI [" << node_ip << "] " << "total_backward_time:\t" << total_backward_time << endl;
    str << "NI [" << node_ip << "] " << "avg_latency_waiting_in_ni(fwd path):\t" << (total_missed_time+0.0)/packets << endl;
    str << "NI [" << node_ip << "] " << "avg_latency_waiting_in_ni(bwd path):\t" << (total_backward_time+0.0)/packets_out << endl;
    if ( resp_buff_occ_cycles != 0)
        str << "NI [" << node_ip << "] " << "Resp buff occ:\t" << avg_resp_buff_occ/resp_buff_occ_cycles<< endl;

    return str.str();
}

#endif   /* ----- #ifndef _NI_cc_INC  ----- */
