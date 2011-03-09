/*
 * =====================================================================================
 *
 *       Filename:  routerVcMP.h
 *
 *    Description:  Router that can be used with meshes/multiple_vcs/DOR 
 *    1. Usess vcs purely for improving buffer and link utilization.
 *    2. Self contained: Example of how you can not use too many pseudo
 *    components
 *    3. WARNING: Does not check if the buffer is empty before sending a new
 *    pkt out. Uses the non blocking interface.
 *
 *        Version:  1.0
 *        Created:  03/11/2010 08:56:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _routerVcMP_h_INC
#define  _routerVcMP_h_INC

#include	"../interfaces/router.h"
#include	"genericBuffer.h"
#include	"genericRC.h"
//#include	"pvtopv_swa.h"
#include	"genericVcAllocator.h"
#include	"ptopSwaVcs.h"
#include	"genericCrossbar.h"
#include	"../../../util/genericData.h"
#include	"../../../util/stats.h"
#include	"genericLink.h"
#include	<sys/time.h>
#include	<algorithm>
#include	<list>

extern uint send_early_credit;
extern uint stat_print_level;
extern uint do_two_stage_router;
extern void print_state_at_deadlock(void);
extern IrisStats* istat;

class RouterVcMP: public Router
{
    public:
        RouterVcMP ();                             /*! constructor */
        ~RouterVcMP();                             /*! destructor */
        void init(uint ports, uint vcs, uint credits, uint buffer_size);
        /*! Initialise the router for ports, vcs and credits. TODO buffer size
         * not used right now. Just make sure its larger than credits.*/

        void set_no_nodes( uint nodes);                 /*! For DOR the no of nodes in the network 
                                                          and the grid sizes are to be set and 
                                                          passed to RC. */
        void set_grid_x_location ( uint a, uint b, uint c);
        void set_grid_y_location ( uint a, uint b, uint c);

        void send_credit_back( uint i);

        void process_event(IrisEvent* e);
        string toString() const;
        string print_stats();
        void set_edge_links();
        vector< vector<uint> > downstream_credits;
        void dump_buffer_state();
	double get_average_packet_latency();
	double get_last_flit_out_cycle();
        double get_flits_per_packet();
        double get_buffer_occupancy();
        double get_swa_fail_msg_ratio();
        double get_swa_load();
        double get_vca_fail_msg_ratio();
        double get_vca_load();
        double get_stat_packets();
        double get_stat_flits();
        double get_stat_ib_cycles();
        double get_stat_rc_cycles();
        double get_stat_vca_cycles();
        double get_stat_swa_cycles();
        double get_stat_st_cycles();
	

    protected:

    private:
        vector <GenericBuffer> in_buffers;
        vector <GenericRC> decoders;
        vector <InputBufferState> input_buffer_state;
        vector< list< uint> > vc_alloc;
        vector< vector< uint> > sw_alloc;
        vector< vector <uint> >request_op;
        vector <uint> available_ports;
        vector <uint> available_vcs;
        vector < vector<uint> > cr_time;

        bool ticking;
        void handle_link_arrival_event(IrisEvent* e);
        void handle_tick_event(IrisEvent* e);
        void handle_detect_deadlock_event(IrisEvent*);
        void do_switch_traversal();
        void do_switch_allocation();
        void do_input_buffering();
        void do_virtual_channel_allocation();
        void request_switch_allocation();

        /*! These are the statistics variables */
        double stat_buffer_occupancy;
        uint stat_packets;
        uint stat_flits;
        double stat_total_packet_latency;
        double last_flit_out_cycle;  /* indicates the last active cycle for the router */
        double stat_swa_fail_msg_ratio;
        double stat_swa_load;
        double stat_vca_fail_msg_ratio;
        double stat_vca_load;
        ullint stat_sim_total_time;
        vector< vector<uint> > stat_packet_out;
        vector< vector<uint> > stat_flit_out;
        bool is_mc_router;
        ullint stat_ib_cycles;
        ullint stat_rc_cycles;
        ullint stat_vca_cycles;
        ullint stat_swa_cycles;
        ullint stat_st_cycles;

}; /* -----  end of class RouterVcMP  ----- */

#endif   /* ----- #ifndef _routerVcMP_h_INC  ----- */

