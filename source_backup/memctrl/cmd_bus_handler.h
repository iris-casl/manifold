/*
 * =====================================================================================
 *
 *       Filename:  cmd_bus_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/23/2010 07:06:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  cmd_bus_handler_h_INC
#define  cmd_bus_handler_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../simIris/data_types/impl/irisEvent.h"
#include        "request.h"
#include	"dram.h" 

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  CmdBusHandler
 *//*!    \brief  The cmd bus class. 
 *//*!      Description: This class models an address and command bus. The cmd
 *      issuer sends a cmd containing address to this. it takes that command
 *      and delivers it to the DRAM after t_CMD cycles. 
 *//*=====================================================================================
 */

class CmdBusHandler : public Component
{
    public:
        CmdBusHandler ();                      /*!< \brief  constructor */
        ~ CmdBusHandler ();                    /*!< \brief  deconstructor */ 
	Component* parent;                     /*!< \brief  pointer to its parent */ 
	Component* child;                      /*!< \brief  pointer to the corresponding DRAM channel */ 
        void process_event (IrisEvent* e);     /*!< \brief  main function that handles the operation of cmd bus */ 
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class CmdBusHandler  ----- */

#endif   /*  ----- #ifndef cmd_bus_handler_INC  ----- */
