
#ifndef  _generictpg_h_INC
#define  _generictpg_h_INC

#include        "genericInterface.h"
#include        "genericEvents.h"
#include        "genericData.h"
#include        "../interfaces/processor.h"
#include        "../../data_types/impl/highLevelPacket.h"
#include	"../../MemCtrl/request.h"
#include        <fstream>
#include        <deque>
#include        "../../MemCtrl/mshr.h"
#include	"../../MemCtrl/constants.h"
#include        <math.h>

#define DEFAULT_RAN_MAX_TIME 100
#define MAX_ADDRESS 3
#define MAX(a,b) (((a)<(b))?(b):(a))
#define MIN(a,b) (((a)<(b))?(a):(b))

extern uint no_nodes;
extern uint no_mcs;

class GenericTPG : public Processor
{

    private:
        uint vcs;
        uint no_nodes;
        uint no_outstanding;
        unsigned long long int max_sim_time;
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
        GenericTPG();
        ~GenericTPG();
        /* stats variables */
        unsigned int packets;
        unsigned int packets_in;
        double min_pkt_latency;
        double last_packet_out_cycle;
        unsigned long long int fwd_path_delay;
		unsigned long long int roundTripLat;
        unsigned long long int stat_round_trip_network_latency;
        unsigned long long int stat_round_trip_hop_count;
        unsigned long long int stat_round_trip_memory_latency;
        unsigned long long int stat_waiting_in_ni;

        MSHR_H *mshrHandler;
        vector< bool > ready;
        bool sending;
        unsigned long long int max_time;
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
        unsigned int bankInfo[8][NO_OF_BUFFERS];
};



#endif   /* ----- #ifndef _generictpg_h_INC  ----- */

