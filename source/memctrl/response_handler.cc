/*
 * =====================================================================================
 *
 *       Filename:  response_handler.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/04/2010 02:06:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include <math.h>
#include "response_handler.h"
#include "request_handler.h"
#include "MC.h"
using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ResponseHandler
 *      Method:  ResponseHandler
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

ResponseHandler::ResponseHandler()
{
    serviced.resize(RESPONSE_BUFFER_SIZE);
    bufferFull = false;
    stoppedQueue = false;	
    responseBuffer.clear();
    for (unsigned int i=0; i<RESPONSE_BUFFER_SIZE; i++)
    {
        serviced[i] = false;
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ResponseHandler
 *      Method:  ~ResponseHandler
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
ResponseHandler::~ResponseHandler()
{

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ResponseHandler
 *      Method:  process_event
 * Description:  bool ResponseHandler::IsBufferFull()
 *--------------------------------------------------------------------------------------
 */
void ResponseHandler::process_event(IrisEvent* e)
{
    Request *req = new Request();	
    DRAMCmdState cmd;	
    int index;	
    vector<Request>::iterator bufferIndex = responseBuffer.begin();	
    switch (e->type)
    {	
	case PUSH_BUFFER:
	     *req = *((Request*)e->event_data.at(0));
	     responseBuffer.push_back(*req);
#ifdef DEEP_DEBUG
	    cout << dec << Simulator::Now() << ": 0x" << hex << req->address << ": I am in ResponseHandler PUSH BUFFER with " << dec << req->cmdType << " and tag " << req->tag << endl;
#endif
	    delete (Request*)e->event_data.at(0);	
	break;
	case REPLY:
        {   cmd = *((DRAMCmdState*)e->event_data.at(0));
	     index = SearchBuffer(&cmd);		// Search the req, returns the index
		responseBuffer[index].busInsertionTime = cmd.req.busInsertionTime;	
		*req = responseBuffer[index];
                responseBuffer[index].serviced = true;
		responseBuffer[index].retireTime = (ullint)Simulator::Now();
		delete (DRAMCmdState*)e->event_data.at(0);
#ifdef DEEP_DEBUG
	      cout << dec << Simulator::Now() << ": 0x" << hex << cmd.req.address << ": I am in Response Handler REPLY with tag " << dec << cmd.req.tag << endl;
#endif
#if MEM_STANDALONE

                index = SendServiced();
                *req = responseBuffer[index];
		responseBuffer.erase(index+bufferIndex);		// delete the req from response buffer
		IrisEvent* ev = new IrisEvent();
        	ev->type = MSHR_DELETE;
		ev->dst = (Component*)mshrHandler[req->threadId];
                Request *rr = new Request();
                *rr = *req;
                ev->event_data.push_back(rr);
        	Simulator::Schedule(Simulator::Now()+1, &MSHR_SA_H::process_event, (MSHR_SA_H*)ev->dst, ev);
#ifdef DEBUG	    
cout << endl << dec << Simulator::Now() << ": 0x" << hex << req->address << ": Total Time taken by request " << dec << req->retireTime - req->arrivalTime << endl;
#endif	 
		((MC*)mc)->stats->CollectStatsPerRequest(req);
            
#else
	     if (((NI*)((MC*)mc)->parent)->niQueue.empty() ) //&& !((NI*)((MC*)mc)->parent)->sending)
	     {
                index = SendServiced();
                 *req = responseBuffer[index];
		((NI*)((MC*)mc)->parent)->niQueue.push_back(*req);	// put the request into NI queue if its free
		responseBuffer.erase(index+bufferIndex);		// delete the req from response buffer
		IrisEvent* ev = new IrisEvent();
        	ev->type = OUT_PULL_EVENT;
		ev->dst = ((MC*)mc)->parent;
        	Simulator::Schedule(Simulator::Now()+1, &NI::process_event, (NI*)ev->dst, ev);
#ifdef DEBUG	    
cout << endl << dec << Simulator::Now() << ": 0x" << hex << req->address << ": Total Time taken by request " << dec << req->retireTime - req->arrivalTime << endl;
#endif	 
		((MC*)mc)->stats->CollectStatsPerRequest(req);
            
 	     }
             else
             {
                responseBuffer[index].serviced = true;
             }

#endif 
         
	     break;	
        }
        case SEND_TO_NI:     
                  index = SendServiced();	
		*req = responseBuffer[index];
                
             if(index != -1 && ((NI*)((MC*)mc)->parent)->niQueue.empty() )
             {
		    ((NI*)((MC*)mc)->parent)->niQueue.push_back(*req);	// put the request into NI queue if its free
                     assert(responseBuffer[index].serviced == true);                    
		    responseBuffer.erase(index+bufferIndex);		// delete the req from response buffer
		    IrisEvent* ev = new IrisEvent();
        	    ev->type = OUT_PULL_EVENT;
		    ev->dst = ((MC*)mc)->parent;
        	    Simulator::Schedule(Simulator::Now()+1, &NI::process_event, (NI*)ev->dst, ev);
                    serviced[index] = false;   
#ifdef DEBUG	    
	cout << endl << dec << Simulator::Now() << ": " << hex << req->address << ": Total Time taken by request " << dec << req->retireTime - req->arrivalTime << endl;
#endif	 
		    ((MC*)mc)->stats->CollectStatsPerRequest(req);
             }

             break;
              
	default:
		cout << dec << Simulator::Now() << ": I should not come to default of process event in response handler\n";
    }		
    delete req;	
    delete e;
}

unsigned int ResponseHandler::SendServiced()
{		
    for (unsigned int i=0; i<responseBuffer.size(); i++)
    {
		if (responseBuffer[i].serviced)
		{	
	    	return i;
		}
    }	    
    return -1;
}
///////// Search the response buffer. Find the request, erase it from the buffer and return it ////////

unsigned int ResponseHandler::SearchBuffer(DRAMCmdState *cmd)
{		
    for (unsigned int i=0; i<responseBuffer.size(); i++)
    {
	if (responseBuffer[i].tag == cmd->req.tag)
	{	
	    return i;
	}
    }	    
    cout << "Error: scheduled cmd could not be found in Response Buffer\n";
    return -1;	
}

bool ResponseHandler::IsBufferFull()
{
    if ( responseBuffer.size() > RESPONSE_BUFFER_SIZE-3 )
    {
	bufferFull = true;	
        return true;
    }
    else
    { 
	bufferFull = false;
        return false;
    }
}

bool ResponseHandler::CanStart()
{
    if ( responseBuffer.size() <= 0.75*(RESPONSE_BUFFER_SIZE) )
        return true;
    else 
        return false;
}

std::string ResponseHandler::toString()
{
//	cout << bufferId <<endl;
	return "ResponseHandler";
}
