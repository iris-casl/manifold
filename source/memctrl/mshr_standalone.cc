/*
 * =====================================================================================
 *
 *       Filename:  mshr.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/12/2010 12:43:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  mshr_standalone_INC
#define  mshr_standalone_INC

#include <math.h>
#include "mshr_standalone.h"
#include "../simIris/components/impl/genericEvents.h"

using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  MSHR_SA_H
 *      Method:  MSHR_SA_H
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

MSHR_SA_H::MSHR_SA_H()
{
    unsink = 0;
    globalUnSink = 0;
    lastFinishTime = 0;	
    done = 0;
    lastScheduledIndex = 0;	
    mshr.clear();
    writeQueue.clear();
    waiting = false;
    lastFullTime = 0;			
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  MSHR_SA_H
 *      Method:  ~MSHR_SA_H
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
MSHR_SA_H::~MSHR_SA_H()
{
     mshr.clear();
     writeQueue.clear();		
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  MSHR_SA_H
 *      Method:  process_event
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void MSHR_SA_H::process_event(IrisEvent* e)
{
    Addr_t addr;
    uint thread_id;
    Time time;
    uint cmd;
    if (e->type == MSHR_DELETE)
    {
	Request * req = new Request();
	*req = *((Request*)e->event_data.at(0));
	DeleteInMSHR(req);	
//	cout << "I came here in MSHR DELETE with " << hex << req->address << "  " << id << endl; 

	if (waiting)    	
	{   
	    unsink += (ullint)(Simulator::Now()-lastFullTime);
	    *req = waitingForMSHR;
	    waiting = false;	
	
		   	IrisEvent *event = new IrisEvent();	
    	    event->src = (Component*)this;
    	    event->dst = (Component*)this;
            event->type = 1120;
    	    event->event_data.push_back(req);	
    	    Simulator::Schedule(Simulator::Now()+1, &MSHR_SA_H::process_event, (MSHR_SA_H*)event->dst, event);
	}
        else
            delete req;
    }		
    else if (mshr.size() >= MSHR_SIZE && !waiting)
    {
	lastFullTime = Simulator::Now();
	waitingForMSHR = *((Request*)e->event_data.at(0));
	waiting = true;

#ifdef DEEP_DEBUG
    	cout << dec << Simulator::Now() << ": " << hex << waitingForMSHR.address << ": MSHR Full Now Waiting " << endl;	
#endif
    }
    else if (!waiting)
    {
	Request * req = new Request();
	*req = *((Request*)e->event_data.at(0));	//TODO needs to set this through manifold kernel's links
        assert( req->arrivalTime <= max_sim_time+1);
	req->startTime = Simulator::Now();
	if (req->cmdType!=CACHE_WRITEBACK)
	    mshr.push_back(*req);
	else
	    writeQueue.push_back(*req);	 
	
	    	
#ifdef DEEP_DEBUG
    	cout << dec << Simulator::Now() << ": " << hex << req->address << ": " << id << " PUSHED IN MSHR of size " << mshr.size() << endl;
#endif
	delete req;
	if (!trace_filename.eof())
	{
	    trace_filename >> hex >> addr;
	    trace_filename >> thread_id;	
	    trace_filename >> dec >> time;
	    trace_filename >> dec >> cmd; 

//	    if (Simulator::Now() > 100000)
//		done = 1;		
	    
	    time = time+lastFinishTime;
	    if (trace_filename.eof())
	    {
	    	if (done == 0)
	    	{
		    done = 1;			//TODO needs to set this
	//	    cout << "thread done " << id << endl;
	    	}	
	    	lastFinishTime = Simulator::Now();
	    	globalUnSink += unsink;
	    	unsink = 0;
	    	cout << dec << "lastFinishTime = " << lastFinishTime << ", " << id << ", " << globalUnSink << endl;	

	    	trace_filename.close();	
	    	trace_filename.open(filename,ios::in);
    	    	if(!trace_filename.is_open())
    	    	{
	    	    cout<<"Err opening trace"<<endl;
	   	    exit(1);
    	    	}	
	   
	    	if (!trace_filename.eof())
	    	{ 
            	    trace_filename >> hex >> addr;
	    	    trace_filename >> thread_id;	
	    	    trace_filename >> dec >> time;
	    	    trace_filename >> dec >> cmd;
		
		    if (!trace_filename.eof())
		    	time = time+lastFinishTime;	
	    	}
	    }
	}
	
	
	

    	Request *req2 = new Request();
	req2->cmdType = (cache_command)cmd;
	req2->address = addr;
	req2->arrivalTime = time+unsink;
	req2->threadId = id;
	req2->address = GlobalAddrMap(req2->address,id);
	IrisEvent *event = new IrisEvent();
    	event->src = (Component*)this;
    	event->dst = (Component*)this;
    	event->event_data.push_back(req2);
	event->type = 0;	
    	Simulator::Schedule(time+unsink, &MSHR_SA_H::process_event, (MSHR_SA_H*)event->dst, event);
	nextReq = *req2; 
	
/*		 
	    IrisEvent *event3 = new IrisEvent();	
    	    event3->src = (Component*)this;
    	    event3->dst = (Component*)parent;
	    event3->type = OUT_PULL_EVENT;	
    	    Simulator::Schedule(Simulator::Now()+1, &GenericTPG::process_event, (GenericTPG*)event3->dst, event3);
*/    }
    else
	cout << "I should never come in else of MSHR" << endl;	
    delete (Request*)e->event_data.at(0);	
    delete e;
}

