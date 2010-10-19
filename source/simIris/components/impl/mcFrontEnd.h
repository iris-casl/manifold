/*
 * =====================================================================================
 *
 *       Filename:  mcFrontEnd.h
 *
 *    Description: This is the interface to connect to the memory controller
 *    module. 
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

#ifndef  _mcFrontEnd_h_INC
#define  _mcFrontEnd_h_INC

#include        "../interfaces/processor.h"
#include        "../../data_types/impl/highLevelPacket.h"
#include	"../../data_types/impl/irisEvent.h"
#include	"../../../util/genericData.h"
#include	"../../../util/mc_constants.h"
#include	"../../../memctrl/request.h"
#include        "../../../memctrl/mshr.h"
#include	<fstream>
#include	<algorithm>

#define DEFAULT_RAN_MAX_TIME 100
extern uint MC_ADDR_BITS;
extern uint no_mcs;
extern vector <uint> mc_positions;

using namespace std;

class McFrontEnd : public Processor
{
    public:
        McFrontEnd ();                             /* constructor */
        ~McFrontEnd ();
        Component* mc;
	vector<Request> niQueue;
        void setup(uint no_nodes, uint vcs, uint max_sim_time);
        void finish();
        void process_event(IrisEvent* e);
        string toString() const;
        uint node_ip;
        void set_no_vcs ( uint v );
        void set_output_path( string v );
        bool compare();
	string print_stats() const;
        bool sending;

    protected:

    private:
        double last_pkt_out_cycle;
        ullint last_out_pull_cycle;
        unsigned long long int total_missed_time;
        unsigned long long int total_backward_time;
        uint missed_time;
        unsigned long long int packets_out;
        ullint resp_buff_occ_cycles;
        ullint avg_resp_buff_occ;

        uint vcs;
        uint no_nodes;
	unsigned int packets;
        unsigned long long int max_sim_time;
        deque< HighLevelPacket > out_packets;
        deque< HighLevelPacket > sent_packets;
        vector <HighLevelPacket*> outstanding_hlp;
        string out_filename;
        string trace_name;
        ofstream out_file;
        fstream trace_filename;
        vector< bool > ready;
        unsigned int last_vc;
        unsigned int flast_vc;
        void handle_new_packet_event( IrisEvent* e);
	void handle_old_packet_event( IrisEvent* e);
        void handle_ready_event(IrisEvent* e);
        void handle_out_pull_event(IrisEvent* e);
        void handle_detect_deadlock_event(IrisEvent*);
	void convertToBitStream(Request* req, HighLevelPacket *hlp);
	void convertFromBitStream(Request* req, HighLevelPacket *hlp);
	bool GetFrommcFrontEndQueue(Request* req);
	void add_mc_bits(Request *req);
	void strip_mc_bits(Request *req);
}; /* -----  end of class mcFrontEnd  ----- */

#endif   /* ----- #ifndef _mcFrontEnd_h_INC  ----- */
