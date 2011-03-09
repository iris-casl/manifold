/*
 * =====================================================================================
 *
 *       Filename:  response_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/04/2010 02:06:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  response_handler_h_INC
#define  response_handler_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../simIris/data_types/impl/irisEvent.h"
#include        "request.h"
#include	"../util/mc_constants.h" 
#include        "../simIris/components/impl/mcFrontEnd.h"
#include 	"cmd_issuer.h"
#include        "mshr_standalone.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  ResponseHandler
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */
extern vector<MSHR_SA_H*> mshrHandler;
class ResponseHandler : public Component
{
    public:
        ResponseHandler ();                             /*  constructor */
        ~ ResponseHandler ();
	Component* mc;
	Component* parent;
	Component* child;
	vector<Request> responseBuffer;
	bool bufferFull;
        vector <bool> serviced;
	bool IsBufferFull();
	Component* reqPtr;
	bool stoppedQueue;
	bool CanStart();
	unsigned int SearchBuffer(DRAMCmdState *cmd);
        unsigned int SendServiced();
        void process_event (IrisEvent* e);
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
        bool out_pull_scheduled;
                                                                            
}; /*  -----  end of class ResponseHandler  ----- */

#endif   /*  ----- #ifndef response_handler_INC  ----- */
