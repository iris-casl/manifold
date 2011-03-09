/*!
 * =====================================================================================
 *
 *       Filename:  genericRouterPhy.h
 *
 *    Description:  This router is meant to work as a small low area low
 *    energy router. 
 *    Does not understand virtual channels and thus does not hava a vca stage.
 *    Head flit: IB/RC->SA->ST
 *    body/tail flit: IB->ST
 *    (Good example for understanding simulation methodology of components and
 *    pseudo-components within iris)
 *
 *    Generral notes on router modelling: Router stages in a generic input buffered physical router model
 *    (BW->RC)->SA->ST->LT
 *    Buffer write(BW)
 *    Route Computation (RC)
 *    Virtual Channel Allocation (VCA)
 *    Switch Allocation (SA)
 *    Switch Traversal (ST)
 *    Link Traversal (LT)
 *
 *    Model Description in cycles:
 *    ---------------------------
 *    BW and RC stages happen in the same cycle in most models( BW pushes the flits into the
 *    input buffer and the RC unit. )
 *    VCA: Allocate a virtual channel on the decoded output port(v virtual
 *    channels per port).
 *    SA: Pick one output port from n requesting input messages (0<n<pv) for the pxp crossbar 
 *    ST: Move the flits across the crossbar and push it out on the link
 *    LT: This is not modelled within the router and is part of the link component.
 *
 *    Pipeline stages for the HEAD and BODY/TAIL are not the same.
 *    HEAD: (IB/RC)->VCA->SA->ST--->LT
 *    BODY/TAIL: IB/RC->SA->ST--->LT
 *
 *    When the tail goes thru ST it frees all resources.
 *    Flit level flow control is used for most routers. The credit is sent
 *    back in the ST stage when it is known for sure that the buffer will
 *    empty.
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

#ifndef  _genericRouterPhy_h_INC
#define  _genericRouterPhy_h_INC

#include	"../interfaces/router.h"
#include	"genericBuffer.h"
#include	"genericRC.h"
#include	"ptop_swa.h"
#include	"genericCrossbar.h"
#include	"../../../util/genericData.h"
#include	"../../../util/stats.h"
#include	"genericLink.h"
#include	<sys/time.h>

extern uint send_early_credit;
extern uint do_two_stage_router;
extern uint stat_print_level;
extern IrisStats* istat;

class GenericRouterPhy: public Router
{
    public:
        GenericRouterPhy ();                             /* constructor */
        ~GenericRouterPhy();
        void init(uint ports, uint vcs, uint credits, uint buffer_size);

        void set_no_nodes( uint nodes);
        void set_grid_x_location ( uint a, uint b, uint c);
        void set_grid_y_location ( uint a, uint b, uint c);

        void send_credit_back( uint i);

        void process_event(IrisEvent* e);
        string toString() const;
        string print_stats();
        vector< vector<uint> > downstream_credits;

        /* These are the statistics variables */
        uint packets;
        uint flits;
        double total_packet_latency;
        double last_flit_out_cycle;
        unsigned long long int stat_sim_total_time;
        vector< vector<uint> > stat_packet_out;
        vector< vector<uint> > stat_flit_out;
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
        PToPSwitchArbiter swa;
        GenericCrossbar xbar;

        bool ticking;
        void handle_link_arrival_event(IrisEvent* e);
        void handle_tick_event(IrisEvent* e);
        void do_switch_traversal();
        void do_switch_allocation();
        void do_input_buffering();

}; /* -----  end of class GenericRouterPhy  ----- */

#endif   /* ----- #ifndef _genericRouterPhy_h_INC  ----- */

