/*
 * =====================================================================================
 *
 *       Filename:  packetSource.h
 *
 *    Description:  This is a test packet to send packets to a interface on
 *    VC0 at some random time and random packet length. 
 *
 *        Version:  1.0
 *        Created:  03/11/2010 05:00:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _traceHandler_h_INC
#define  _traceHandler_h_INC

#include        "MersenneTwister.h"
#include        "../components/interfaces/processor.h"
#include        "../components/impl/genericEvents.h"
#include        "../data_types/impl/highLevelPacket.h"
#include	"../data_types/impl/irisEvent.h"
#include	"../components/impl/genericData.h"
#include	"../MemCtrl/constants.h"
#include	"../MemCtrl/request.h"		
#include	<fstream>

#define DEFAULT_RAN_MAX_TIME 100
//#define PURE_SINK 1

using namespace std;

/*
 * =====================================================================================
 *        Class:  TraceHandler
 *  Description:  
 * =====================================================================================
 */
class TraceHandler : public Processor
{
    public:
        TraceHandler (char* traceName);                             /* constructor */
        ~TraceHandler ();
        uint address;
        uint min_address;
        uint max_address;
        uint min_length;
        uint max_length;
        uint min_delay;
        uint max_delay;
        uint hotspots;
        uint max_time;
        uint seed;
        void setup();
        void finish();
        void process_event(IrisEvent* e);
        string toString() const;
        uint node_ip;

    protected:

    private:
	ifstream trace_filename;
        string in_filename;
        string out_filename;
        ofstream out_file;
        ofstream in_file;
        vector<bool> ready;
	vector<Request> nextRequest;
	void InitNextRequest();
        uint last_vc;
        uint no_of_packets;
        bool sending;
        void handle_new_packet_event( IrisEvent* e);
        void handle_ready_event(IrisEvent* e);
        void handle_out_pull_event(IrisEvent* e);
	void convertToBitStream(Request* req, HighLevelPacket *hlp);
	void convertFromBitStream(Request* req, HighLevelPacket *hlp);
	bool GetNextRequest(Request* req);

}; /* -----  end of class TraceHandler  ----- */

#endif   /* ----- #ifndef _traceHandler_h_INC  ----- */