Addr_t MSHR_SA_H::GlobalAddrMap(Addr_t addr, uint threadId)
{
    Addr_t newAddr;
    unsigned int threadBits = ceil(log2(NO_OF_THREADS));
    unsigned int threadBitsPos = THREAD_BITS_POSITION;
    Addr_t lowerMask = (Addr_t)pow(2.0,threadBitsPos*1.0)-1;
    Addr_t upperMask = 0xFFFFFFFF << threadBitsPos;
    Addr_t lowerAddr = addr & lowerMask;
    Addr_t upperAddr = addr & upperMask;
    Addr_t threadAddr = (threadId & ((Addr_t)pow(2.0,threadBits*1.0)-1));
    newAddr = lowerAddr | (threadAddr << threadBitsPos)| (upperAddr << threadBits);
//    cout << hex << addr << ", " << threadId << ", " << newAddr << endl;	
    return newAddr;
}

void MSHR_SA_H::DeleteInMSHR(Request* req)
{
    vector<Request>::iterator index = mshr.begin();
    for (unsigned int i=0; i<mshr.size(); i++)
    {
	if (mshr[i].address == req->address)
	{	
	    mshr.erase(i+index);
	    lastScheduledIndex--;		
	//    ((GenericTPG*)parent)->roundTripLat += (Simulator::Now() - mshr[i].startTime);	 
#ifdef DEEP_DEBUG
	    cout << Simulator::Now() << hex << ": Deletion Time of Request " << req->address << dec << ", of Thread " << id << ", lastScheduledIndex = " << lastScheduledIndex << endl;
#endif	
	     break;
	}
     }
     cout << dec;	
}

void MSHR_SA_H::demap_addr(Addr_t oldAddress, Addr_t newAddress)
{
    vector<Request>::iterator index = mshr.begin();
    for (unsigned int i=0; i<mshr.size(); i++)
    {
	if (mshr[i].address == oldAddress)
	{
            mshr[i].address = newAddress;
#ifdef DEEP_DEBUG
	    cout << Simulator::Now() << hex << ": Replace Address " << oldAddress << " in MSHR of Thread " << id << " to " << mshr[i].address << endl;
#endif	
            //cout << hex <<  oldAddress << " hihi " << newAddress << endl;
	     break;
	}
     }

}
#endif   /* ----- #ifndef mshr_standalone_INC  ----- */
