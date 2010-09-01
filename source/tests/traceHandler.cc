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

#include	"MersenneTwister.h"
#include	"traceHandler.h"
//#define SEND_ONE_PACKET 1
using namespace std;

TraceHandler::TraceHandler(char* traceName)
{
    name = "TraceHandler";
    interface_connections.resize(1);
    cout<<"TraceName: "<<traceName<<endl;
    trace_filename.open(traceName,ios::in);
    if(!trace_filename.is_open())
    {
	cout<<"Err opening trace"<<endl;
	exit(1);
    }
    InitNextRequest();
	
}

TraceHandler::~TraceHandler()
{
}

void 
TraceHandler::setup()
{
    address = myId();
    no_of_packets = 0;
    out_filename = "rpg_trace_out.tr";
    in_filename = "rpg_trace_in.tr";
    out_file.open(out_filename.c_str());
    in_file.open(in_filename.c_str());
    last_vc = 0;
    ready.resize(DEFAULT_VIRTUAL_CHANNELS);

    /* These are the values for the init of the distribution */
    seed = 12345;
    max_address = 1024;
    min_delay = 5;
    max_delay = 100;
    min_length = 3;
    max_length = 10;
    hotspots = 7; 
    max_time = DEFAULT_RAN_MAX_TIME;

    stringstream str;
    str << " Running setup on TraceHandler " << Simulator::Now() << endl;
    timed_cout( str.str());

    /* Init the generator here */

    for ( uint i=0 ; i<ready.size() ; i++ )
    {
        IrisEvent* event = new IrisEvent();
        VirtualChannelDescription* vc = new VirtualChannelDescription();
        vc->vc = i;
        event->event_data.push_back(vc);
        event->type = READY_EVENT;
        Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event);
    }

    for ( uint i=0 ; i<ready.size() ; i++ )
        ready[i] = false;

     int x = 1;
     IrisEvent* event = new IrisEvent();
     event->vc = 1;	
     event->type = OUT_PULL_EVENT;
//     Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, this, event);	
}

void 
TraceHandler::finish()
{
    out_file.close();
}

void 
TraceHandler::process_event(IrisEvent* e)
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
            cout << "TraceHandler:: Unk event exception" << endl;
            break;
    }
}

void 
TraceHandler::handle_out_pull_event(IrisEvent* e)
{
//#ifdef _DEBUG
    stringstream str;
    str << "TraceHandler " << address << " handle_out_pull_event " << e->vc ;
    timed_cout( str.str());
//#endif

    Request * req = new Request();
    if (GetNextRequest(req))
    {
    	uint no_vcs = ready.size();
    	bool found = false;

    
    	//  Make sure you start from last_vc 
    	for ( uint i=last_vc ; i<last_vc+1 ; i++ )
            if ( ready[i] )
            {
            	found = true;
            	last_vc = i;
            	break;
            }

    	if ( !found )
            for ( uint i=0 ; i<last_vc+1 ; i++ )
                if ( ready[i] )
            	{
                    found = true;
                    last_vc = i;
                    break;
            	}
   	 //  End of finding vc 

	    MTRand mtrand1;
    	if ( found )
    	{
            /*  Send a high level packet now */
            IrisEvent* event = new IrisEvent();
            HighLevelPacket* hlp = new HighLevelPacket();
            event->type = NEW_PACKET_EVENT;
            hlp->virtual_channel = last_vc;
            hlp->source = myId();

            /* Random packet generator should change this to match the
             * distribution */
            if( node_ip == 1 ) 
            	hlp->destination = 0;
            else
            	hlp->destination = 1;
	
	    convertToBitStream(req, hlp);      

            hlp->transaction_id = mtrand1.randInt(100);
            hlp->sent_time = Simulator::Now();

            in_file << hlp->toString() << endl;
            event->event_data.push_back(hlp);
            Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event,interface_connections[0], event);

            stringstream str;
            str << "TraceHandler " << address << " Generated HLP: " << hlp->toString() << Simulator::Now() << " no_of_packets: " << ++no_of_packets << endl;
            timed_cout( str.str() );

            seed++;
            ready[hlp->virtual_channel] = false;
            found = false;

            for ( uint i=0 ; i<no_vcs ; i++ )
                if ( ready[i] ) //&& Simulator::Now() < max_time)
                {
                	found = true;
                	break;
                }
            if ( found )
            {
            	IrisEvent* event = new IrisEvent();
            	event->type = OUT_PULL_EVENT;
            	Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, this, event);
            	sending = true;
            }
            else
            	sending = false;
        }
        else
            sending = false;
    }
    else 
    {
	IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
	event->vc = e->vc;
        Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, this, event);
        sending = false;
    }
    delete req;		
    delete e;
}

void 
TraceHandler::handle_ready_event(IrisEvent* e)
{
#ifdef _DEBUG
    stringstream str;
    str << "TraceHandler " << address << " handle_ready_event " << e->vc ;
    timed_cout( str.str());
#endif

//    if( no_of_packets < 2)
//    {
    /* Send the next packet if it is time */
    VirtualChannelDescription* vc = static_cast<VirtualChannelDescription*>(e->event_data.at(0));
    ready[vc->vc] = true;

    if( !sending && Simulator::Now() < max_time )
    {
        IrisEvent* event = new IrisEvent();
        event->type = OUT_PULL_EVENT;
        sending = true;
        Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, this, event);
    }
