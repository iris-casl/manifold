/*
 * =====================================================================================
 *
 *       Filename:  rank_handler.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 05:33:10 PM
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
 *       Class:  RankHandler
 *      Method:  RankHandler
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

#include <math.h>
#include "rank_handler.h"

using namespace std;

RankHandler::RankHandler()
{
    readsOWrite = 0;
    prevReadsOWrite = 0;
    bank.resize(NO_OF_BANKS);
    rbuffer.resize(NO_OF_BUFFERS);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  RankHandler
 *      Method:  ~RankHandler
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
RankHandler::~RankHandler()
{
	
}
