
#ifndef  _genericPktGen_h_INC
#define  _genericPktGen_h_INC

#include        "genericInterface.h"
#include        "genericEvents.h"
#include        "../../../util/genericData.h"
#include        "../interfaces/processor.h"
#include        "../../data_types/impl/highLevelPacket.h"
#include	"../../../memctrl/request.h"
#include        "../../../memctrl/mshr.h"
#include	"../../../util/mc_constants.h"
#include        <math.h>
#include        <fstream>
#include        <deque>
#include        <string.h>

#define DEFAULT_RAN_MAX_TIME 100
#define MAX_ADDRESS 3

extern uint no_nodes;
extern uint no_mcs;

class GenericPktGen : public Processor
{

    private:
        uint vcs;
        uint no_nodes;
        uint no_outstanding;
        ullint max_sim_time;
        string out_filename;
        string trace_name;
        ofstream out_file;
        ifstream *trace_filename;
        unsigned int last_vc;
        void handle_new_packet_event(IrisEvent* e);
        void handle_ready_event(IrisEvent* e);
        void handle_out_pull_event(IrisEvent* e);

    public :
        GenericPktGen();
        ~GenericPktGen();
        /* stats variables */
        unsigned int stat_packets_out;
        unsigned int stat_packets_in;
        double stat_min_pkt_latency;
        double stat_last_packet_out_cycle;
        ullint stat_total_lat;
        ullint stat_hop_count;

        vector< bool > ready;
        bool sending;
        ullint max_time;
        void setup(uint no_nodes, uint vcs, uint max_sim_time);
        void finish();
        void process_event(IrisEvent* e);
        string toString() const;
        string print_stats() const;
        void set_trace_filename( string filename );
        void set_no_vcs ( uint v );
        void set_output_path( string outpath );
        vector <uint> mc_node_ip;

        /* for debugging.. break if not sending for too long */
        ullint lastSentTime;
};



#endif   /* ----- #ifndef _genericPktGen_h_INC  ----- */

