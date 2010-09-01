/*
 * =====================================================================================
 *
 *       Filename:  bus.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/05/2010 05:36:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  bus_h_INC
#define  bus_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../simIris/data_types/impl/irisEvent.h"
#include        "request.h"
#include	"data_bus_handler.h"
#include	"cmd_bus_handler.h"
#include	"dram.h" 

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  Bus
 *//*!      \brief Class to model cmd, address and data buses of a DRAM.
 *//*!       Description: This class just acts a wrapper around the data and cmd
 *      buses of all the channels. It contains the array of both cmd and data bus handlers. 
 *      It is not used during normal MC & DRAM operations but only used to set
 *      up some links between the busses and other components in the beginning
 *//*=====================================================================================
 */

class Bus : public Component
{
    public:
        Bus();                                  /*!< \brief  constructor */
        ~ Bus();                                /*!< \brief  deconstructor */
	Component* mc;                          /*!< \brief  pointer to the Mem Ctrl that contains it */
	Component* parent;                      /*!< \brief  pointer to its parent */
	Component* child1;      		/*!< \brief  DRAM side link */
	Component* child2;	        	/*!< \brief  Response side link */
	vector<DataBusHandler> dataBus;         /*!< \brief  Array of data buses (1 per channel) */
	vector<CmdBusHandler> cmdBus;           /*!< \brief  Array of cmd buses (1 per channel) */
        void process_event (IrisEvent* e);      
	void SetLinks();                        /*!< \brief  Setd the links of buses with various components */        
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class Bus  ----- */

#endif   /*  ----- #ifndef bus_INC  ----- */
