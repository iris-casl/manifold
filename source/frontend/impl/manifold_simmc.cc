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
#include        "../../kernel/component.h"
#include        "../../kernel/simulator.h"
#include        "../../util/config_params.h"
#include        "../../simIris/data_types/impl/irisEvent.h"
#include        "../../memctrl/request.h"
#include	"../../memctrl/request_handler.h" 
#include        "../../memctrl/bus_handler.h"
#include        "../../memctrl/bus.h"
#include        "../../memctrl/dram.h"
#include	"../../memctrl/refresh_manager.h"
#include	"../../memctrl/response_handler.h"
#include	"../../memctrl/MC.h"
#include	"../../memctrl/mshr_standalone.h"
#include        "../../zesto/zesto-cache.h"

using namespace std;

//uint MC_ADDR_BITS = 14;
//uint THREAD_BITS_POSITION = 25;
vector<MSHR_SA_H*> mshrHandler;
bool GetNextRequest(Request *req, unsigned int *index)
{
    bool temp = false;	

    //  vector<MSHR_SA_H> mshrHandler;
    //   mshrHandler = *mshrH;
    //   mshrHandler.resize(NO_OF_THREADS);
    static int lastThreadIndex = NO_OF_THREADS -1;
    vector<Time> time;
    time.resize(NO_OF_THREADS);	
    vector<bool> mshrFilled;
    mshrFilled.resize(NO_OF_THREADS);
    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    {
        mshrFilled[i] = true;
        time[i] = -1;
    }    
    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    {
        if (!mshrHandler[i]->mshr.empty())
        {	
            if ( mshrHandler[i]->lastScheduledIndex < mshrHandler[i]->mshr.size() )
            {
                time[i] = mshrHandler[i]->mshr[mshrHandler[i]->lastScheduledIndex].arrivalTime;
                temp = true;
                mshrFilled[i] = false;
                break;
            }				
        }
        if (!mshrHandler[i]->writeQueue.empty())
        {	
            time[i] = mshrHandler[i]->writeQueue[0].arrivalTime;
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
        if (!mshrHandler[rr]->writeQueue.empty())
        {	
            vector<Request>::iterator bufferIndex = mshrHandler[rr]->writeQueue.begin();	
            *req = mshrHandler[rr]->writeQueue[0];
            mshrHandler[rr]->writeQueue.erase(bufferIndex);
            lastThreadIndex = rr;	
            return true;
        }
        else if (time[rr] < minTime && !mshrFilled[rr])
        { 	  
            minTime = time[rr];
            *index = rr;
            lastThreadIndex = *index;			
            *req = mshrHandler[rr]->mshr[mshrHandler[rr]->lastScheduledIndex];
            temp = true;
        }
    }
    mshrHandler[*index]->lastScheduledIndex++;    			

    return temp;		  	
}

void MC::sim_main(IrisEvent* e)
{
    Request *req;
    unsigned int evType;
    switch (e->type)
    {
        case CONTINUE:
        {
            if (!reqH->oneBufferFull)
            {
                unsigned int index;
                req = new Request();
                if (GetNextRequest(req, &index))
                {
                    IrisEvent *e = new IrisEvent();
                    e->src = NULL;
                    e->dst = (Component*)reqH;
                    e->event_data.push_back((void*)req);
                    e->type = START;	
                    Simulator::Schedule(Simulator::Now()+1, &RequestHandler::process_event, (RequestHandler*)e->dst, e);
                    evType = CONTINUE;
                    break;	
                }
                else
                {
                    evType = CONTINUE;
                    delete req;
                }
            }
            else
            {
                evType = CONTINUE;
            }
            break;
        }
    }

    IrisEvent *event2 = new IrisEvent();
    event2->type = evType;
    Simulator::Schedule(Simulator::Now()+1, &MC::sim_main, this, event2);

    delete e;
    return;
}

int main(int argc, char **argv)
{
    init_dram_timing_parameters();
    //    cout << fixed << setprecision(0);
    cout<< "TraceNames: ";
    for (unsigned int i=0; i<NO_OF_THREADS; i++)
        cout << argv[i+1] << ", ";
    cout << endl;

    MC *mc = new MC(); 
    mc->Init();	
    ifstream *trace_filename[NO_OF_THREADS];

      
    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    {
        MSHR_SA_H *tempmshrH = new MSHR_SA_H(); 
        mshrHandler.push_back(tempmshrH);
    }
  
//    mshrHandler.resize(NO_OF_THREADS);
    vector<Addr_t> addr;
    addr.resize(NO_OF_THREADS);
    vector<uint> thread_id;
    thread_id.resize(NO_OF_THREADS);
    vector<Time> time;
    time.resize(NO_OF_THREADS);
    vector<uint> cmd;
    cmd.resize(NO_OF_THREADS);	

    for (unsigned int i = 0; i<NO_OF_THREADS; i++)
    {
        mshrHandler[i]->id = i; 
        //        mc->stats->doneOnce[i] = &mshrHandler[i]->done; 
        trace_filename[i] = &mshrHandler[i]->trace_filename; 
        mshrHandler[i]->filename = argv[i+1];
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
        req2->cmdType = (cache_command)cmd[i];
        req2->address = addr[i];
        req2->arrivalTime = time[i];
        req2->threadId = i;
        req2->address = mshrHandler[i]->GlobalAddrMap(req2->address,i);

        IrisEvent *event = new IrisEvent();
        event->src = NULL;
        event->dst = (Component*)(mshrHandler[i]);
        event->event_data.push_back(req2);	
#ifdef DEEP_DEBUG
        cout << dec << Simulator::Now() << ": " << hex << req2->address << ": First Request of each trace to be send to mshrs" << endl;	
#endif
        Simulator::Schedule(time[i], &MSHR_SA_H::process_event, (MSHR_SA_H*)event->dst, event);   
    }	

    IrisEvent *event2 = new IrisEvent();
    event2->type = CONTINUE;
    Simulator::Schedule(Simulator::Now()+1, &MC::sim_main, mc, event2);

    for (unsigned int i = 0; i<NO_OF_THREADS; i++)
    {
        mshrHandler[i]->globalUnSink += mshrHandler[i]->unsink;
    }	


    Simulator::StopAt(max_sim_time);
    Simulator::Run();

    mc->stats->CalculateAggregateStats();
    cerr << mc->stats->PrintAggregateStats(0);

    delete mc;	
    return 0;
}

