/*
 * =====================================================================================
 *
 *       Filename:  data_bus_handler.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/23/2010 07:05:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include <math.h>
#include "data_bus_handler.h"
#include "cmd_issuer.h"
#include "response_handler.h"

using namespace std;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  DataBusHandler
 *      Method:  DataBusHandler
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */

DataBusHandler::DataBusHandler()
{
   prevTime = 0; 
   busBusyTime = 0;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  DataBusHandler
 *      Method:  ~DataBusHandler
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
DataBusHandler::~DataBusHandler()
{

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  DataBusHandler
 *      Method:  process_event
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void DataBusHandler::process_event(IrisEvent* e)
{
  //  DRAMCmdState *cmd = new DRAMCmdState();
    IrisEvent *event = new IrisEvent();
    DRAMCmdState* cmd = ((DRAMCmdState*)e->event_data.at(0));	//TODO needs to set this through manifold kernel's links  	
    int burstDelay=0;	
    burstDelay = (ullint)ceil(ceil(cmd->req.data.size*1.0/DDR_BUS_WIDTH)*BUS_CYCLE);

    switch (e->type)
    {	
	case START_READ:
	     event->src = (Component*)this;
	     event->dst = (Component*)child2;		// towards response handler
	     event->type = REPLY;	
	     event->event_data.push_back((void*)cmd);	//TODO needs to set this through manifold kernel's links
#ifdef DEEP_DEBUG
	     cout << dec << Simulator::Now() << ": " << hex << cmd->req.address << ": DataBus!! Receiving data from DRAM. Will take cycles = " << dec << burstDelay << endl;
#endif	
             ((DRAMChannel*)child1)->dramReadCycles += burstDelay;
	     Simulator::Schedule(Simulator::Now()+burstDelay+1, &ResponseHandler::process_event, (ResponseHandler*)event->dst, event); 
	     break;

	case START_WRITE:	
	     event->src = (Component*)this;
	     event->dst = (Component*)child1;		// towards DRAM handler
	     event->type = START_WRITE;	
	     event->event_data.push_back((void*)cmd);	//TODO needs to set this through manifold kernel's links
#ifdef DEEP_DEBUG
	     cout << dec << Simulator::Now() << ": " << hex << cmd->req.address << ": DataBus!! Sending data to be written to DRAM. Will take cycles = " << dec << burstDelay << endl;	
#endif
	     ((DRAMChannel*)child1)->dramWriteCycles += burstDelay;
	     Simulator::Schedule(Simulator::Now()+burstDelay+1, &DRAMChannel::process_event, (DRAMChannel*)event->dst, event); 
	     break;
	default:
		delete cmd;
		delete event;
		cout << Simulator::Now() << ": I should not come to default of process event in data bus\n";
    }
   if (prevTime + burstDelay > Simulator::Now())
   {	cerr << "Error in databus" << endl;
   	exit(1);
   }
   prevTime = (ullint)Simulator::Now();


   busBusyTime += burstDelay;
   ((DRAMChannel*)child1)->dramBusyTime = (ullint)Simulator::Now()+burstDelay;		
   ((DRAMChannel*)child1)->dramBankBusyTime[cmd->req.rankNo][cmd->req.bankNo] = (ullint)Simulator::Now()+burstDelay;
   ((DRAMChannel*)child1)->dramBusyCycles += (burstDelay-t_CMD);
   ((DRAMChannel*)child1)->dramBankBusyCycles[cmd->req.rankNo][cmd->req.bankNo] += (burstDelay-t_CMD);

 //  delete (DRAMCmdState*)e->event_data.at(0);
   delete e;
}

