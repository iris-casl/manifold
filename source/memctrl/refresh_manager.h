/*
 * =====================================================================================
 *
 *       Filename:  refresh_manager.h
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  02/18/2010 02:53:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  refresh_manager_h_INC
#define  refresh_manager_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../simIris/data_types/impl/irisEvent.h"
#include        "request.h"
#include	"request_handler.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  RefreshMgr
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */

class RefreshMgr : public Component
{
    public:
        RefreshMgr ();                             /*  constructor */
        ~ RefreshMgr ();
	Component* mc;
	Time nextRefresh;
	RequestHandler *reqPtr;
        void process_event(IrisEvent* e);
	unsigned int currentRankNo;
	unsigned int currentRowNo;

        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class RefreshMgr  ----- */

#endif   /*  ----- #ifndef refresh_manager_INC  ----- */

