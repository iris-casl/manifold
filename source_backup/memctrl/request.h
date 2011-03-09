/*
 * =====================================================================================
 *
 *       Filename:  request.h
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

#ifndef  request_h_INC
#define  request_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include        <cstdlib>
#include        <map>
#include        "../kernel/component.h"
#include        "../kernel/simulator.h"
#include	"../util/mc_constants.h"

#ifdef USE_ZESTO
#include	"../zesto/host.h"
#include	"../zesto/machine.h"
#include	"../zesto/zesto-cache.h"
#endif

using namespace std;

enum CStatus {OPEN, CLOSED, CONFLICT, IDLE}; /*!< enumeration for the req status */

struct Data                             /*!< The data structure to store the data of the memory operation. Not used currently */
{
    unsigned long long int value;       /*!< \brief Value of the data */
    short size;	                        /*!< \brief Size of the data */
};
typedef Data Data;
/*  
 *  =====================================================================================
 *      Class:  Request
 *//*!    \brief The main data structure for each memory operation that flows in the memory controller. 
 *//*!       Description: This data structure flows through almost all the phases
 *      of the Mem Ctrl. A high level packet is converted to Request in NI. On
 *      return path, this request with corresponding stats updated will be
 *      converted back to high level packet which is sent to the network. All
 *      memory operations are converted to this data structure 
 *//* =====================================================================================
 */


class Request
{
    public:
        Request();             /*!< \brief constructor*/
        ~Request();            /*!< \brief deconstructor*/
        UInt mcNo;              /*!< \brief Id of MC which the request belongs to*/
        UInt channelNo;         
        UInt dimmNo;            // For future use
        UInt rankNo;            
        UInt bankNo;            
        UInt columnNo;          
        UInt rowNo;             
        UInt lowerBits;         /*!< \brief The lower bits of the address which constitutes address within a cache block. */
        cache_command cmdType;      /*!< \brief The type of cmd (read(load), write(store), writeback, prefetch) coming from the cache. */
        UInt threadId;          /*!< \brief Core Id from where the request came. */        
        Addr_t address;         /*!< \brief Requesting memory operation's address. */
        Data data;              /*!< Data field to store the actual value of the request. Not used currently. */
        bool mark;              /*!< \brief Signal to indicate that the request belongs to a batch or not. Used in PAR-BS. */
        bool local;             // For future use
        bool scheduledInMSHR;	/*< \brief Signal to indicate this request has been sent out to the network. Used by MSHR. */
        CStatus status;         /*!< \brief Signal to indicate the request is a hit or conflict. */      
        int tag;                /*!< \brief Unique tag of the request in Mem Ctrl. */
        bool serviced;          // For future use

        Request *child_req;     /*!< \brief Pointer of the child req in MSHR clubbed */
        Request *mother_req;    /*!< \brief Pointer of the mother req in MSHR clubbed */

        //////////////////////* Stats variables */////////////////

        Time startTime;                 /*!< Time at which it enters the MSHR */
        Time arrivalTime;	        /*!< Time at which it enters the Mem Ctrl */
        Time scheduleTime;	        /*!< Time at which it is sent out to the network */
        Time retireTime;	        /*!< Time at which it retires from the MC */
        Time busInsertionTime;          /*!< Time when it is inserted into the bus */
        Time rbufferInsertionTime       /*!< Time when it is inserted into request buffer */;
        Time cbufferInsertionTime;	/*!< Time when it is inserted into the cmd buffer */
        uint hop_count;                 /*!< Network hop count of this memory operation until now */
        double avg_network_latency;     /*!< Total memory latency of this memory operation until now */
        Time throttleTime;
    protected:

    private:

}; /* ----- end of class Request ------ */

#endif   /*  ----- #ifndef request_INC  ----- */