//    }
    delete e;
}

void 
TraceHandler::handle_new_packet_event(IrisEvent* e)
{
#ifdef _DEBUG
    stringstream str;
    str << "TraceHandler " << address << " handle_new_packet_event " << e->vc ;
    timed_cout( str.str());
#endif

    HighLevelPacket* hlp = static_cast<HighLevelPacket*>(e->event_data.at(0));
    cout << "\n\nRPG*********** GOT NEW PACKET *******************" << endl;
    cout << hlp->toString() << " Recv time: " << Simulator::Now() 
        << " pkt_latency: " << Simulator::Now() - hlp->sent_time << endl;
    out_file << hlp->toString() << " Recv time: " << Simulator::Now() 
        << " pkt_latency: " << Simulator::Now() - hlp->sent_time << endl;

    /* Send ready event back to NI */
    IrisEvent* event = new IrisEvent();
    event->type = READY_EVENT;
    VirtualChannelDescription* vc = new VirtualChannelDescription();
    vc->vc = hlp->virtual_channel;
    event->event_data.push_back(vc);
    Simulator::Schedule(Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event);
    delete hlp;
    delete e;
}

string 
TraceHandler::toString() const
{
    stringstream str;
    str << "TraceHandler"
        << "\t addr: " << address
        << "\tInput File= " << in_filename
        << "\tOutput File= " << out_filename;
    return str.str();
}

void
TraceHandler::InitNextRequest()
{
    Addr_t addr =0;
    UInt thread_id = 0;
    Time time = 0;
    UInt cmd;
    Request tempRequest;

    if (trace_filename.eof())
	return;

    trace_filename >> hex >> addr;
    trace_filename>> thread_id;	
    trace_filename>>dec>> time;
    trace_filename>>dec>> cmd;
//  trace_filename >> hex >> addr;
	
    tempRequest.cmdType = (Command_t)cmd;
    tempRequest.address = addr;
    tempRequest.arrivalTime = time;
    tempRequest.threadId = thread_id;
    nextRequest.push_back(tempRequest);
	
    return;	
}
bool
TraceHandler::GetNextRequest(Request* req)
{
    Addr_t addr =0;
    UInt thread_id = 0;
    Time time = 0;
    UInt cmd;
    Request tempRequest;	
    vector<Request>::iterator index = nextRequest.begin();

    if (!nextRequest.empty())
    {
	if (nextRequest[0].arrivalTime > Simulator::Now())
	    return false;

	*req = nextRequest[0];
	req->arrivalTime = Simulator::Now();
	nextRequest.erase(index);
#ifdef DEBUG	
	cout << dec << Simulator::Now() << ": " << hex << req->address << ": I am in main just being sent from file\n"; 
#endif

        if (trace_filename.eof())
	    return false;

    	trace_filename >> hex >> addr;
    	trace_filename>> thread_id;	
    	trace_filename>>dec>> time;
    	trace_filename>>dec>> cmd;
	
    	tempRequest.cmdType = (Command_t)cmd;
    	tempRequest.address = addr;
    	tempRequest.arrivalTime = time;
    	tempRequest.threadId = thread_id;
	nextRequest.push_back(tempRequest);
	
	return true;
    }
    else
	return false;
    
} 

void 
TraceHandler::convertToBitStream(Request* req, HighLevelPacket *hlp)
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
	bool bit = (bool)((req->cmdType >> i) & 0x1); 
        hlp->data.push_back(bit);
    }
    if (req->cmdType == CACHE_WRITEBACK)
	for ( uint i=0 ; i < 8*WRITEBACK_SIZE ; i++ )
	{
	    bool bit = true;	// sending 1's as data 
            hlp->data.push_back(bit);
        }
	        
    hlp->data_payload_length = ceil(hlp->data.size() *1.0 / max_phy_link_bits);
    hlp->data_payload_length = hlp->data_payload_length * max_phy_link_bits;

    for ( uint i=0 ; i < hlp->data_payload_length - hlp->data.size(); i++ )
    {
	bool bit = false; 
        hlp->data.push_back(bit);
    } 
}

void 
TraceHandler::convertFromBitStream(Request* req, HighLevelPacket *hlp)
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
    req->cmdType = (Command_t)temp;
	
    req->threadId = 0;
    for (unsigned int i=0; i < NETWORK_THREADID_BITS; i++)
    {
	req->threadId = req->threadId | (hlp->data[i+NETWORK_ADDRESS_BITS+NETWORK_COMMAND_BITS] << i);
    }
    req->data.value = 90;
    if (req->cmdType == CACHE_WRITEBACK)
	for ( uint i=0 ; i < 8*WRITEBACK_SIZE ; i++ )
	{
	    req->data.value = req->data.value | (hlp->data[i+NETWORK_ADDRESS_BITS+NETWORK_COMMAND_BITS+NETWORK_THREADID_BITS] << i);
	    req->data.size = CACHE_WRITEBACK;
        }
}

#endif   /* ----- #ifndef _traceHandler_cc_INC  ----- */
