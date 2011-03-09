/*
 * =====================================================================================
 *
 *       Filename:  data_bus_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/23/2010 07:05:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  data_bus_handler_h_INC
#define  data_bus_handler_h_INC

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
#include 	"../util/mc_constants.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  DataBusHandler
 *//*!      \brief The data bus class
 *//*!      Description: This class handles the operation of the data bus. It
           receives read and write data from the corresponding dram channel or cmd issuer,
 *         respectively. It sends them to the response handler or DRAM after
 *         burst delay cycles. It also calculates data bus related stats.     
 *//*=====================================================================================
 */

class DataBusHandler : public Component
{
    public:
        DataBusHandler ();              /*!< \brief constructor */
        ~ DataBusHandler ();            /*!< \brief deconstructor */
	Component* parent;              /*!< \brief pointer to its parent */
	Component* child1;		/*!< \brief pointer to corresponding DRAM channel */ 
	Component* child2; 		/*!< \brief pointer to response handler */
        void process_event (IrisEvent* e); /*!< \brief Handles all the operations of the data bus */
        std::string toString();    
        //////////* Stats variables *////////////
	Time prevTime;                  /*!< \brief variable to keep track of the last time data bus was used */
        Time busBusyTime;               /*!< \brief total time the bus remained busy */
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class DataBusHandler  ----- */

#endif   /*  ----- #ifndef data_bus_handler_INC  ----- */
