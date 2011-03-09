/*
 * =====================================================================================
 *
 *       Filename:  cmd_issuer.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/21/2010 07:21:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  cmd_iss_h_INC
#define  cmd_iss_h_INC

#include "cmd_issuer.h"
#include "bus_handler.h"

using namespace std;

CmdIssuer::CmdIssuer()
{
    Request *req = new Request();
    prevState.prevCmdTime = 0;
    prevState.prevCmdRank = NO_OF_RANKS+1;
    prevState.prevCmdBank = NO_OF_BANKS+1;
    prevState.cmd.set(PRECHARGE,NORMAL,*req);    
    busBusy = 0;
    generated_start_event = false;
    delete req;			
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CmdIssuer
 *      Method:  ~CmdIssuer
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
CmdIssuer::~CmdIssuer()
{

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  CmdIssuer
 *      Method:  process_event
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
void CmdIssuer::process_event(IrisEvent* e)
{   
    bool isWrite = false;
    switch (e->type)
    {
	case START:
	    
	    ((BusHandler*)parent)->SetIfFull();
	    ((BusHandler*)parent)->linkBusy[Id] = false;	
            generated_start_event = false;
            if (HasWork())
            {
		DRAMCmdState *cmd = new DRAMCmdState();
                if (IssueCmd(&isWrite,cmd))	
                {
#ifdef DEEP_DEBUG
		    cout << dec << Simulator::Now() << ": " << hex << cmd->req.address << ": Issuing " << dec << cmd->cmd << " with following isWrite signal " << isWrite << endl;	
#endif	
		    if (cmd->cmd == READ || cmd->cmd == WRITE)
		    {	
			cmd->req.busInsertionTime = (ullint)Simulator::Now()+1;	
		    }

                    if( dram_page_policy == OPEN_PAGE_POLICY )
                    {
                        if (cmd->cmd == READ || cmd->cmd == WRITE)
                        {
                            ((BusHandler*)parent)->oneReq[cmd->req.channelNo][cmd->req.rankNo][cmd->req.bankNo] = false;
                        }
                    }
                    else if ( dram_page_policy == CLOSE_PAGE_POLICY )
                    {
                        if (cmd->cmd == PRECHARGE)
                        {
                            ((BusHandler*)parent)->oneReq[cmd->req.channelNo][cmd->req.rankNo][cmd->req.bankNo] = false;
                        }
                    }
                    else
                    {
                        cout << " Error: Undefine DRAM page policy " << endl;
                        exit(1);
                    }

                    IrisEvent *event = new IrisEvent();
		    event->src = (Component*)this;
        	    event->dst = (Component*)cmdBus;
	            event->event_data.push_back((void*)cmd);	//TODO needs to set this through manifold kernel's links
		    event->type = START;
		    Simulator::Schedule(Simulator::Now()+1, &CmdBusHandler::process_event, (CmdBusHandler*)event->dst, event);
                }
		else 
		   delete cmd;
		
                if (isWrite)
                {
		    DRAMCmdState *cmd2 = new DRAMCmdState();
		    *cmd2 = *cmd;	
                    IrisEvent *event3 = new IrisEvent();
                    Time delay = CalculateDataDelay(*cmd2);
                    event3->src = (Component*)this;
                    event3->dst = (Component*)dataBus;
                    event3->event_data.push_back((void*)cmd2);	//TODO needs to set this through manifold kernel's links
                    event3->type = START_WRITE;
                    Simulator::Schedule(Simulator::Now()+delay+1, &DataBusHandler::process_event, (DataBusHandler*)event3->dst, event3);
                    // issue Data Bus Command as well if write
                }
		
	//	if (HasWork())
            if( !generated_start_event)
		{
                    generated_start_event = true;
		IrisEvent *event2 = new IrisEvent();
		event2->src = (Component*)this;
                event2->dst = (Component*)this;	
                event2->type = START;
                Simulator::Schedule(Simulator::Now()+1, &CmdIssuer::process_event, (CmdIssuer*)event2->dst, event2);
		}
	//	else
	//	    ((BusHandler*)parent)->SetIfFull();	
            }      
	    break;	

        default:
                 //cerr <<  "CmdIssuer There should be no default case as such but OK /n";
        break;
    }
    delete e;
}

bool CmdIssuer::HasWork()
{
    return (!((BusHandler*)parent)->cmdQueue[bufferId].empty());
}

std::string CmdIssuer::toString()
{
	return "CmdIssuer";
}

bool CmdIssuer::IssueCmd(bool *isWrite, DRAMCmdState *scheduledCmd)
{
        DRAMCmdState prevCmd;
        bool temp = false;
        vector<DRAMCmdState>::iterator bufferIndex = ((BusHandler*)parent)->cmdQueue[bufferId].begin();
        BusHandler * parent2 = ((BusHandler*)parent);
        *isWrite = false;
	prevCmd = prevState.cmd;

	for (unsigned int i=0; i<parent2->cmdQueue[bufferId].size(); i++)
	{	
	    if (!BusNotBusy(parent2->cmdQueue[bufferId][i]) )//&&		// Check whether Data Bus is busy only for Read & Write cmds
            {   bufferIndex++;
                continue;
            }
	    if (!BanksFirstReq(parent2->cmdQueue[bufferId][i],i)	)//&&	// Low Level cmds within a bank has to be serialized
            {   bufferIndex++;
                continue;
            }
            if (BankNotBusy(parent2->cmdQueue[bufferId][i]) ) 		// Check whether the bank is busy or not
	    {		
		if (CanSchedule(parent2->cmdQueue[bufferId][i],prevCmd))
		{	// Check the timing constraints with last cmd	
		    *scheduledCmd = parent2->cmdQueue[bufferId][i];  // issue command to command bus
                    if (parent2->cmdQueue[bufferId][i].cmd==WRITE)
                        *isWrite = true;
		    SetPrevState(parent2->cmdQueue[bufferId][i]);
        	    SetBusBusyTime(*scheduledCmd);  	        
                    parent2->cmdQueue[bufferId].erase(bufferIndex);	// erase the scheduled command;
                    temp = true;
		    break;
		 } 	
	     }
	     bufferIndex++;
	}
	return temp;	
}

bool CmdIssuer::BusNotBusy(DRAMCmdState currentCmd)
{

    if (currentCmd.cmd == READ || currentCmd.cmd == WRITE)
    {
        Time temp;
        if (currentCmd.cmd == WRITE && prevBusCmd == READ)	   
    	    temp = (ullint)busBusy+t_CMD;
        else if (currentCmd.cmd == READ && prevBusCmd == WRITE)	   
    	    temp = (ullint)busBusy+t_CMD;
        else if (currentCmd.cmd == READ || currentCmd.cmd == WRITE)
            temp = busBusy;
    
         if (temp<=Simulator::Now())
            return true;
        else 
            return false;
    }
    else
        return true;
			
}

void CmdIssuer::SetBusBusyTime(DRAMCmdState currentCmd)
{
  //  if (currentCmd.cmd == WRITE && prevBusCmd == READ)	   
    //	busBusy = Simulator::Now()+CalculateBurstL(currentCmd);
//    else if (currentCmd.cmd == READ && prevBusCmd == WRITE)	   
//	busBusy = Simulator::Now()+CalculateBurstL(currentCmd);
    if (currentCmd.cmd == READ || currentCmd.cmd == WRITE)
    {	
        busBusy = (ullint)Simulator::Now()+CalculateBurstL(currentCmd);
        prevBusCmd  = currentCmd.cmd;
    }
}

bool CmdIssuer::BanksFirstReq(DRAMCmdState currentCmd, unsigned int index)
{
    BusHandler * parent2 = ((BusHandler*)parent);	
    for (unsigned int i=0; i<index; i++)
    {
	if (parent2->cmdQueue[bufferId][i].req.bankNo == currentCmd.req.bankNo)
	    return false;	
    }
    return true;	 
}

bool CmdIssuer::BankNotBusy(DRAMCmdState currentCmd)
{
   if (currentCmd.cmd == ALL_BANK_REFRESH /*&& currentCmd.req.rankNo == prevState.prevCmdRank*/)
   {
	for (unsigned int i=0; i<NO_OF_BANKS; i++)
	{
	    Time prevTime = prevState.rank[currentCmd.req.rankNo].bank[i].prevCmdTime;
	    DRAMCmd prevCmd = prevState.rank[currentCmd.req.rankNo].bank[i].prevCmd;
    
            Time prevBurstL = CalculateBurstL(prevState.cmd);	
            Time busyTime = CalculateBusyTime(currentCmd.cmd, prevCmd, SAME, SAME, prevBurstL);

        if (prevTime+busyTime>Simulator::Now()) 
	    return false;
	}
	return true;
   }
   else
