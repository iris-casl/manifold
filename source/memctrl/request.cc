/*
 * =====================================================================================
 *
 *       Filename:  request.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 02:50:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include "request.h"

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Request
 *      Method:  Request
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
Request::Request()
{
    mark = true;
    mcNo = 0;	    
    channelNo = NO_OF_CHANNELS+1;
    dimmNo = 0;    
    rankNo = NO_OF_RANKS+1;    
    bankNo = NO_OF_BANKS+1;    
    columnNo = NO_OF_COLUMNS+1;
    rowNo = NO_OF_ROWS+1;
//    cmdType = CACHE_READ;
    threadId = 0;
    address = 0;
    lowerBits = 0;
    
    tag = 0;	
//    indexWhenScheduled = 0;
    local = true;
    scheduledInMSHR = false;
    serviced = false;
    
    child_req=NULL;
    mother_req=NULL;
    busInsertionTime = 0;
    rbufferInsertionTime = 0;
    cbufferInsertionTime = 0;
    arrivalTime = 0;
    retireTime = 0;
    startTime = 0;	
    avg_network_latency = 0;
    hop_count = 0;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Request
 *      Method:  Request
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
Request::~Request()
{

}
