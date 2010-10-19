/*
 * =====================================================================================
 *
 *       Filename:  MC.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/07/2010 07:15:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  MC_h_INC
#define  MC_h_INC

#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../simIris/data_types/impl/irisEvent.h"
#include        "request.h"
#include	"request_handler.h" 
#include        "bus_handler.h"
#include        "bus.h"
#include        "dram.h"
#include        "../simIris/components/impl/mcFrontEnd.h"
#include	"refresh_manager.h"
#include	"response_handler.h"
#include	"stats.h"

class MC
{
    public:
        McFrontEnd *ni;
	RequestHandler *reqH;
	RefreshMgr *refMgr;
	Bus *bus;
	DRAM *dram;
	ResponseHandler *responseH;
	Statistic *stats;
	Component *parent;
//	bool * doneOnce[NO_OF_THREADS];
	void Init();
	void StartRefresh();
        UInt id;
	MC ()
    	{
	    stats = new Statistic();
	    reqH = new RequestHandler();
	    refMgr = new RefreshMgr();
	    bus = new Bus();
	    dram = new DRAM();
	    responseH = new ResponseHandler();		
	};
    	~MC () 
	{
	    delete reqH;
	  //  delete refMgr;
	    delete bus;
	    delete responseH;
	    delete dram;
	//    delete ni;			
	};
        void sim_main(IrisEvent* e);
};/*  -----  end of class MC  ----- */

#endif   /*  ----- #ifndef MC_INC  ----- */
