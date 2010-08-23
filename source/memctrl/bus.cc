/*
 * =====================================================================================
 *
 *       Filename:  bus.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/05/2010 05:36:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include <math.h>
#include "bus.h"

using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Bus
 *      Method:  Bus
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

Bus::Bus()
{

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Bus
 *      Method:  ~Bus
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
Bus::~Bus()
{

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Bus
 *      Method:  process_event
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void Bus::process_event(IrisEvent* e)
{
   delete e;
}

void Bus::SetLinks()
{
    for(short i=0; i<NO_OF_CHANNELS; i++)
    {
	dataBus[i].parent = this;
	cmdBus[i].parent = this;
    	dataBus[i].child1 = (Component*)(&((DRAM*)child1)->dc[i]);	// dram side link
    	dataBus[i].child2 = (Component*)child2;					// response side link	
	cmdBus[i].child = (Component*)(&((DRAM*)child1)->dc[i]);
    }	
}
std::string Bus::toString()
{
//	cout << bufferId <<endl;
	return "Bus";
}


