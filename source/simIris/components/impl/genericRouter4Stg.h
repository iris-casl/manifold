/*
 * =====================================================================================
 *
 *       Filename:  genericRouterVct.h
 *
 *    Description:  
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

#ifndef  _genericRouter4Stg_h_INC
#define  _genericRouter4Stg_h_INC

#include	"../interfaces/router.h"
#include	"genericBuffer.h"
#include	"genericRC.h"
//#include	"pvtopv_swa.h"
#include	"genericVcAllocator.h"
#include	"ptopSwaVcs.h"
#include	"genericCrossbar.h"
#include	"genericEvents.h"
#include	"../../../util/genericData.h"
#include	"genericLink.h"
#include	<sys/time.h>
#include	<algorithm>

extern uint send_early_credit;
extern uint do_two_stage_router;
extern void print_state_at_deadlock(void);

/*!
 * =====================================================================================
 *        Class: \class GenericRouter4Stg   
 *    Description: Implementing a generic 4 stage physical router model
 *    BW->RC->VCA->SA->ST->LT
 *    Buffer write(BW)
 *    Route Computation (RC)
 *    Virtual Channel Allocation (VCA)
 *    Switch Allocation (SA)
 *    Switch Traversal (ST)
 *    Link Traversal (LT)
 *
 *    Model Description in cycles:
 *    ---------------------------
 *    BW and RC stages happen in the same cycle ( BW pushes the flits into the
 *    input buffer and the RC unit. )
 *    VCA: Head flits request for an output virtual channel and max(pxv) requesting
 *    input ports/vcs. On winning the grant the packet requests for SA. The
 *    winner is cleared when the tail exits.
 *    SA: Pick one output port from n requesting input ports (0<n<p) for the pxp crossbar 
 *    ST: Move the flits across the crossbar and push it out on the link
 *    LT: This is not modelled within the router and is part of the link component.
 * =====================================================================================
 */

class GenericRouter4Stg: public Router
{
    public:
        GenericRouter4Stg ();                             /*! constructor */
        ~GenericRouter4Stg();                             /*! destructor */
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
//        uint get_credits_for(uint op);
        string toString() const;
        string print_stats();
        void set_edge_links();
        vector< vector<uint> > downstream_credits;
        void dump_buffer_state();

        /*! These are the statistics variables */
        uint packets;
        uint flits;
        double total_packet_latency;
        double last_flit_out_cycle;
        unsigned long long int stat_sim_total_time;
        vector< vector<uint> > stat_packet_out;
        vector< vector<uint> > stat_flit_out;
        bool is_mc_router;

    protected:

    private:
        vector <GenericBuffer> in_buffers;
        vector <GenericRC> decoders;
        vector <InputBufferState> input_buffer_state;
        GenericVcAllocator vca;
        PToPSwitchArbiterVcs swa;
        GenericCrossbar xbar;
        vector <uint> available_ports;
        vector <uint> available_vcs;
        vector < vector<uint> > cr_time;

        bool ticking;
        void handle_link_arrival_event(IrisEvent* e);
        void handle_tick_event(IrisEvent* e);
        void handle_detect_deadlock_event(IrisEvent*);
        void do_switch_traversal();
        void do_switch_allocation();

}; /* -----  end of class GenericRouter4Stg  ----- */

#endif   /* ----- #ifndef _genericRouter4Stg_h_INC  ----- */