//	if (currentCmd.req.rankNo == prevState.prevCmdRank && currentCmd.req.bankNo == prevState.prevCmdBank)
    {		
	Time prevTime = prevState.rank[currentCmd.req.rankNo].bank[currentCmd.req.bankNo].prevCmdTime;
	DRAMCmd prevCmd = prevState.rank[currentCmd.req.rankNo].bank[currentCmd.req.bankNo].prevCmd;
//	if (prevTime+CmdDelay(prevCmd)<=Simulator::Now())
//	    return true;
//	else
//	    return false;	
    
        Time prevBurstL = CalculateBurstL(prevState.cmd);	
        Time busyTime = CalculateBusyTime(currentCmd.cmd, prevCmd, SAME, SAME, prevBurstL);

        if (prevTime+busyTime<=Simulator::Now()) 
	    return true;
	else
	    return false;
   }
}

Time CmdIssuer::CmdDelay(DRAMCmd cmd)
{
    Time cmdTime = 0;
	//// TODO ///////
    if (cmd == ACTIVATE)
	cmdTime = (ullint)t_RCD;
    else if (cmd == PRECHARGE)
	cmdTime = t_RP;
    else if (cmd == READ || cmd == WRITE)
	cmdTime = t_CAS;
    else if (cmd == ALL_BANK_REFRESH)
	cmdTime = t_RFC;
		
    return cmdTime;
}

