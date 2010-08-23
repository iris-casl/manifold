/*
 * =====================================================================================
 *
 *       Filename:  refresh_manager.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2010 02:53:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
/*
 *--------------------------------------------------------------------------------------
 *       Class:  RefreshMgr
 *      Method:  RefreshMgr
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

#include <math.h>
#include "refresh_manager.h"

using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  RefreshMgr
 *      Method:  RefreshMgr
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

RefreshMgr::RefreshMgr()
{
    nextRefresh = 0;
    currentRankNo = 0;
    currentRowNo = 0;		
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  RefreshMgr
 *      Method:  ~RefreshMgr
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
RefreshMgr::~RefreshMgr()
{
	
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  RefreshMgr
 *      Method:  process_event
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void RefreshMgr::process_event(IrisEvent* e)
{
    IrisEvent *event = new IrisEvent();
    Request *req = new Request();
    req->cmdType = REFRESH;
    req->rankNo = currentRankNo;
    req->rowNo = currentRowNo;	
    currentRankNo = (currentRankNo + 1) % NO_OF_RANKS;
    if (currentRankNo == 0)
	currentRowNo = (currentRowNo + (ROW_SIZE*NO_OF_RANKS/8192)) % NO_OF_ROWS;    		
    reqPtr->busHandler->LowLevelCmdGen(req);	
    for (unsigned int i=0; i<NO_OF_CHANNELS; i++)		
    {
	event->src = (Component*)this;
    	event->dst = (Component*)(&reqPtr->busHandler->cmdIssuer[i]);
	event->type = START;
 //   	event->event_data.push_back((void*)req);
    	Simulator::Schedule(Simulator::Now()+1, &CmdIssuer::process_event, (CmdIssuer*)event->dst, event);   
    }
#ifdef DEEP_DEBUG
cout << dec << Simulator::Now() << ": Refresh Manager: Now I am sending all bank refresh command to cmd queue for rank: " << req->rankNo << ", row: " << req->rowNo << " of all channels simultaneously\n";	
#endif
    nextRefresh = static_cast<ullint>(REFRESH_INC);	
    IrisEvent *event2 = new IrisEvent();
    event2->src = (Component*)this;
    event2->dst = (Component*)this;		    	
    Simulator::Schedule(Simulator::Now()+nextRefresh, &RefreshMgr::process_event, (RefreshMgr*)event2->dst, event2);    
    delete req;	
    delete e;
}
