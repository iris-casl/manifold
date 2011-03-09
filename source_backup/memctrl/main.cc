/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 04:39:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include        <string>
#include        <sstream>
#include        <iostream>
#include 	<iomanip>
#include        <cstdlib>
#include 	<fstream>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../data_types/impl/irisEvent.h"

#include        "../MemCtrl/request.h"
#include	"../MemCtrl/request_handler.h" 
#include        "../MemCtrl/bus_handler.h"
#include        "../MemCtrl/bus.h"
#include        "../MemCtrl/dram.h"
#include        "../MemCtrl/NI.h"
#include	"../MemCtrl/refresh_manager.h"
#include	"../MemCtrl/response_handler.h"
#include	"../MemCtrl/MC.h"
#include	"../MemCtrl/mshr.h"

using namespace std;

bool GetNextRequest(MSHR_H mshrHandler[], Request *req, unsigned int *index)
{
    static int lastThreadIndex = NO_OF_THREADS -1;
    Time time[NO_OF_THREADS];	
    bool temp = false;	
    bool mshrFilled[NO_OF_THREADS];
    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    {
        mshrFilled[i] = true;
	time[i] = -1;
	if (!mshrHandler[i].mshr.empty())
	{	
	    if ( mshrHandler[i].lastScheduledIndex < mshrHandler[i].mshr.size() )
	    {
	        time[i] = mshrHandler[i].mshr[mshrHandler[i].lastScheduledIndex].arrivalTime;
	    	temp = true;
		mshrFilled[i] = false;
		break;
	    }				
	}
	if (!mshrHandler[i].writeQueue.empty())
	{	
	    time[i] = mshrHandler[i].writeQueue[0].arrivalTime;
	    temp = true;
	    break;
	}		
    }
//    cout << Simulator::Now() << ":  " << temp << endl;	
    if (!temp)
	return temp;	
    unsigned int minTime = -1;    
    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    {
	unsigned int rr = (i+lastThreadIndex+1)%NO_OF_THREADS;

	if (!mshrHandler[rr].writeQueue.empty())
	{	
	    vector<Request>::iterator bufferIndex = mshrHandler[rr].writeQueue.begin();	
	    *req = mshrHandler[rr].writeQueue[0];
	    mshrHandler[rr].writeQueue.erase(bufferIndex);
	    lastThreadIndex = rr;	
	    return true;
	}
//	cout << dec << time[i] << " "; 	
	else if (time[rr] < minTime && !mshrFilled[rr])
	{ 	  
	    minTime = time[rr];
	    *index = rr;
	    lastThreadIndex = *index;			
	    *req = mshrHandler[rr].mshr[mshrHandler[rr].lastScheduledIndex];
	    temp = true;			
	}
    }
    mshrHandler[*index].lastScheduledIndex++;    			
    return temp;		  	
}

