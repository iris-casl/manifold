/*
 * =====================================================================================
 *
 *       Filename:  bus_handler.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2010 11:48:34 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include <math.h>
#include "bus_handler.h"
#include "request_handler.h"
#include "bus.h"

using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  BusHandler
 *      Method:  BusHandler
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

BusHandler::BusHandler()
{
    cmdIssuer.resize(NO_OF_CHANNELS);
    linkBusy.resize(NO_OF_CHANNELS);
    full.resize(NO_OF_CHANNELS);
    oneReq.resize(NO_OF_CHANNELS);
    cmdQueue.resize(NO_OF_CHANNELS);
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
        oneReq[i].resize(NO_OF_RANKS);
    
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
        for (unsigned int j=0; j<NO_OF_RANKS; j++)
            oneReq[i][j].resize(NO_OF_BANKS);

    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {
	full[i] = false;
	stopSignal = false;
	linkBusy[i] = false;	
	cmdIssuer[i].parent = (Component*)this;
	cmdIssuer[i].Id = i;
	cmdIssuer[i].bufferId = i;
        for (unsigned int j=0; j<NO_OF_RANKS; j++)
        {
    	    for (unsigned int k=0; k<NO_OF_BANKS; k++)
    	    {
		oneReq[i][j][k] = false;
	    }
	}
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  BusHandler
 *      Method:  ~BusHandler
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
BusHandler::~BusHandler()
{

}

std::string BusHandler::toString()
{
	return "BusHandler";
}

void BusHandler::SetIfFull()
{
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
    {	
    	if (cmdQueue[i].size() >= MAX_CMD_BUFFER_SIZE-2)
	    full[i] = true;
	else if (!stopSignal)
	    full[i] = false;	
    }	
}
bool BusHandler::IsFull(int i)
{
    return full[i];
}
bool BusHandler::BankRankFree(unsigned int bankNo, unsigned int rankNo, unsigned int channelNo)
{
    return !oneReq[channelNo][rankNo][bankNo];
}

void BusHandler::LowLevelCmdGen(Request *req)
{
    DRAMCmdState cmdState;	
    DRAMCmdState cmdState2;
    DRAMCmdState cmdState3;	

switch (dram_page_policy)
{
  case OPEN_PAGE_POLICY:
  {
    if (req->cmdType == REFRESH)
    {
	cmdState.set(ALL_BANK_REFRESH,NORMAL,*req);
	for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
	    cmdQueue[i].push_back(cmdState);
    }
    else
    {
        switch (req->status)
    	{
            case OPEN:
                  break;  
            case CLOSED:
		  cmdState2.set(ACTIVATE,NORMAL,*req);
		  cmdQueue[req->channelNo].push_back(cmdState2);	
		  break;
            case CONFLICT:
		  cmdState3.set(PRECHARGE,NORMAL,*req);
		  cmdQueue[req->channelNo].push_back(cmdState3);
		  cmdState2.set(ACTIVATE,NORMAL,*req);
		  cmdQueue[req->channelNo].push_back(cmdState2);
                  break;
            case IDLE:
                  cout << "why I came in IDLE in CmdGen??" << endl;
                  break;
            default:
                  cout << "I cannot came in default of request status in CmdGen??" << endl;
                  break;
        }

        if (req->status == OPEN || req->status == CLOSED || req->status == CONFLICT)
	{
	    switch (req->cmdType)
	    {
        	case CACHE_READ: 
        	      cmdState.set(READ,READL,*req);
        	      break;
        	case CACHE_WRITE:
        	      cmdState.set(READ,WRITEL,*req);
        	      break;
     	        case CACHE_WRITEBACK:
        	      cmdState.set(WRITE,WRITEBACKL,*req);
        	      break;
        	case CACHE_PREFETCH:
        	      cmdState.set(READ,PREFETCHL,*req);
        	      break;
        	case REFRESH:
		      cout << "should not come to refresh\n";
        	      break;
        	default: 
        	      break;
            }	
	    cmdQueue[req->channelNo].push_back(cmdState);
	}
    }
    break;
  }

  case CLOSE_PAGE_POLICY:
  {
    if (req->cmdType == REFRESH)
    {
	cmdState.set(ALL_BANK_REFRESH,NORMAL,*req);
	for (unsigned int i=0; i<NO_OF_CHANNELS; i++)
	    cmdQueue[i].push_back(cmdState);
    }
    else
    {
	  cmdState2.set(ACTIVATE,NORMAL,*req);
	  cmdQueue[req->channelNo].push_back(cmdState2);	

        if (req->status == OPEN || req->status == CLOSED || req->status == CONFLICT)
	{
	    switch (req->cmdType)
	    {
        	case CACHE_READ: 
        	      cmdState.set(READ,READL,*req);
        	      break;
        	case CACHE_WRITE:
        	      cmdState.set(READ,WRITEL,*req);
        	      break;
     	        case CACHE_WRITEBACK:
        	      cmdState.set(WRITE,WRITEBACKL,*req);
        	      break;
        	case CACHE_PREFETCH:
        	      cmdState.set(READ,PREFETCHL,*req);
        	      break;
        	case REFRESH:
		      cout << "should not come to refresh\n";
        	      break;
        	default: 
        	      break;
            }	
	    cmdQueue[req->channelNo].push_back(cmdState);
	    cmdState3.set(PRECHARGE,NORMAL,*req);
	    cmdQueue[req->channelNo].push_back(cmdState3);
	}
    }
    break;
  }
  default:
  {
      cout << "Wrong page policy in bus" << endl;
      exit(1);
  }
}

}
