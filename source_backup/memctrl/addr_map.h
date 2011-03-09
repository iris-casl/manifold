/*
 * =====================================================================================
 *
 *       Filename:  addr_map.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 02:13:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  addr_map_h_INC
#define  addr_map_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "../simIris/data_types/impl/irisEvent.h"
#include        "request.h" 
#include	"bank_handler.h"
#include	"../util/mc_constants.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  AddrMap
 *//*!       Description: \brief Class for handling adress translations within the
 *      Memory Controller
 *   =====================================================================================
 */

class AddrMap : public Component
{
    public:
        AddrMap ();                             /*!< \brief  constructor */
        ~ AddrMap ();                           /*!< \brief  deconstructor */
	Component* parent;                      /*!< \brief  pointer to the request handler */
        void process_event (IrisEvent* e);      /*!< \brief  Main function that handles this class*/
	void map_addr(Request *req); /*!< \brief Main address translation function */
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class AddrMap  ----- */

#endif   /*  ----- #ifndef addr_map_INC  ----- */
