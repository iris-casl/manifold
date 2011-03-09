/*
 * =====================================================================================
 *
 *       Filename:  mshr.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/12/2010 12:43:25 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  mshr_sa_h_INC
#define  mshr_sa_h_INC

#include        <string>
#include        <sstream>
#include        <iostream>
#include 	<iomanip>
#include        <cstdlib>
#include 	<fstream>
#include        <map>
#include        "../kernel/simulator.h"
#include        "../simIris/data_types/impl/irisEvent.h"
#include        "request.h"

using namespace std;

/*  
 *  =====================================================================================
 *      Class:  MSHR_SA_H
 *      Description:  
 *     
 *     
 *   =====================================================================================
 */
extern ullint max_sim_time;
extern uint THREAD_BITS_POSITION;
class MSHR_SA_H : public Component
{
    public:
        MSHR_SA_H ();                             /*  constructor */
        ~ MSHR_SA_H ();
	Time unsink;
	Time lastFinishTime;
	Time globalUnSink;
	bool done;
	vector <Request> mshr;
	vector <Request> writeQueue;
	ifstream trace_filename;
	char * filename;
	unsigned int id; 
	unsigned int lastScheduledIndex;
	Component* parent;
	Component* child;
	Addr_t GlobalAddrMap(Addr_t addr, uint threadId);
        void process_event (IrisEvent* e);
	void DeleteInMSHR(Request* req);	
        void demap_addr(Addr_t oldAddress, Addr_t newAddress);
	bool waiting;
	Request waitingForMSHR;
	Request nextReq;
	Time lastFullTime;
        std::string toString();
                                                                            
    protected:
                                                                            
    private:
                                                                            
}; /*  -----  end of class MSHR_SA_H  ----- */

#endif   /*  ----- #ifndef mshr_INC  ----- */
