/*
 * =====================================================================================
 *
 *       Filename:  bank_handler.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 06:49:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include <math.h>
#include "bank_handler.h"
#include "rank_handler.h"
#include "request_handler.h"
#include "cmd_issuer.h"
#include "response_handler.h"
#include <time.h>

using namespace std;

BankHandler::BankHandler()
{	
    rowBufferIndex = 0;
    rowOpen = false;
    bufferFull = false;	
    bypassReq = 0;
    prevBypassReq = 0;	
    prevBufferIndex = 0;
    prevRowOpen = false;
    generated = false;
    bufferOccupancy = 0;
    bufferSize = 0;
    bufferFullCounter = 0;
 //   bankTag = 0; 	
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  BankHandler
 *      Method:  ~BankHandler
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
BankHandler::~BankHandler()
{

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  BankHandler
 *      Method:  process_event
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
void BankHandler::process_event(IrisEvent* e)
{  
    switch (e->type)
    {
	case START:		
            if (HasWork())
            {
		Request *req = new Request();	
		int index=0;
                bufferOccupancy++;
                bufferSize += ((RankHandler*)myRank)->rbuffer[bankId].size();
		if (IsBufferFull())
		    bufferFullCounter++;

		if ( !((RequestHandler*)parent)->busHandler->IsFull(myChannel) &&
		       (((RequestHandler*)parent)->busHandler->linkBusy[myChannel] == false) &&
		     !((ResponseHandler*)((RequestHandler*)parent)->resPtr)->IsBufferFull() 
		     &&  ((RequestHandler*)parent)->busHandler->BankRankFree(bankId,((RankHandler*)myRank)->rankId,myChannel)
		     )
		{		
                    if(MainScheduler(req,&index))  
                    {
		    	vector<Request>::iterator bufferIndex = ((RankHandler*)myRank)->rbuffer[bankId].begin() + index;
			req->tag = ((RequestHandler*)parent)->reqTag;
			req->cbufferInsertionTime = (ullint)Simulator::Now()+1;
		    	((RequestHandler*)parent)->busHandler->LowLevelCmdGen(req);
			((RequestHandler*)parent)->busHandler->linkBusy[req->channelNo] = true;		
		        ((RequestHandler*)parent)->busHandler->oneReq[myChannel][((RankHandler*)myRank)->rankId][bankId] = true;
		    	((RankHandler*)myRank)->rbuffer[bankId].erase(bufferIndex);
#ifdef DEEP_DEBUG
cout << dec << Simulator::Now() << ": 0x" << hex << req->address << ": Now I am in start with bank " << dec << req->bankNo << ", row " << req->rowNo <<" of BankHandler " << bankId << "\n";    	
#endif		    	
                if(!((RequestHandler*)parent)->busHandler->cmdIssuer[req->channelNo].generated_start_event)
                {
                        ((RequestHandler*)parent)->busHandler->cmdIssuer[req->channelNo].generated_start_event = true;
			IrisEvent *event = new IrisEvent();
		    	event->src = (Component*)this;
                    	event->dst = (Component*)(&((RequestHandler*)parent)->busHandler->cmdIssuer[req->channelNo]);	
                    	event->type = START;
                    	Simulator::Schedule(Simulator::Now()+1, &CmdIssuer::process_event, (CmdIssuer*)event->dst, event);
                }	
			if (req->cmdType == CACHE_READ || req->cmdType == CACHE_PREFETCH || req->cmdType == CACHE_WRITE)
			{
			    IrisEvent *event10 = new IrisEvent();			
			    event10->src = (Component*)this;
                    	    event10->dst = (Component*)((RequestHandler*)parent)->resPtr;
#ifdef DEEP_DEBUG
			    cout << Simulator::Now() << ": 0x" << hex << req->address << ": I will be sending request of bank " << dec << req->bankNo << ", row " << req->rowNo <<" to ResponseHandler\n"; 
#endif		
                    	    event10->event_data.push_back((void*)req);
                    	    event10->type = PUSH_BUFFER;
			    Simulator::Schedule(Simulator::Now()+1, &ResponseHandler::process_event, (ResponseHandler*)event10->dst, event10);
	 		    ((RequestHandler*)parent)->reqTag = (((RequestHandler*)parent)->reqTag+1) % (RESPONSE_BUFFER_SIZE*2);	
			}
			else 
			    delete req;
		    }
		    else
		    {
		//	RestorePrevState();	
			delete req;
		    }	
		}
		else
		    delete req;	
		if (!generated)
		{
		generated = true;
		IrisEvent *event2 = new IrisEvent();
		event2->src = (Component*)this;
                event2->dst = (Component*)this;	
                event2->type = CONTINUE;
                Simulator::Schedule(Simulator::Now()+1, &BankHandler::process_event, (BankHandler*)event2->dst, event2);
		}
            }
            break;		

        case CONTINUE: 
	    
		generated = false; 
	    if (bufferFull && !IsBufferFull())
	    {
		bufferFull = false;
		IrisEvent *event = new IrisEvent();
		event->src = (Component*)this;
                event->dst = (Component*)parent;
                event->type = CONTINUE;
                Simulator::Schedule(Simulator::Now()+1, &RequestHandler::process_event, (RequestHandler*)event->dst, event);
	    } 

	    if (IsBufferFull())
	        bufferFullCounter++;

            if (HasWork())
            {
		Request *req = new Request();
		int index=0;
                bufferOccupancy++;
                bufferSize += ((RankHandler*)myRank)->rbuffer[bankId].size();

		if ( !((RequestHandler*)parent)->busHandler->IsFull(myChannel) &&
		     (((RequestHandler*)parent)->busHandler->linkBusy[myChannel] == false) &&
		     !((ResponseHandler*)((RequestHandler*)parent)->resPtr)->IsBufferFull() 
	&&	      ((RequestHandler*)parent)->busHandler->BankRankFree(bankId,((RankHandler*)myRank)->rankId,myChannel) 
                   )
                {	
                    if(MainScheduler(req,&index)) 
                    {
                        vector<Request>::iterator bufferIndex = ((RankHandler*)myRank)->rbuffer[bankId].begin() + index;
                        req->tag = ((RequestHandler*)parent)->reqTag;
                        req->cbufferInsertionTime = (ullint)Simulator::Now()+1;
                        ((RequestHandler*)parent)->busHandler->LowLevelCmdGen(req);
                        ((RequestHandler*)parent)->busHandler->linkBusy[req->channelNo] = true;				
                        ((RequestHandler*)parent)->busHandler->oneReq[myChannel][((RankHandler*)myRank)->rankId][bankId] = true;
                        ((RankHandler*)myRank)->rbuffer[bankId].erase(bufferIndex);		
#ifdef DEEP_DEBUG
                        cout << dec << Simulator::Now() << ": 0x" << hex << req->address << ": Now I am in continue with bank " << dec << req->bankNo << ", row " << req->rowNo <<" of BankHandler " << bankId << "\n";      	
#endif		    	
                        if(!((RequestHandler*)parent)->busHandler->cmdIssuer[req->channelNo].generated_start_event)
                        {
                            ((RequestHandler*)parent)->busHandler->cmdIssuer[req->channelNo].generated_start_event = true;
                            IrisEvent *event = new IrisEvent();
                            event->src = (Component*)this;
                            event->dst = (Component*)(&((RequestHandler*)parent)->busHandler->cmdIssuer[req->channelNo]);	
                            event->type = START;
                            Simulator::Schedule(Simulator::Now()+1, &CmdIssuer::process_event, (CmdIssuer*)event->dst, event);
                        }	

                        if (req->cmdType == CACHE_READ || req->cmdType == CACHE_PREFETCH || req->cmdType == CACHE_WRITE)
                        {
                            IrisEvent *event10 = new IrisEvent();			
                            event10->src = (Component*)this;
                            event10->dst = (Component*)((RequestHandler*)parent)->resPtr;
#ifdef DEEP_DEBUG
                            cout << Simulator::Now() << ": " << hex << req->address << ": I will be sending request of bank " << dec << req->bankNo << ", row " << req->rowNo <<" to ResponseHandler\n"; 	
#endif	
                            event10->event_data.push_back((void*)req);
                            event10->type = PUSH_BUFFER;
                            Simulator::Schedule(Simulator::Now()+1, &ResponseHandler::process_event, (ResponseHandler*)event10->dst, event10);
                            ((RequestHandler*)parent)->reqTag = (((RequestHandler*)parent)->reqTag+1) % (RESPONSE_BUFFER_SIZE*2);	
                        }
                        else 
                            delete req;
                    }
                 //   else
                    {	
                        //	RestorePrevState(); 
               //         delete req;
                    }		
                }	
                else
                    delete req;	

                if (!generated)
                {
                    generated = true;
                    IrisEvent *event2 = new IrisEvent();
                    event2->src = (Component*)this;
                    event2->dst = (Component*)this;	
                    event2->type = CONTINUE;
                    Simulator::Schedule(Simulator::Now()+1, &BankHandler::process_event, (BankHandler*)event2->dst, event2);
                }
            }   
            break;

        default:
            //cerr <<  "BankHandler There should be no default case as such but OK /n";
            break;

    }
    delete e;
}

bool BankHandler::HasWork()
{
    if (((RankHandler*)myRank)->rbuffer[bankId].empty())
        return false;
    else 
        return true;
}

bool BankHandler::IsBufferFull()
{
    if (((RankHandler*)myRank)->rbuffer[bufferId].size() >= MAX_BUFFER_SIZE)
    {
        bufferFull = true;	
        return true;
    }
    else 
        return false;
}

std::string BankHandler::toString()
{
    return "BankHandler";
}

bool BankHandler::MainScheduler(Request *req, int* index) 
{
    switch (dram_page_policy)
    {
        case OPEN_PAGE_POLICY:
        {
            switch (mc_scheduling_algorithm)
            {
                case PAR_BS:
                {
	            return PARBS(req,index);
                    break;
                }
                case FR_FCFS:
                {
	            return FRFCFS(req,index);
                    break;
                }
                case FC_FS:
                {
	            return FCFS(req,index);
                    break;
                }
                default:
                {
                    cout << "Wrong Scheduling Algorithm defined" << endl;
                    exit(1);
                }
            }
            break;
        }

        case CLOSE_PAGE_POLICY:
        {
            return FCFS(req,index);
            break;
        }

        default:
        {
            cout << "Wrong Page Policy" << endl;
            exit(1);
        }
    }
    cout << "not a likely place" << endl; 
    exit(1);
    return false;

}

bool BankHandler::PARBS(Request *req, int* index)
{
    UInt highest = FindHighest(); 
    RankHandler * tempRank = ((RankHandler*)myRank);			
    if (!tempRank->rbuffer[bufferId].empty())
    {
        if (RowHitFirst(req,index))
            return true;
        else if (ReadsFirst(req,index))
            return true; 
        else if (HighestRankedFirst(req, highest,index))
            return true;
        else if (OldestFirst(req,index))
            return true;
        else if (!tempRank->rbuffer[bufferId].empty())
        {
            if(ScheduleUnmarked(req,index))
                return true;
        }
        else 
            return false;
    }
    return false;		
} 

bool BankHandler::FRFCFS(Request *req, int* index)
{
    //   unsigned int i;
    RankHandler * tempRank = ((RankHandler*)myRank); 	
    if (!tempRank->rbuffer[bufferId].empty())
    {
        if (RowHitFirst(req,index))
            return true;
        else if (ReadsFirst(req,index))
            return true; 
        else if (OldestFirst(req,index))
            return true;
        else 
            return false;
    }
    return false;
}

bool BankHandler::FCFS(Request *req, int* index)
{
    // unsigned int i;
    RankHandler * tempRank = ((RankHandler*)myRank);
    if (!tempRank->rbuffer[bufferId].empty())
    {
        //	if (ReadsFirst(req,index))
        //           return true;   
        //	else 
        if (OldestFirst(req,index))
            return true;
        else
            return false;
    }
    return false;
} 

bool BankHandler::OldestFirst(Request* req, int* index)
{
    unsigned int i;
    bool temp = false;
    RankHandler * tempRank = ((RankHandler*)myRank);	
    // RankHandler * tempRank = (RankHandler*)myRank;
    for (i=0; i<tempRank->rbuffer[bufferId].size(); i++)	
    {
        if (!tempRank->rbuffer[bufferId][i].mark)
            continue;

        *req = tempRank->rbuffer[bufferId][i];
        if (req->bankNo==bankId)
        {
            if (req->rowNo==rowBufferIndex && rowOpen)
                req->status = OPEN;
            else if (rowOpen)
                req->status = CONFLICT;	
            else
                req->status = CLOSED;
            SetPrevState();
            SetReadsOWrite(OLDEST_FIRST,req->cmdType); 
            *index = i;
            rowBufferIndex = req->rowNo;
            if( dram_page_policy == OPEN_PAGE_POLICY)
                rowOpen = true;
            else if ( dram_page_policy == CLOSE_PAGE_POLICY )
                rowOpen = false;
            else
            {
                cout << " Incorrect dram_page_policy " << endl;
                exit (1);
            }
            temp = true;
            break;
        }
    }
    //    if (temp) cout << "Oldest " << req->status << endl;				
    return temp;
}

bool BankHandler::RowHitFirst(Request* req, int* index)
{
    unsigned int i;
    bool temp = false;
    RankHandler * tempRank = ((RankHandler*)myRank);
    for (i=0; i<tempRank->rbuffer[bufferId].size(); i++)	
    {
        if (!tempRank->rbuffer[bufferId][i].mark)
            continue;

        *req = tempRank->rbuffer[bufferId][i]; 
        if ( (req->bankNo==bankId) && (req->rowNo==rowBufferIndex) && rowOpen )
        {

            if (req->rowNo==rowBufferIndex && rowOpen)
                req->status = OPEN;
            else if (rowOpen)
                req->status = CONFLICT;	
            else
                req->status = CLOSED;	
            //    req->indexWhenScheduled = i; 
            SetPrevState();
            SetBypasses(i);
            SetReadsOWrite(HIT_FIRST,req->cmdType);
            *index = i;
            rowBufferIndex = req->rowNo;
            rowOpen = true;	
            temp = true;	
            break;
        }
    }	
    //    if (temp) cout << "Hit " << req->status << endl;			
    return temp;	
}

bool BankHandler::ReadsFirst(Request* req, int* index)
{
    unsigned int i;
    bool temp = false;
    RankHandler * tempRank = ((RankHandler*)myRank);
    for (i=0; i<tempRank->rbuffer[bufferId].size(); i++)	
    {
        if (!tempRank->rbuffer[bufferId][i].mark)
            continue;

        *req = tempRank->rbuffer[bufferId][i];
        if ( (req->bankNo==bankId) && 
             (req->cmdType == CACHE_READ || req->cmdType == CACHE_PREFETCH || req->cmdType == CACHE_WRITE) && 
             (tempRank->readsOWrite<MAX_READ_OV_WRITE) )
        {
            if (req->rowNo==rowBufferIndex && rowOpen)
                req->status = OPEN;
            else if (rowOpen)
                req->status = CONFLICT;	
            else
                req->status = CLOSED;	
            //    req->indexWhenScheduled = i;
            SetPrevState();
            SetBypasses(i); 
            SetReadsOWrite(READS_FIRST,req->cmdType);	
            *index = i;
            rowBufferIndex = req->rowNo;
            rowOpen = true;
            temp = true;
            break;
        }
    }	
    //    if (temp) cout << "Reads " << req->status << endl;	    			
    return temp;	
}

bool BankHandler::ScheduleUnmarked(Request *req, int* index)
{
    //    cout << "Maybe here I came" << endl;	
    unsigned int i;
    bool temp = false;
    RankHandler * tempRank = ((RankHandler*)myRank);
    for (i=0; i<tempRank->rbuffer[bufferId].size(); i++)	
    {
        *req = tempRank->rbuffer[bufferId][i];
        if ( req->bankNo==bankId )
        {
            if (req->rowNo==rowBufferIndex && rowOpen)
                req->status = OPEN;
            else if (rowOpen)
                req->status = CONFLICT;	
            else
                req->status = CLOSED;	
            //    req->indexWhenScheduled = i;
            SetPrevState();
            SetReadsOWrite(UNMARKED_FIRST,req->cmdType); 
            *index = i;
            rowBufferIndex = req->rowNo;
            rowOpen = true;
            temp = true;
            break;
        }
    }
    //    if (temp) cout << "Unmarked " << req->status << endl;						
    return temp;	
}	

bool BankHandler::HighestRankedFirst(Request *req, UInt highest, int* index)
{
    unsigned int i;
    bool temp = false;
    RankHandler * tempRank = ((RankHandler*)myRank);
    for (i=0; i<tempRank->rbuffer[bufferId].size(); i++)	
    {
        if (!tempRank->rbuffer[bufferId][i].mark)
            continue;

        *req = tempRank->rbuffer[bufferId][i];
        if (req->bankNo==bankId && (req->threadId==highest) )
        {
            if (req->rowNo==rowBufferIndex && rowOpen)
                req->status = OPEN;
            else if (rowOpen)
                req->status = CONFLICT;	
            else
                req->status = CLOSED;
            //    req->indexWhenScheduled = i;
            SetPrevState();
            SetBypasses(i);	
            SetReadsOWrite(HIGHEST_RANKED_FIRST,req->cmdType); 
            *index = i;
            rowBufferIndex = req->rowNo;
            rowOpen = true;
            temp = true;
            //	    cout << Simulator::Now() << ": I came here" << endl;	
            break;
        }
    }	
    //    if (temp) cout << "Highest Ranked " << req->status << endl;		
    return temp;	
}

UInt BankHandler::FindHighest()
{
    unsigned int i,j,k;
    int maxBankLoad[NO_OF_THREADS];	
    int totalLoad[NO_OF_THREADS];
    //   bool flag=0;
    RankHandler * tempRank = ((RankHandler*)myRank);	

    for (k=0; k<NO_OF_THREADS;k++)		// For Each Thread
    {	
        maxBankLoad[k]=0;			// Assign maxBankLoad & totalLoad to 0
        totalLoad[k]=0;				
        for (i=0;i<NO_OF_BUFFERS;i++)            	// for each Bank
        {
            int bankLoad=0;
            for (j=0; j<tempRank->rbuffer[i].size(); j++)	
            {	
                if (tempRank->rbuffer[i][j].mark==true && tempRank->rbuffer[i][j].threadId==k)
                {	
                    bankLoad++;
                    totalLoad[k]++;			// Set Total Load of a Thread k here as well
                }
            }
            //			cout << bankLoad << endl;
            //			cout << endl;
            if (maxBankLoad[k]<bankLoad)		// if maxBankLoad is less than bankload
                maxBankLoad[k]=bankLoad;			// set this bank's load to maxBankLoad
        }
    }

    for (j=0;j<NO_OF_THREADS;j++)			// if maxBankLoad is 0, 
        if (maxBankLoad[j]==0) 				// set it to high value so that
            maxBankLoad[j]=MAX_BUFFER_SIZE+1;		// its priority is the least

    for (j=0;j<NO_OF_THREADS;j++)				
        if (totalLoad[j]==0) 				// Similar for Total Load
            totalLoad[j]=MAX_BUFFER_SIZE+1;

    UInt highest=0;
    for (j=0;j<NO_OF_THREADS;j++)				// find the thread with highest bank load
        if (maxBankLoad[j]>maxBankLoad[highest])
            highest = j;
        else if (maxBankLoad[j]==maxBankLoad[highest] && totalLoad[j]>totalLoad[highest])
            highest = j;

    //	cout << highest << endl;
    return highest;

}

void BankHandler::SetReadsOWrite(CallerType caller, cache_command cmdType)
{
    RankHandler * tempRank = ((RankHandler*)myRank);
    if (cmdType == CACHE_READ || cmdType == CACHE_PREFETCH || cmdType == CACHE_WRITE)
        tempRank->readsOWrite++;

    switch (caller)
    {
        case HIT_FIRST:
            //	     if (cmdType != CACHE_READ && cmdType != CACHE_PREFETCH && cmdType != CACHE_WRITE)	
            //		tempRank->readsOWrite = 0;	
            break;
        case OLDEST_FIRST:
            if (cmdType != CACHE_READ && cmdType != CACHE_PREFETCH && cmdType != CACHE_WRITE)	
                tempRank->readsOWrite = 0;	
            break;
        case UNMARKED_FIRST:
            if (cmdType != CACHE_READ && cmdType != CACHE_PREFETCH && cmdType != CACHE_WRITE)	
                tempRank->readsOWrite = 0;	
            break;
        case HIGHEST_RANKED_FIRST:
            if (cmdType != CACHE_READ && cmdType != CACHE_PREFETCH && cmdType != CACHE_WRITE)	
                tempRank->readsOWrite = 0;
            break;
        default:
            break;
    }
    //      cout << dec << tempRank->readsOWrite << endl;	
}

void BankHandler::SetBypasses(unsigned int index)
{
    unsigned int i;
    RankHandler * tempRank = ((RankHandler*)myRank);

    for (i=0; i<tempRank->rbuffer[bufferId].size(); i++)	
    {
        if (tempRank->rbuffer[bufferId][i].bankNo == bankId)
            break;
    }

    if (index == i)
        bypassReq = 0;
    else if (index > i)
        bypassReq++;
    else
        cout << "Error: Bypasses got less than 1st req in the bank. Should not come here" << endl;			
}

void BankHandler::SetPrevState()
{
    RankHandler * tempRank = ((RankHandler*)myRank);
    prevBufferIndex = rowBufferIndex;
    prevRowOpen = rowOpen;
    prevBypassReq = bypassReq;
    tempRank->prevReadsOWrite = tempRank->readsOWrite;		
}

void BankHandler::RestorePrevState()
{
    RankHandler * tempRank = ((RankHandler*)myRank);
    rowBufferIndex = prevBufferIndex;
    rowOpen = prevRowOpen;
    bypassReq = prevBypassReq;
    tempRank->readsOWrite = tempRank->prevReadsOWrite;	
}
