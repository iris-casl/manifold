
#ifndef  _genericrpg_h_INC
#define  _genericrpg_h_INC

#include "../../randomNumbers/impl/libRandom.hpp"

#include "../interfaces/processor.h"
#include "genericInterface.h"
#include "genericData.h"
#include <fstream>
#include <deque>

#ifndef IN_OUT_MISMATCH
#define IN_OUT_MISMATCH -1
#endif

#ifndef SIM_SUCCESS 
#define SIM_SUCCESS 0
#endif

#ifdef REPORT_BASE
#undef REPORT_BASE
#endif

#define REPORT_BASE 20

#define DEFAULT_RAN_LAMDA 2.35
#define DEFAULT_RAN_DESTINATION_TYPE "uniform"
#define DEFAULT_RAN_LENGTH_TYPE "uniform"
#define DEFAULT_RAN_ADDRESS 0
#define MAX_ADDRESS 3
#define MAX_LENGTH 10
#define MIN_LENGTH 1
#define MIN_DELAY 1
#define MAX_DELAY 100
#define DEFAULT_RAN_MAX_VC 1
#define DEFAULT_RAN_SEED 324986
#define DEFAULT_RAN_MAX_TIME 100
#define HOT_SPOTS 3
#define DEFAULT_RAN_TRACE_FILE_NAME "randomOut.tr"
const string run_destination_type = "uniform";
#define MAX(a,b) (((a)<(b))?(b):(a))
#define MIN(a,b) (((a)<(b))?(a):(b))

class GenericRPG : public Processor
{

    private:
        deque< HighLevelPacket > out_packets;
        deque< HighLevelPacket > sent_packets;
        string out_filename;
        uint vcs;
        ofstream out_file;
        vector< bool > ready;
        unsigned int last_vc;
        bool only_sink;
        bool sending;
        void handle_new_packet_event(IrisEvent* e);
        void handle_ready_event(IrisEvent* e);
        void handle_out_pull_event(IrisEvent* e);
        libRandom::randomNumberGenerator generator;

    public :
        GenericRPG();
        ~GenericRPG();
        /* stats variables */
        unsigned int packets;
        uint max_sim_time;
        void init_generator();
        double lamda;
        libRandom::randomNumberGenerator::distribution destination_type;
        libRandom::randomNumberGenerator::distribution length_type;
        libRandom::randomNumberGenerator::distribution delay_type;
        unsigned int address;
        unsigned int max_address;
        unsigned int min_length;
        unsigned int max_length;
        unsigned int min_delay;
        unsigned int max_delay;
        unsigned int hot_spots;
        unsigned long long int max_time;
        unsigned int seed;
        void setup(uint no_nodes, uint vcs, uint max_sim_time);
        void finish();
        void pre_tick();
        void post_tick();
        void idle();		
        void process_event(IrisEvent* e);
        void set_no_vcs( uint v );
        string toString() const;
        bool compare();
        set< HighLevelPacket > get_all_sent();
        set< HighLevelPacket > get_all_recv();
        void init();	
};



#endif   /* ----- #ifndef _genericrpg_h_INC  ----- */

