/*
 * =====================================================================================
 *
 *       Filename:  channel_handler.cc
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
 *       Class:  ChannelHandler
 *      Method:  ChannelHandler
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

#include <math.h>
#include "channel_handler.h"

using namespace std;

ChannelHandler::ChannelHandler()
{
    rank.resize(NO_OF_RANKS);    
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  ChannelHandler
 *      Method:  ~ChannelHandler
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
ChannelHandler::~ChannelHandler()
{
	
}

