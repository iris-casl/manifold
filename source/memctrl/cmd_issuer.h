/*
 * =====================================================================================
 *
 *       Filename:  cmd_issuer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/21/2010 09:41:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  cmd_issuer_h_INC
#define  cmd_issuer_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../simIris/data_types/impl/irisEvent.h"
#include        "../util/genericEvents.h"
#include        "request.h" 
#include	"data_bus_handler.h"
#include	"cmd_bus_handler.h"
#include	"../util/mc_constants.h"

using namespace std;

enum DRAMCmd {READ, WRITE, PRECHARGE, ACTIVATE, ALL_BANK_REFRESH};
typedef DRAMCmd DRAMCmd;

enum BurstLength {READL, WRITEL, PREFETCHL, WRITEBACKL, NORMAL};
typedef BurstLength BurstLength;

struct DRAMCmdState
{
	DRAMCmd cmd;
	Request req;
	BurstLength burst;
	void set(DRAMCmd cmdS, BurstLength burstL, Request reqI)
	{
	    cmd = cmdS;
	    req = reqI;	
	    burst = burstL;
	    
	    if (burst == READL)
		req.data.size = READ_SIZE;	/// TODO needs to set this
	    if (burst == WRITEL)
		req.data.size = WRITE_SIZE;
	    else if (burst == PREFETCHL)
		req.data.size = PREFETCH_SIZE;
	    else if (burst == WRITEBACKL)
		req.data.size = WRITEBACK_SIZE;	
	}
	DRAMCmdState () 
	{
	    cmd = PRECHARGE;
	    burst = NORMAL;		
	};
	~DRAMCmdState () { };
};
typedef DRAMCmdState DRAMCmdState;
typedef vector<DRAMCmdState> CmdQueue;

enum RankBankComb {SAME, DIFF};

struct BankState
{
    unsigned long long int prevRow;
    DRAMCmd prevCmd;
    Time prevCmdTime;
    BankState ()
    {
	prevRow = 0;
        prevCmd = PRECHARGE;
	prevCmdTime = 0;	    	
    }	
};
typedef BankState BankState; 

struct RankState
{
	vector<BankState> bank;
        RankState()
        {
            bank.resize(NO_OF_BANKS);
        }
};
typedef RankState RankState;

struct ChannelState
{
	vector<RankState> rank;
	unsigned int prevCmdRank;
	unsigned int prevCmdBank;
	Time prevCmdTime;
	DRAMCmdState cmd;
        ChannelState()
        {
            rank.resize(NO_OF_RANKS);
        }
};
typedef ChannelState ChannelState;

class CmdIssuer : public Component
{
    public:
        CmdIssuer ();			/*  constructor */
        ~ CmdIssuer ();
	Component* parent;
	DataBusHandler* dataBus;
	CmdBusHandler* cmdBus;	
	short Id;
	short bufferId;
	ChannelState prevState;
	Time busBusy;
        void process_event(IrisEvent* e);
	bool HasWork();
	std::string toString();

	bool IssueCmd(bool *isWrite, DRAMCmdState *scheduledCmd);
	bool BankNotBusy(DRAMCmdState currentCmd);
	bool BanksFirstReq(DRAMCmdState currentCmd, unsigned int index);
	bool BusNotBusy(DRAMCmdState currentCmd);
	bool CanSchedule(DRAMCmdState currentCmd, DRAMCmdState prevCmd);
	void SetBusBusyTime(DRAMCmdState currentCmd);
	Time CmdDelay(DRAMCmd cmd);
	Time CalculateDataDelay(DRAMCmdState cmdS);
	Time CalculateBurstL(DRAMCmdState prevCmd);
	Time CalculateBusyTime(DRAMCmd curCmd, DRAMCmd prevCmd, RankBankComb rankC, RankBankComb bankC, Time prevBurstL);
	void SetPrevState(DRAMCmdState currentCmd);
	Time Max(Time t1, Time t2);
        bool generated_start_event;
        DRAMCmd prevBusCmd;
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class CmdIssuer  ----- */

#endif   /*  ----- #ifndef cmd_issuer_INC  ----- */
