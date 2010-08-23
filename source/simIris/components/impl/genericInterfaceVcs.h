/*
 * =====================================================================================
 *
 *       Filename:  genericinterfacevcs.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/24/2010 02:04:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericinterfacevcs_h_INC
#define  _genericinterfacevcs_h_INC

#include	"../../../util/genericData.h"
#include	"../../data_types/impl/highLevelPacket.h"
#include	"../../data_types/impl/irisEvent.h"
#include	"../interfaces/interface.h"
#include	"../interfaces/networkComponent.h"
#include	"../interfaces/processor.h"
#include	"../interfaces/buffer.h"
#include	"../impl/genericLink.h"
#include	"genericEvents.h"
#include	"genericBuffer.h"
#include	"genericArbiter.h"
#include	<queue>
#include	<vector>
#include	<math.h>
#include	<algorithm>
#define DEFAULT_NO_OF_CREDITS 1
/*
 * =====================================================================================
 *        Class:  GenericInterfaceVcs
 *  Description:  
 * =====================================================================================
 */
extern uint do_two_stage_router;
extern vector <uint> mc_positions;

class GenericInterfaceVcs : public Interface
{
    public:
        GenericInterfaceVcs();                             /* constructor */
        ~GenericInterfaceVcs();                             /* constructor */

        uint get_no_credits() const;
        void set_no_credits( int credits );
        void set_no_vcs ( uint v );
        void set_buffer_size( uint b );
        void setup( uint v, uint cr);

        string toString() const;
        void process_event( IrisEvent* e);
        string print_stats();
        ullint get_packets_out();
        ullint get_packets();
        ullint get_flits_out();
        bool is_mc_interface;

    protected:

    private:
        uint vcs;
        uint buffer_size;
        int credits;
        bool in_packet_cleared;
        vector <bool> in_packet_complete;
        uint flast_vc;

        bool ticking;
        GenericBuffer out_buffer;
        GenericBuffer in_buffer;
        GenericArbiter out_arbiter;
        vector < int > downstream_credits;

        /* The current packet being pushed into the output buffers */
        vector < LowLevelPacket> out_packets;
        vector < uint > out_packet_flit_index;

        /* The current packet being pulled from the input buffers */
        vector < LowLevelPacket> in_packets;
        vector < uint > in_packets_flit_index;
        vector < bool> in_packets_valid;

        vector < bool > in_ready;

        /* event handlers */
        void handle_new_packet_event( IrisEvent* e);
        void handle_ready_event( IrisEvent* e);
        void handle_tick_event( IrisEvent* e);
        void handle_link_arrival( IrisEvent* e);

        /* stats */
        ullint flits_in;
        ullint packets_in;
        ullint flits_out;
        ullint packets_out;
        ullint total_packets_in_time;

}; /* -----  end of class GenericInterfaceVcs  ----- */

#endif   /* ----- #ifndef _genericinterfacevcs_h_INC  ----- */