Time CmdIssuer::CalculateDataDelay(DRAMCmdState cmdS)
{
    Time delay = 0; 
    if (cmdS.cmd == WRITE)
    {
        delay = t_CWD+t_CMD;
    }
    else
        cout << "Error: calculating data bus delay for non write cmd\n";
    return delay;	
}

bool CmdIssuer::CanSchedule(DRAMCmdState currentCmd, DRAMCmdState prevCmd)
{
    RankBankComb rankC;
    RankBankComb bankC;
    Time prevTime = prevState.prevCmdTime;		

    if (currentCmd.req.rankNo==prevCmd.req.rankNo)
	rankC = SAME;
    else 
	rankC = DIFF;
	
    if (currentCmd.req.bankNo==prevCmd.req.bankNo)
	bankC = SAME;
    else 
	bankC = DIFF;

    Time prevBurstL = CalculateBurstL(prevCmd);	
    Time busyTime = CalculateBusyTime(currentCmd.cmd, prevCmd.cmd, rankC, bankC, prevBurstL);

    if (prevTime+busyTime<=Simulator::Now()) 
	return true;
    else
	return false;
}

Time CmdIssuer::CalculateBurstL(DRAMCmdState prevCmd)
{
    Time t_Burst = 0;	
    switch (prevCmd.burst)
    {
        case READL:
	    t_Burst = (ullint)ceil(ceil(READ_SIZE/DDR_BUS_WIDTH)*BUS_CYCLE); 
	    break;
        case WRITEL:
	    t_Burst = (ullint)ceil(ceil(WRITE_SIZE/DDR_BUS_WIDTH)*BUS_CYCLE); 	
	    break;
        case PREFETCHL:
	    t_Burst = (ullint)ceil(ceil(PREFETCH_SIZE/DDR_BUS_WIDTH)*BUS_CYCLE); 	
	    break;
	case WRITEBACKL:
	    t_Burst = (ullint)ceil(ceil(WRITEBACK_SIZE/DDR_BUS_WIDTH)*BUS_CYCLE); 
	    break; 
         default:
	     break;
    }
    return t_Burst;
}

