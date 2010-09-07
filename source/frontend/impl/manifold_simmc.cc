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
#include        <string.h>
#include        <sstream>
#include        <iostream>
#include 	<iomanip>
#include        <cstdlib>
#include 	<fstream>
#include        <map>
#include        "../../kernel/component.h"
#include        "../../kernel/simulator.h"
#include        "../../util/config_params.h"
#include        "../../util/mc_constants.h"
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

using namespace std;

//uint MC_ADDR_BITS = 14;
//uint THREAD_BITS_POSITION = 25;
vector<MSHR_SA_H*> mshrHandler;

void iris_process_options(int argc,char *argv[]);
void dump_configuration ( void );
bool GetNextRequest(Request *req, unsigned int *index);

int main(int argc, char **argv)
{
    iris_process_options(argc,argv);
    dump_configuration(); 	
    init_dram_timing_parameters();

    //    cout << fixed << setprecision(0);
    cout<< "TraceNames: ";
    for (unsigned int i=0; i<NO_OF_THREADS; i++)
        cout << traces[i] << ", ";
    cout << endl;

    MC *mc = new MC(); 
    mc->Init();	
    ifstream *trace_filename[NO_OF_THREADS];

      
    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    {
        MSHR_SA_H *tempmshrH = new MSHR_SA_H(); 
        mshrHandler.push_back(tempmshrH);
    }

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
        mc->stats->doneOnce[i] = &mshrHandler[i]->done; 
        trace_filename[i] = &mshrHandler[i]->trace_filename; 
        mshrHandler[i]->filename = const_cast<char*>(trace_name.c_str());
    }

    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    {
        //	trace_filename[i] = mshrHandler[i].trace_filename;
        (*trace_filename[i]).open(traces[i].c_str(),ios::in);
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
        unsigned int rr = (i+lastThreadIndex+1)%NO_OF_THREADS;
        if (!mshrHandler[rr]->mshr.empty())
        {	
            if ( mshrHandler[rr]->lastScheduledIndex < mshrHandler[rr]->mshr.size() )
            {
                time[rr] = mshrHandler[rr]->mshr[mshrHandler[rr]->lastScheduledIndex].arrivalTime;
                temp = true;
                mshrFilled[rr] = false;
                break;
            }				
        }
        if (!mshrHandler[rr]->writeQueue.empty())
        {	
            time[rr] = mshrHandler[rr]->writeQueue[0].arrivalTime;
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

void
dump_configuration ( void )
{
    /* Creating mesh topology with the following config */
    cerr << " no_of_mcs:\t" << no_mcs << endl;
    cerr << " no_of_traces:\t" << traces.size() << endl;
    cerr << " max_sim_time:\t" << max_sim_time << endl;
    cerr << " THREAD_BITS_POSITION:\t" << THREAD_BITS_POSITION<< endl;
    cerr << " MC_ADDR_BITS:\t" << MC_ADDR_BITS<< endl;
    cerr << " NETWORK_BITS:\t" << NETWORK_ADDRESS_BITS << endl;
    cerr << " COREID_BITS:\t" << NETWORK_THREADID_BITS << endl;
    cerr << " COMMAND_BITS:\t" << NETWORK_COMMAND_BITS << endl;
    cerr << " NO_OF_THREADS:\t" << NO_OF_THREADS << endl;
    cerr << " NO_OF_CHANNELS:\t" << NO_OF_RANKS << endl;
    cerr << " NO_OF_RANKS:\t" << NO_OF_RANKS << endl;
    cerr << " NO_OF_BANKS:\t" << NO_OF_BANKS << endl;
    cerr << " NO_OF_ROWS:\t" << NO_OF_ROWS << endl;
    cerr << " NO_OF_COLUMNS:\t" << NO_OF_COLUMNS << endl;
    cerr << " COLUMN_SIZE:\t" << COLUMN_SIZE << endl;
    cerr << " Msg_class with arbitration priority:\t" << msg_type_string << endl;

    if( traces.size() < (no_nodes - no_mcs) )
    {
        cout << " Not enough trace files for simulation " << endl;
        exit(1);
    }
    return ;
}

void iris_process_options(int argc,char *argv[])
{
/* The following parameters must be specified in the config with the
     * correct knobs */
    unsigned int offset;    
    ifstream fd(argv[1]);
    string data, word;
    while(!fd.eof())
    {
        getline(fd,data);
        string::size_type position = -1;
        position = data.find("#");
        istringstream iss( data, istringstream::in);
        while ( position > data.size() && iss >> word )
        {
            if ( word.compare("MCS") == 0)
                iss >> no_mcs;
            if ( word.compare("MAX_SIM_TIME") == 0)
                iss >> max_sim_time;
            if ( word.compare("THREAD_BITS_POSITION") == 0)
                iss >> THREAD_BITS_POSITION;
            if ( word.compare("MC_ADDR_BITS") == 0)
                iss >> MC_ADDR_BITS;
            if ( word.compare("BANK_BITS") == 0)
                iss >> BANK_BITS;

	    if ( word.compare("NO_OF_NODES") == 0)
                iss >> no_nodes;
	    NO_OF_THREADS = no_nodes;	
            if ( word.compare("NO_OF_CHANNELS") == 0)
                iss >> NO_OF_CHANNELS;     
            if ( word.compare("NO_OF_RANKS") == 0)
                iss >> NO_OF_RANKS;
            if ( word.compare("NO_OF_BANKS") == 0)
                iss >> NO_OF_BANKS;                
            if ( word.compare("NO_OF_ROWS") == 0)
                iss >> NO_OF_ROWS;
            if ( word.compare("NO_OF_COLUMNS") == 0)
                iss >> NO_OF_COLUMNS;
            if ( word.compare("COLUMN_SIZE") == 0)
                iss >> COLUMN_SIZE;
            if ( word.compare("MSHR_SIZE") == 0)
                iss >> MSHR_SIZE;
                
            if ( word.compare("MAX_BUFFER_SIZE") == 0)
                iss >> MAX_BUFFER_SIZE;
            if ( word.compare("MAX_CMD_BUFFER_SIZE") == 0)
                iss >> MAX_CMD_BUFFER_SIZE;
            if ( word.compare("RESPONSE_BUFFER_SIZE") == 0)
                iss >> RESPONSE_BUFFER_SIZE;
             
            if ( word.compare("NETWORK_ADDRESS_BITS") == 0)
                iss >> NETWORK_ADDRESS_BITS;
            if ( word.compare("NETWORK_THREADID_BITS") == 0)
                iss >> NETWORK_THREADID_BITS ;
            if ( word.compare("NETWORK_COMMAND_BITS") == 0)
                iss >> NETWORK_COMMAND_BITS;

            if ( word.compare("DRAM_PAGE_POLICY") == 0)
                iss >> dram_page_policy_string;
            if ( word.compare("ADDRESS_MAP_SCHEME") == 0)
                iss >> addr_map_scheme_string;
            if ( word.compare("MC_SCHEDULING_ALGORITHM") == 0)
                iss >> mc_scheduling_algorithm_string;

            if ( word.compare("TRACE") == 0)
            {
                iss >> trace_name;
                traces.push_back(trace_name);
            }
        }
    }

    for ( uint i=0; i<argc; i++)
    {
        if( strcmp(argv[i],"--thread_id_bits")==0)
            THREAD_BITS_POSITION = atoi(argv[i+1]);
        if( strcmp(argv[i],"--mc_bits")==0)
            MC_ADDR_BITS = atoi(argv[i+1]);
        if( strcmp(argv[i],"--bank_bits")==0)
            BANK_BITS = atoi(argv[i+1]);
    }
 
    if( dram_page_policy_string.compare("OPEN_PAGE_POLICY") == 0)
        dram_page_policy = OPEN_PAGE_POLICY;
    if( dram_page_policy_string.compare("CLOSE_PAGE_POLICY") == 0)
        dram_page_policy = CLOSE_PAGE_POLICY;
    
    if( mc_scheduling_algorithm_string.compare("FR_FCFS") == 0)
        mc_scheduling_algorithm = FR_FCFS;
    if( mc_scheduling_algorithm_string.compare("FC_FS") == 0)
        mc_scheduling_algorithm = FC_FS;
    if( mc_scheduling_algorithm_string.compare("PAR_BS") == 0)
        mc_scheduling_algorithm = PAR_BS;
    
    if( addr_map_scheme_string.compare("PAGE_INTERLEAVING") == 0)
        addr_map_scheme = PAGE_INTERLEAVING;
    if( addr_map_scheme_string.compare("PERMUTATION") == 0)
        addr_map_scheme = PERMUTATION;
    if( addr_map_scheme_string.compare("CACHELINE_INTERLEAVING") == 0)
        addr_map_scheme = CACHELINE_INTERLEAVING;
    if( addr_map_scheme_string.compare("GENERIC") == 0)
        addr_map_scheme = GENERIC;
    if( addr_map_scheme_string.compare("NO_SCHEME") == 0)
        addr_map_scheme = NO_SCHEME;
    if( addr_map_scheme_string.compare("LOCAL_ADDR_MAP") == 0)
        addr_map_scheme = LOCAL_ADDR_MAP;

    return;
}

