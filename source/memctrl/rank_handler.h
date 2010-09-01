/*
 * =====================================================================================
 *
 *       Filename:  rank_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 05:30:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  rank_handler_h_INC
#define  rank_handler_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include        "request.h"
#include	"bank_handler.h"
#include	"../util/mc_constants.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  RankHandler
 *//*! \brief      Handles of the operations of a rank.
 *//*!      Description: This class contains data structures related to per rank
 *      in the Mem Ctrl. Acts as a wrapper around all the bank handlers. Also
 *      contains some rank related state information.     
 *//*=====================================================================================
 */

typedef vector<Request> ReqBuffer;

class RankHandler
{
    public:
        RankHandler ();                     /*!<  \brief constructor */
        ~ RankHandler ();                   /*!<  \brief destructor */
        short rankId;                       /*!<  \brief Unique Id of the rank in that channel */
	vector<ReqBuffer> rbuffer;          /*!<  \brief Request buffers of the whole rank. */
                                            /*!<  Each bank will access its own buffer with bufferId */
        vector<BankHandler> bank;           /*!<  \brief Array of the handlers to the bank that constitutes this rank */
	unsigned int readsOWrite;           /*!<  \brief No of reads that have bypassed the first write in the rank */
	unsigned int prevReadsOWrite;       /*!<  Temporary for state restoration */                               
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class RankHandler  ----- */

#endif   /*  ----- #ifndef rank_handler_INC  ----- */
