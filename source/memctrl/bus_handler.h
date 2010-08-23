/*
 * =====================================================================================
 *
 *       Filename:  bus_handler.h
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  02/18/2010 11:48:21 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  bus_handler_h_INC
#define  bus_handler_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../simIris/data_types/impl/irisEvent.h"
#include        "request.h" 
#include	"cmd_issuer.h"
#include	"../util/mc_constants.h"

using namespace std;

//typedef vector<Request> Buffer;
/*  
 *  =====================================================================================
 *      Class:  BusHandler
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

class BusHandler
{
    public:
        BusHandler ();                             /*  constructor */
        ~ BusHandler ();
	Component* parent;
//	short busId;
 	CmdQueue cmdQueue[NO_OF_CHANNELS];	
	CmdIssuer cmdIssuer[NO_OF_CHANNELS];
	bool stopSignal;
	bool full[NO_OF_CHANNELS];
	bool linkBusy[NO_OF_CHANNELS];
	vector< vector< vector<bool> > > oneReq;
	bool BankRankFree(unsigned int bankNo, unsigned int rankNo, unsigned int channelNo);
	void SetIfFull();
	bool IsFull(int i);
	void LowLevelCmdGen(Request *req);
//	bool stoppedUpper;
//	bool stoppedLower;		
//      void process_event(IrisEvent* e);
//	bool HasWork();
//	bool StopUpper();
//	bool CanUpperStart();
//	void SetEvent(IrisEvent *event, Component* src, Component* dst, unsigned int type);
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class BusHandler  ----- */

#endif   /*  ----- #ifndef bus_handler_INC  ----- */
