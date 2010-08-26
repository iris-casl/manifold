
#ifndef  _GenericTPGVcs_h_INC
#define  _GenericTPGVcs_h_INC

#include        "genericInterface.h"
#include        "genericRouter4Stg.h"
#include        "genericEvents.h"
#include        "../../../util/genericData.h"
#include        "../interfaces/processor.h"
#include        "../../data_types/impl/highLevelPacket.h"
#include	"../../../memctrl/request.h"
#include        "../../../memctrl/mshr.h"
#include	"../../../util/mc_constants.h"
#include        "../../../frontend/impl/mesh.h"
#include        "../../../zesto/zesto-cache.h"
#include        <math.h>
#include        <fstream>
#include        <deque>
#include        <string.h>

#define DEFAULT_RAN_MAX_TIME 100
#define MAX_ADDRESS 3

extern uint no_nodes;
extern uint no_mcs;

class GenericTPGVcs : public Processor
{

    private:
        uint vcs;
        uint no_nodes;
        uint no_outstanding;
        ullint max_sim_time;
        deque< HighLevelPacket > out_packets;
        deque< HighLevelPacket > sent_packets;
        string out_filename;
        string trace_name;
        ofstream out_file;
        ifstream *trace_filename;
        unsigned int last_vc;
        Request* GetNextRequest();
        bool GetNewRequest(Request *req);
        Request* GetRequest();
        void convertToBitStream(Request* req, HighLevelPacket* hlp);
		void convertFromBitStream(Request* req, HighLevelPacket *hlp);
        void handle_new_packet_event(IrisEvent* e);
        void handle_ready_event(IrisEvent* e);
        void handle_out_pull_event(IrisEvent* e);

    public :
        GenericTPGVcs();
        ~GenericTPGVcs();
        /* stats variables */
        unsigned int packets;
        unsigned int packets_in;
        double min_pkt_latency;
        double last_packet_out_cycle;
        ullint fwd_path_delay;
		ullint roundTripLat;
		ullint TotalBLP;
        ullint stat_round_trip_network_latency;
        ullint stat_round_trip_hop_count;
        ullint stat_round_trip_memory_latency;
        ullint stat_waiting_in_ni;

        MSHR_H *mshrHandler;
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
        bool compare();
        vector <uint> mc_node_ip;
        ullint lastSentTime;
};



#endif   /* ----- #ifndef _GenericTPGVcs_h_INC  ----- */

