/*
 * =====================================================================================
 *
 *       Filename:  MC.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/07/2010 07:16:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#include "MC.h"

void MC::Init()
{
    reqH->mc = (Component*)this;
    bus->mc = (Component*)this;
    dram->mc = (Component*)this;
    responseH->mc = (Component*)this;
    refMgr->mc = (Component*)this;
//    ni->mc = (Component*)this;
    reqH->parent = (Component*)ni;
    reqH->child  = (Component*)bus;
    bus->parent = (Component*)reqH;
    bus->child1 = (Component*)dram; 	// dram link
    bus->child2 = (Component*)responseH; //response link	
    dram->parent = (Component*)bus; 
    responseH->parent = (Component*)bus;
    responseH->child = (Component*)ni;			
    reqH->SetLinks();	// TODO need to fix these for parallel versions
    bus->SetLinks();	// No ptrs thrown here & there
    dram->SetLinks();	
    reqH->resPtr = (Component*)responseH;
    responseH->reqPtr = (Component*)reqH;		
    refMgr->reqPtr = reqH;
    
    StartRefresh();
    stats->mc = (Component*)this; 	
    stats->InitStats();	
}

void MC::StartRefresh()
{
    refMgr->nextRefresh = static_cast<ullint>(REFRESH_INC);	
    IrisEvent *event = new IrisEvent();
    event->src = (Component*)this;
    event->dst = (Component*)refMgr;		    	
    Simulator::Schedule(Simulator::Now()+refMgr->nextRefresh, &RefreshMgr::process_event, (RefreshMgr*)event->dst, event);    	
}