int main(int argc, char **argv)
{
/*
    cout << fixed << setprecision(0);
    cout<<"TraceName: "<<argv[1]<<endl;
    MC *mc = new MC(); 
    Addr_t addr =0;
    UInt thread_id = 0;
    Time time = 0;
    UInt cmd;
    Time unsink = 0;
    vector <Request> mshr; 
    mc->ni->mshr = &mshr;	
    ifstream trace_filename;
    trace_filename.open(argv[1],ios::in);	
    if(!trace_filename.is_open())
    {
	cout<<"Err opening trace"<<endl;
	exit(1);
    }
    while(1)
    { 
	if (trace_filename.eof())
		break;
	trace_filename >> hex >> addr;
	trace_filename>> thread_id;	
	trace_filename>>dec>> time;
	trace_filename>>dec>> cmd;
//	trace_filename >> hex >> addr;
	
	Simulator::StopAt(time+unsink);
    	Simulator::Run();
	Simulator::halted = false;

	Request *req = new Request();
	Request *req2 = new Request(); 	
	req->cmdType = (Command_t)cmd;
	req->address = addr;
	req->arrivalTime = time+unsink;
	req->threadId = thread_id;
	*req2 = *req;
#ifdef DEBUG	
	cout << dec << Simulator::Now() << ": " << hex << req->address << ": I am in main just being loaded from file\n"; 
#endif
	while (1)
	{
	    if (mshr.size() >= MSHR_SIZE)
	    {
		unsink++;
		req->arrivalTime = time+unsink;
	//	cout << Simulator::Now() << ", Unsink = " << unsink << ", " << time+unsink << endl;
		Simulator::StopAt(Simulator::Now()+1);
    	    	Simulator::Run();
	    	Simulator::halted = false;
	    }
	    else if (!mc->reqH->oneBufferFull)
	    {
	//	cout << Simulator::Now() << ", " << mshr.size() << endl;
		if (req->cmdType!=CACHE_WRITEBACK)
			mshr.push_back(*req2);
	    	IrisEvent *e = new IrisEvent();
		e->src = NULL;
		e->dst = (Component*)mc->reqH;
    	    	e->event_data.push_back((void*)req);
    	    	e->type = START;	
    	    	Simulator::Schedule(Simulator::Now()+1, &RequestHandler::process_event, (RequestHandler*)e->dst, e);
	    	break;	
	    }
	    else
	    {
	    	Simulator::StopAt(Simulator::Now()+1);
    	    	Simulator::Run();
	    	Simulator::halted = false;
	    }
	}
	
//	if (time+unsink>100000)
//	    break;
    }
	Simulator::StopAt(time+1000);
    	Simulator::Run();

	cout << endl;
	cout << "Unsink of Thread 0 = " << unsink << ", ";
	cout << endl << endl;

	mc->stats->CalculateAggregateStats();
	mc->stats->PrintAggregateStats();
*/
//    cout << fixed << setprecision(0);
    cout<< "TraceNames: ";
    for (unsigned int i=0; i<NO_OF_THREADS; i++)
	cout << argv[i+1] << ", ";
    cout << endl;

    MC *mc = new MC(); 
    ifstream *trace_filename[NO_OF_THREADS];

    MSHR_H mshrHandler[NO_OF_THREADS];
    Addr_t addr[NO_OF_THREADS];
    UInt thread_id[NO_OF_THREADS];
    Time time[NO_OF_THREADS];
    UInt cmd[NO_OF_THREADS];	

    for (unsigned int i = 0; i<NO_OF_THREADS; i++)
    {
	mc->ni->mshrHandler[i] = &mshrHandler[i];
	mshrHandler[i].id = i; 
	mc->stats->doneOnce[i] = &mshrHandler[i].done; 
	trace_filename[i] = &mshrHandler[i].trace_filename; 
	mshrHandler[i].filename = argv[i+1];
    }

    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    {
//	trace_filename[i] = mshrHandler[i].trace_filename;
	(*trace_filename[i]).open(argv[i+1],ios::in);
    	if(!(*trace_filename[i]).is_open())
    	{
	   cout<<"Err opening trace"<<endl;
	   exit(1);
    	}
    }	
   
    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    {
	if (!(*trace_filename[i]).eof())
	{
	(*trace_filename[i]) >> hex >> addr[i];
	(*trace_filename[i]) >> thread_id[i];	
	(*trace_filename[i]) >> dec >> time[i];
	(*trace_filename[i]) >> dec >> cmd[i];
	}
	
	Request *req2 = new Request();
	req2->cmdType = (Command_t)cmd[i];
	req2->address = addr[i];
	req2->arrivalTime = time[i];
	req2->threadId = i;
	req2->address = mshrHandler[i].GlobalAddrMap(req2->address,i);

	IrisEvent *event = new IrisEvent();
    	event->src = NULL;
    	event->dst = (Component*)(&mshrHandler[i]);
    	event->event_data.push_back(req2);	
#ifdef DEEP_DEBUG
    	cout << dec << Simulator::Now() << ": " << hex << req2->address << ": First Request of each trace to be send to mshrs" << endl;	
#endif
    	Simulator::Schedule(time[i], &MSHR_H::process_event, (MSHR_H*)event->dst, event);   
    }	

    while(1)
    {
	unsigned int index;
	Request * req = new Request();
	Time tempTime;
	if (GetNextRequest(mshrHandler, req, &index))
	{
	    if (req->arrivalTime >= Simulator::Now()+1)
	    	tempTime = req->arrivalTime;
	    else
	    	tempTime = Simulator::Now()+1;
	
	    while (Simulator::Now() < tempTime)
	    {
	    	Simulator::StopAt(Simulator::Now()+1);
    	    	Simulator::Run();
	    	mc->stats->CollectStatsPerCycle();
	    	Simulator::halted = false;
	    }
	
#ifdef DEBUG	
	    cout << dec << Simulator::Now() << ": " << hex << req->address << ": Thread " << req->threadId << ": I am in main just being loaded from file\n"; 
#endif
	    while (1)
	    {
	    	if (!mc->reqH->oneBufferFull)
	    	{
	    	    IrisEvent *e = new IrisEvent();
		    e->src = NULL;
		    e->dst = (Component*)mc->reqH;
    	    	    e->event_data.push_back((void*)req);
    	    	    e->type = START;	
    	    	    Simulator::Schedule(Simulator::Now()+1, &RequestHandler::process_event, (RequestHandler*)e->dst, e);
	    	    break;	
	    	}
	    	else
	    	{
		    Simulator::StopAt(Simulator::Now()+1);
    	    	    Simulator::Run();
		    mc->stats->CollectStatsPerCycle();
	    	    Simulator::halted = false;
	    	}
	    }
	}
	else
	{
	    delete req;
	    Simulator::StopAt(Simulator::Now()+1);
    	    Simulator::Run();
	    mc->stats->CollectStatsPerCycle();
	    Simulator::halted = false;
	}
	
	bool temp = true;
	for (unsigned int i = 0; i<NO_OF_THREADS; i++)
    	{
	    if ((*mc->stats->doneOnce[i])==false)
	    	temp = false;
	}
	if (temp)
	    break;
    }
    for (unsigned int i = 0; i<NO_OF_THREADS; i++)
    {
    	mshrHandler[i].globalUnSink += mshrHandler[i].unsink;
    }	
	    			

/*    Time unsink[NO_OF_THREADS];
    Time globalUnSink[NO_OF_THREADS];	
    vector <Request> mshr[NO_OF_THREADS]; 
    Time lastFinishTime[NO_OF_THREADS];
    for (unsigned int i = 0; i<NO_OF_THREADS; i++)
    {
	time2[i] = -1;
	unsink[i] = 0;  
	globalUnSink[i] = 0; 
	mc->ni->mshr[i] = &mshr[i];
	lastFinishTime[i] = 0; 
    }		

    ifstream trace_filename[NO_OF_THREADS];

    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    {
	 trace_filename[i].open(argv[i+1],ios::in);
    	if(!trace_filename[i].is_open())
    	{
	   cout<<"Err opening trace"<<endl;
	   exit(1);
    	}
    }

    unsigned int index;
    unsigned int count = 0;
    Time prevTime;	

    

    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    {
	if (!trace_filename[i].eof())
	{
	trace_filename[i] >> hex >> addr2[i];
	trace_filename[i] >> thread_id2[i];	
	trace_filename[i] >> dec >> time2[i];
	trace_filename[i] >> dec >> cmd2[i];
	}
	time2[i] = time2[i]+lastFinishTime[i];
    }
    cout << "hello\n";	   
    	
    while(1)
    {
	index = FindMin(time2, unsink);

	addr = GlobalAddrMap(addr2[index],index);
	time = time2[index];
	cmd = cmd2[index];
	thread_id = index;

	Request *req = new Request();
	Request *req2 = new Request();	
	req->cmdType = (Command_t)cmd;
	req->address = addr;
	req->arrivalTime = time+unsink[index];
	req->threadId = thread_id;
	*req2 = *req;
	if (req->cmdType!=CACHE_WRITEBACK)
		    mshr[index].push_back(*req2);

//	cout << time << " " << unsink[0] << " " << unsink[1] << endl;
	if (prevTime >= time+unsink[index])
	    time = prevTime+1-unsink[index];  
//	cout << time << " " << unsink[0] << " " << unsink[1] << endl;
	Time tempTime = 0;
	if (time+unsink[index] >= Simulator::Now()+1)
	    tempTime = time+unsink[index];
	else
	    tempTime = Simulator::Now()+1;
	Simulator::StopAt(tempTime);
    	Simulator::Run();
	Simulator::halted = false;

#ifdef DEBUG	
	cout << dec << Simulator::Now() << ": " << hex << req->address << ": Thread " << req->threadId << ": I am in main just being loaded from file\n"; 
#endif
	while (1)
	{
	    if (mshr[index].size() >= MSHR_SIZE)
	    {
		unsink[index]++;
		req->arrivalTime = time+unsink[index];
	//	cout << dec << Simulator::Now() << ", Unsink = " << unsink[index] << ", " << time+unsink[index] << endl;
		Simulator::StopAt(Simulator::Now()+1);
    	    	Simulator::Run();
	    	Simulator::halted = false;
	    }
	    else if (!mc->reqH->oneBufferFull)
	    {
	//	cout << dec << Simulator::Now() << ", " << mshr[index].size() << ", " << req->arrivalTime << endl;
	    	IrisEvent *e = new IrisEvent();
		e->src = NULL;
		e->dst = (Component*)mc->reqH;
//		req->throttleTime = Simulator::Now() - req->arrivalTime;
//		req->arrivalTime = Simulator::Now();
    	    	e->event_data.push_back((void*)req);
    	    	e->type = START;	
    	    	Simulator::Schedule(Simulator::Now()+1, &RequestHandler::process_event, (RequestHandler*)e->dst, e);
	    	break;	
	    }
	    else
	    {
		Simulator::StopAt(Simulator::Now()+1);
    	    	Simulator::Run();
	    	Simulator::halted = false;
	    }
	}

	if (!trace_filename[index].eof())
	{
	    trace_filename[index] >> hex >> addr2[index];
	    trace_filename[index] >> thread_id2[index];	
	    trace_filename[index] >> dec >> time2[index];
	    trace_filename[index] >> dec >> cmd2[index]; 

	    if (!trace_filename[index].eof())	
	    	time2[index] = time2[index]+lastFinishTime[index];
	    else
	    {
	    	if (lastFinishTime[index] == 0)
	    	    count++;
	    	lastFinishTime[index] = Simulator::Now();
	    	globalUnSink[index] += unsink[index];
	    	unsink[index] = 0;
	    	cout << dec << "lastFinishTime = " << lastFinishTime[index] << ", " << index << ", " << globalUnSink[index] << endl;	

	    	trace_filename[index].close();	
	    	trace_filename[index].open(argv[index+1],ios::in);
    	    	if(!trace_filename[index].is_open())
    	    	{
	    	    cout<<"Err opening trace"<<endl;
	   	    exit(1);
    	    	}	
	   
	    	if (!trace_filename[index].eof())
	    	{ 
            	    trace_filename[index] >> hex >> addr2[index];
	    	    trace_filename[index] >> thread_id2[index];	
	    	    trace_filename[index] >> dec >> time2[index];
	    	    trace_filename[index] >> dec >> cmd2[index];
		
		    if (!trace_filename[index].eof())
		    	time2[index] = time2[index]+lastFinishTime[index];	
	    	}
		//    cout << time2[0] << " " << time2[1] << " " << unsink[0] << " " << unsink[1] << endl;
	    }		
	}
	
	
	prevTime = time+unsink[index];
	

	if (count >= NO_OF_THREADS)
	    break;

	if (time>1000)
	    break;
    }

*/    
	for (unsigned int x = 1; x<=2000; x++)
	{
	    Simulator::StopAt(Simulator::Now()+1);
    	    Simulator::Run();
	    mc->stats->CollectStatsPerCycle();
	    Simulator::halted = false;
	}

	cout << endl;
	for (unsigned int i=0; i<NO_OF_THREADS; i++)
	{
	    cout << dec << "Unsink of Thread " << i << " = " << mshrHandler[i].globalUnSink << ", ";
	}
	cout << endl << endl;

	mc->stats->CalculateAggregateStats();
	mc->stats->PrintAggregateStats();

    delete mc;	
    return 0;
}
