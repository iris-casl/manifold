/*
 * =====================================================================================
 *
 *       Filename:  cmd_bus_handler.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/23/2010 07:06:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include <math.h>
#include "cmd_bus_handler.h"
#include "cmd_issuer.h"

using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CmdBusHandler
 *      Method:  CmdBusHandler
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

CmdBusHandler::CmdBusHandler()
{
    
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CmdBusHandler
 *      Method:  ~CmdBusHandler
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CmdBusHandler::~CmdBusHandler()
{

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CmdBusHandler
 *      Method:  process_event
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void CmdBusHandler::process_event(IrisEvent* e)
{
  //  DRAMCmdState *cmd = new DRAMCmdState();
    IrisEvent *event = new IrisEvent();
    DRAMCmdState* cmd = ((DRAMCmdState*)e->event_data.at(0));	//TODO needs to set this through manifold kernel's links
    event->src = (Component*)this;
    event->dst = (Component*)child;
    event->type = START_READ;	
    event->event_data.push_back((void*)cmd);	//TODO needs to set this through manifold kernel's links
#ifdef DEEP_DEBUG
    cout << dec << Simulator::Now() << ": " << hex << cmd->req.address << ": Sending " << dec << cmd->cmd << " to DRAM " << endl;	
#endif
    Simulator::Schedule(Simulator::Now()+t_CMD, &DRAMChannel::process_event, (DRAMChannel*)event->dst, event);  

 //   delete (DRAMCmdState*)e->event_data.at(0);  	
    delete e;
}

