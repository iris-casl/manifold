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

#ifndef  _genericRouterAdaptive_h_INC
#define  _genericRouterAdaptive_h_INC

#include	"../interfaces/router.h"
#include	"genericBuffer.h"
#include	"genericRC.h"
//#include	"pvtopv_swa.h"
#include	"ptop_swa.h"
#include	"genericCrossbar.h"
#include	"genericEvents.h"
#include	"../../../util/genericData.h"
#include	"genericLink.h"
#include	<sys/time.h>
#include	<algorithm>

extern uint send_early_credit;
extern uint do_two_stage_router;
extern const bool multiple_flit_in_buf;

class GenericRouterAdaptive: public Router
{
    public:
        GenericRouterAdaptive ();                             /* constructor */
        ~GenericRouterAdaptive();
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

    protected:

    private:
        vector <GenericBuffer> in_buffers;
        vector <GenericBuffer> new_pkt_buffer;
        vector <GenericRC> decoders;
        vector <InputBufferState> input_buffer_state;
        PToPSwitchArbiter swa;
        GenericCrossbar xbar;
        vector <uint> available_ports;

        bool ticking;
        void init_buffer_state(uint, uint, HeadFlit*);
        void handle_link_arrival_event(IrisEvent* e);
        void handle_link_arrival_event_one_msg_per_buffer(IrisEvent*);
        void handle_link_arrival_event_multiple_flit_in_buffer(IrisEvent*);
        void handle_tick_event(IrisEvent* e);
        void do_switch_traversal();
        void do_switch_allocation();
        bool is_pkt_in_progress(GenericLink*, uint);

}; /* -----  end of class GenericRouterAdaptive  ----- */

#endif   /* ----- #ifndef _genericRouterAdaptive_h_INC  ----- */