Time CmdIssuer::CalculateBusyTime(DRAMCmd curCmd, DRAMCmd prevCmd, RankBankComb rankC, RankBankComb bankC, Time prevBurstL)
{
    Time busyTime=0;
    Time t_Burst = prevBurstL;
    busyTime += t_CMD;	

    switch (prevCmd)
    {
        case ACTIVATE:
            switch(curCmd)
            {
                case ACTIVATE:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case SAME:
                                    busyTime = 999999; // infinity	
                                    //   cout << "Should not do activate and activate again to same bank\n";//busyTime = t_RC;
                                    break;
                                case DIFF:
                                    busyTime = t_RRD;  
                                    break; 	
                                default:
                                    break; 
                            }			    
                            break;
                        default:
                            break;
                    }
                    break;
                case PRECHARGE:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case SAME:
                                    busyTime = t_RAS;
                                    break;
                                default:
                                    break; 
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case READ:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case SAME:
                                    busyTime = t_RCD-t_AL;
                                    break;
                                default:
                                    break; 
                            }
                            break;
                        default:
                            break;
                    }

                    break;
                case WRITE:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case SAME:
                                    busyTime = t_RCD-t_AL;
                                    break;
                                default:
                                    break; 
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case ALL_BANK_REFRESH:
                    switch(rankC)
                    {
                        case SAME:
                            busyTime = t_RAS;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;	
            }
            break;
        case PRECHARGE:
            switch(curCmd)
            {
                case ACTIVATE:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case DIFF:	
                                    busyTime = t_RP;
                                    break;
                                default:
                                    break;
                            }     
                            break;
                        default:
                            break;
                    }
                    break;
                case PRECHARGE:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case SAME:	
                                    busyTime = t_RP;
                                    break;
                                default:
                                    break;
                            }
                        case DIFF:
                            busyTime = t_RTRS;
                            break;
                        default:
                            break;
                    }
                    break;
                case READ:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case SAME:
                                    busyTime = 999999; // infinity
                                    //    cout << "cannot directly do READ: after precharge\n";
                                    break;
                                default:
                                    break; 
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case WRITE:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case SAME:
                                    busyTime = 999999; // infinity	
                                    //cout << "cannot directly do WRITE: after precharge\n";
                                    break;
                                default:
                                    break; 
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case ALL_BANK_REFRESH:
                    switch(rankC)
                    {
                        case SAME:
                            busyTime = t_RP;
                            break;
                        case DIFF:
                            busyTime = t_RTRS;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;	
            }
            break;
        case READ:
            switch(curCmd)
            {
                case ACTIVATE:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case SAME:
                                    busyTime = 999999; // infinity	
                                    //cout << "ACTIVATE: after READ: to the same bank: cannot happen\n";
                                    break;
                                default:
                                    break; 
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case PRECHARGE:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case SAME:
                                    busyTime = (t_AL + t_Burst + t_RTP - t_CCD); 
                                    break;
                                default:
                                    break; 
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case READ:
                    switch(rankC)
                    {
                        case SAME:
                            busyTime = Max(t_Burst,t_CCD);
                            break;
                        case DIFF:
                            busyTime = Max(t_Burst,t_CCD) + t_RTRS; 
                            break;
                        default:
                            break;
                    }
                    break;
                case WRITE:
                    busyTime = t_CAS + t_Burst + t_RTRS - t_CWD; 
                    break;
                case ALL_BANK_REFRESH:
                    switch(rankC)
                    {
                        case SAME:
                            busyTime = (t_AL + t_Burst + t_RTP - t_CCD); 
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;	
            }
            break;
        case WRITE:
            switch(curCmd)
            {
                case ACTIVATE:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case SAME:
                                    busyTime = 999999; // infinity
                                    //cout << "Activate After WRITE: to the same bank cannot happen\n";
                                    break;
                                default:
                                    break; 
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case PRECHARGE:
                    switch(rankC)
                    {
                        case SAME:
                            switch(bankC)
                            {	
                                case SAME:
                                    busyTime = t_AL + t_Burst + t_CWD + t_WR;
                                    break;	
                                default:
                                    break; 
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case READ:
                    switch(rankC)
                    {
                        case SAME:
                            busyTime = t_CWD + t_Burst + t_WTR;
                            break;
                        case DIFF:
                            busyTime = t_CWD + t_Burst + t_RTRS - t_CAS;
                            break;
                        default:
                            break;
                    }
                    break;
                case WRITE:
                    switch(rankC)
                    {
                        case SAME:
                            busyTime = Max(t_Burst, t_CCD);
                            break;
                        case DIFF:
                            busyTime = Max(t_Burst, t_CCD) + t_OST;
                            break;
                        default:
                            break;
                    }
                    break;
                case ALL_BANK_REFRESH:
                    switch(rankC)
                    {
                        case SAME:
                            busyTime = t_AL + t_Burst + t_CWD + t_WR;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;	
            }
            break;
        case ALL_BANK_REFRESH:
            switch(rankC)
            {
                case DIFF:
                    busyTime = t_RTRS;
                    break;
                case SAME:
                    if(dram_page_policy == OPEN_PAGE_POLICY)
                        busyTime = t_RP + t_RFC + t_RCD;	// First precharge the active row and then refresh and then activate the row again
                    else if (dram_page_policy == CLOSE_PAGE_POLICY)
                        busyTime = t_RFC;	
                    else
                    {
                        cout << " Error: Undefine DRAM page policy " << endl;
                        exit(1);
                    }
                default:
                    break;
            }	
            break;
        default:
            break;
    }	
    return busyTime;
}

void CmdIssuer::SetPrevState(DRAMCmdState currentCmd)
{
    prevState.prevCmdRank = currentCmd.req.rankNo;
    prevState.prevCmdBank = currentCmd.req.bankNo;
    prevState.prevCmdTime = (ullint)Simulator::Now();
    prevState.cmd = currentCmd;

    prevState.rank[currentCmd.req.rankNo].bank[currentCmd.req.bankNo].prevRow = currentCmd.req.rowNo;
    prevState.rank[currentCmd.req.rankNo].bank[currentCmd.req.bankNo].prevCmd = currentCmd.cmd;
    prevState.rank[currentCmd.req.rankNo].bank[currentCmd.req.bankNo].prevCmdTime = (ullint)Simulator::Now(); 
}

Time CmdIssuer::Max(Time t1, Time t2)
{
    if (t1>=t2)
        return t1;
    else 
        return t2;
}

#endif
