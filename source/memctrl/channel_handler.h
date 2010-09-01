/*
 * =====================================================================================
 *
 *       Filename:  channel_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 05:30:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  channel_handler_h_INC
#define  channel_handler_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "request.h"
#include	"rank_handler.h" 
#include 	"../util/mc_constants.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  ChannelHandler
 *//*!      \brief Handles all the operations of a channel in the MC.
 *//*!       Description: This class contains data structures related to per
 *      channel in the memory controller. Right now just acts as a wrapper
 *      around all rank handlers of that channel     
 *//* =====================================================================================
 */

class ChannelHandler
{
    public:
        ChannelHandler ();                     /*!< \brief constructor */
        ~ ChannelHandler ();                   /*!< \brief destructor  */ 
	short chanId;                          /*!< \brief unique Id of the channel per MC */ 
	vector<RankHandler> rank;         /*!< \brief Array of handlers to the rank constituting the channel */
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class ChannelHandler  ----- */

#endif   /*  ----- #ifndef channel_handler_INC  ----- */
