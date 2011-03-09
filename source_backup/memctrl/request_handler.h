/*
 * =====================================================================================
 *
 *       Filename:  request_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/17/2010 01:07:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  request_handler_h_INC
#define  request_handler_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../simIris/data_types/impl/irisEvent.h"
#include        "../simIris/components/impl/mcFrontEnd.h"
#include        "request.h" 
#include	"channel_handler.h"
#include	"bus_handler.h"
#include	"bank_handler.h"
#include 	"addr_map.h"
#include	"../util/mc_constants.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  RequestHandler
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */ 

class RequestHandler : public Component
{
    public:
        RequestHandler ();                             /*  constructor */
        ~ RequestHandler ();
	Component* mc;
        Component* parent;
	Component* child;
        ChannelHandler *chan;
	BusHandler *busHandler;
	AddrMap	*addrMap;
	Component* resPtr;		// TODO Pointer to response handler needs to be changed via manifold link
	Time lastBatchFormTime;
	Request pipeline;
	int reqTag;
	bool oneBufferFull;
	bool pipelineFilled;
	void SetLinks();
	void FormBatch();
	void MarkAll();
	void MarkBatchOnly();
	void PushPipeline(Request *req);
        void process_event(IrisEvent* e);
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class RequestHandler  ----- */

#endif   /*  ----- #ifndef request_handler_INC  ----- */

