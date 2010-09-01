/*
 * =====================================================================================
 *
 *       Filename:  bank_handler.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2010 06:38:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  bank_handler_h_INC
#define  bank_handler_h_INC

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
#include	"addr_map.h"
#include	"bus_handler.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  Bank_Handler
 *//*!  \brief Class for implementing any per bank related policy. 
 *//*!      Description: This class handles all the operations related to a bank in the MC. 
 *    Currently it contains 3 per bank scheduling algorithms (FR-FCFS, FCFS & PAR-BS).
 *    It also maintains all bank related state information required in the memory controller. 
 *//*=====================================================================================
 */

enum CallerType {READS_FIRST, HIT_FIRST, OLDEST_FIRST, UNMARKED_FIRST, HIGHEST_RANKED_FIRST};

class BankHandler : public Component
{
    public:
        BankHandler ();                             /*!< \brief constructor */
        ~ BankHandler ();                           /*!< \brief deconstructor */     
        UInt bankId;                                /*!< \brief Id of the bank in current rank */    
	UInt bufferId;                              /*!< \brief Id of the buffer to look for */
        void* myRank;                               /*!< \brief pointer to its rank handler */    
        Component* parent;                          /*!< \brief pointer to its request handler */    


	bool bufferFull;                            /*!< \brief signal to indicate that the buffer is full */     
        void process_event(IrisEvent* e);           /*!< \brief Main process function */
	bool HasWork();
        


        bool MainScheduler(Request* req, int* index);   /*!< \brief The wrapper for all scheduling algorithms in this class */
        bool PARBS(Request* req, int* index);           /*!< \brief Function to implement PAR-BS scheduling policy */
        bool FRFCFS(Request* req, int* index);          /*!< \brief Function to implement FR-FCFS scheduling policy */ 
        bool FCFS(Request* req, int* index);            /*!< \brief Function to implement FCFS scheduling policy */
	bool OldestFirst(Request* req, int* index);     /*!< \brief Schedule oldest request */  
	bool RowHitFirst(Request* req, int* index);     /*!< \brief Schedule row hits */
	bool ReadsFirst(Request* req, int* index);      /*!< \brief Schedule read request */ 
	bool HighestRankedFirst(Request* req, UInt highest, int* index);  /*!< \brief Schedule highest ranked request */
	bool ScheduleUnmarked(Request* req, int* index);  /*!< \brief Schedule unmarked request */
	void FindRank(int priority[]);                  /*!< \brief Set the rank of each request and return it in priority[] */
	UInt FindHighest();                             /*!< \brief Find the highest ranked request */
	bool IsBufferFull();                            /*!< \brief Check whether the buffer is full or not */
	void SetReadsOWrite(CallerType caller, cache_command cmdType); /*!< \brief Set the Reads over write counter in the current rank */
	void SetBypasses(unsigned int index);           /*!< \brief Set the bypass counter in the current bank*/
	void SetPrevState();                            /*!< \brief Set the previous state  */
	void RestorePrevState();                        /*!< \brief Restore the previous state */

	UInt myChannel;                 /*!< \brief Id of the channel */
	UInt rowBufferIndex;            /*!< \brief Index of current row in the row buffer */
	UInt prevBufferIndex;           /*!< \brief Index of previous row in the row buffer (A temporary to restore previous state)*/
	bool rowOpen;                   /*!< \brief Signal to indicate a row is open or not */
	unsigned int bypassReq;         /*!< \brief Counter of requests that have bypassed the first one */
        bool prevRowOpen;               /*!< \brief A temporary for row open to restore previous state */ 
    	unsigned int prevBypassReq;     /*!< \brief Temporary of bypass counter */
	unsigned int bankTag;	        /*!< \brief Counter to indicate the current tag */        

        std::string toString();                 

        ////////////  Some Counter to collect stats /////////////////
	unsigned long long int bufferFullCounter;   
        unsigned long long int bufferOccupancy;
        unsigned long long int bufferSize;
                                                                            
    protected:
                                                                            
    private:
       bool generated;
		uint address;           
                                                                            
}; /*  -----  end of class BankHandler  ----- */

#endif   /*  ----- #ifndef bank_handler_INC  ----- */

