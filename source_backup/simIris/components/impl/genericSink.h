
#ifndef  _genericsink_h_INC
#define  _genericsink_h_INC
#include	"../interfaces/processor.h"
#include	"../../data_types/impl/highLevelPacket.h"
#include        "../../../util/genericData.h"
#include        "genericInterfaceNB.h"
#include	<deque>
#include	<fstream>

using namespace std;

/*
 * =====================================================================================
 *        Class:  GenericSink
 *  Description:  
 * =====================================================================================
 */
class GenericSink : public Processor
{
    private:
        uint vcs;
        uint no_nodes;
        ullint max_sim_time;
        string out_filename;
        uint last_vc;
        /* stats variables */
        unsigned int stat_packets_in;
        double stat_min_pkt_latency;
        ullint stat_last_packet_in_cycle;
        ullint stat_last_packet_out_cycle;
        ullint stat_total_lat;
        ullint stat_hop_count;
        ullint stat_packets_out;
        
        //Event handlers
        void handle_new_packet_event( IrisEvent* e);
        void handle_ready_event( IrisEvent* e);
        void handle_out_pull_event( IrisEvent* e);
    public:
        GenericSink ();                             /* constructor */
        void setup(uint v, uint time, uint no_nodes);
        void process_event(IrisEvent* e);
        string toString() const;
        string print_stats() const;
        ofstream out_file;
        uint address;
        vector<bool> ready;
        unsigned int packets;
        bool sending;
        void set_output_path( string outpath );

    protected:


}; /* -----  end of class GenericSink  ----- */


#endif   /* ----- #ifndef -genericsink_h_INC  ----- */
