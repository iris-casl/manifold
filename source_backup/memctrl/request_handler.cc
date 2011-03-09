/*
 * =====================================================================================
 *
 *       Filename:  request_handler.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/17/2010 01:32:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include <math.h>
#include "request_handler.h"
#include "bus.h"
#include "MC.h"


using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  RequestHandler
 *      Method:  process_event
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

RequestHandler::RequestHandler()
{
    reqTag = 0;	
    pipelineFilled = false;	
    oneBufferFull = false;	
    addrMap = new AddrMap();
    addrMap->parent = this;
    busHandler = new BusHandler();
    busHandler->parent = this;
    chan = new ChannelHandler[NO_OF_CHANNELS];
    for(short i=0; i<NO_OF_CHANNELS; i++)
    {
	chan[i].chanId = i;
	for(short j=0; j<NO_OF_RANKS; j++)
	{
	    chan[i].rank[j].rankId = j;
	    for(short k=0; k<NO_OF_BANKS; k++)
            {	
		chan[i].rank[j].bank[k].myChannel = i;
		chan[i].rank[j].bank[k].bankId = k;
                chan[i].rank[j].bank[k].parent = this;
                chan[i].rank[j].bank[k].bufferId = (ullint)floor(k*NO_OF_BUFFERS/NO_OF_BANKS); 	// TODO need to set based on number of buffers
                chan[i].rank[j].bank[k].myRank = (void*)(&chan[i].rank[j]);
            }	
	}
    }
    lastBatchFormTime = 0;	
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  RequestHandler
 *      Method:  ~RequestHandler
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
RequestHandler::~RequestHandler()
{
    delete addrMap;
//    delete busHandler;	
}

void RequestHandler::SetLinks()
{
    for(short i=0; i<NO_OF_CHANNELS; i++)
    {
    	busHandler->cmdIssuer[i].dataBus = &((Bus*)child)->dataBus[i];
    	busHandler->cmdIssuer[i].cmdBus = &((Bus*)child)->cmdBus[i];	
    }
}

void RequestHandler::process_event(IrisEvent* e)
{
    Request *req = new Request();	
    IrisEvent *event = new IrisEvent();	
    int temp = lastBatchFormTime+BATCH_FORM_TIME;
    switch (e->type)
    {
	case START:   
		if ( temp > Simulator::Now() )
		    FormBatch();
                *req = *((Request*)e->event_data.at(0));
  		PushPipeline(req);	
#ifdef DEEP_DEBUG
		cout << dec << Simulator::Now() << ": 0x" << hex << req->address << ": Now I am in start of request handler\n"; 
#endif
		if (!oneBufferFull)	
                {
		    event->src = (Component*)this;
                    event->dst = (Component*)addrMap;	
                    event->type = START;
                    Simulator::Schedule(Simulator::Now()+1, &AddrMap::process_event, (AddrMap*)event->dst, event);
		}
		else 
		{
		    pipelineFilled = true;	
		    delete event;
		}
	    delete req;	                
       	    break;		

    case CONTINUE: 
            {
		if (temp > Simulator::Now())
		    FormBatch();
		oneBufferFull = false;
/*                if( !((MC*)mc)->ni->ni_recv)
                {
                    ((MC*)mc)->ni->ni_recv = true;
                    IrisEvent* event2 = new IrisEvent();
                    event2->src = (Component*)this;
                    event2->dst = (Component*)(((MC*)mc)->ni);	
                    event2->type = NEW_PACKET_EVENT;
                    Simulator::Schedule(Simulator::Now()+1, &NI::process_event, (NI*)event2->dst, event2);
                }
                else
                {
                    IrisEvent* event2 = new IrisEvent();
                    event2->src = (Component*)this;
                    event2->dst = (Component*)this;	
                    event2->type = CONTINUE;
                    Simulator::Schedule(Simulator::Now()+1, &RequestHandler::process_event, this, event2);

                }

*/
                if (pipelineFilled)	
                {
#ifdef DEEP_DEBUG
		    cout << dec << Simulator::Now() << ": 0x" << hex << pipeline.address << ": NI Now I am in continue of request handler\n"; 
#endif
		    pipelineFilled = false;
		    event->src = (Component*)this;
                    event->dst = (Component*)addrMap;	
                    event->type = START;
                    Simulator::Schedule(Simulator::Now()+1, &AddrMap::process_event, (AddrMap*)event->dst, event);
		}
		else
		   delete event;
	    delete req;	
            }
	    break;

        default:
                 //cerr <<  "RequestHandler There should be no default case as such/n";
            break;

    }
    if (!e->event_data.empty())
        delete (Request*)e->event_data.at(0);		
    delete e;
}

void RequestHandler::PushPipeline(Request *req)
{
    pipeline = *req;
}

std::string RequestHandler::toString()
{
	return "RequestHandler";
}

void RequestHandler::FormBatch() 
{
#ifdef PARBS
//	MarkAll();
//#else
	MarkBatchOnly();
#endif
   
    lastBatchFormTime += BATCH_FORM_TIME;	
}

void RequestHandler::MarkAll() 
{
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)	
	    	for (unsigned int l=0; l<chan[i].rank[j].rbuffer[k].size(); l++)
		    	chan[i].rank[j].rbuffer[k][l].mark = true;
}

void RequestHandler::MarkBatchOnly()
{
    unsigned int count[NO_OF_THREADS];	
    for (unsigned int i=0; i<NO_OF_THREADS; i++)
    	count[i]=0;
    	
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
	for (unsigned int j=0; j<NO_OF_RANKS; j++)
	    for (unsigned int k=0; k<NO_OF_BUFFERS; k++)
		if (chan[i].rank[j].rbuffer[k].size() > MAX_BATCH_SIZE)
		    for (unsigned int l=MAX_BATCH_SIZE; l<chan[i].rank[j].rbuffer[k].size(); l++)
			chan[i].rank[j].rbuffer[k][l].mark = false;	
}
