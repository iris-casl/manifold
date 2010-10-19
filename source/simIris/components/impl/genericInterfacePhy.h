/*!
 * =====================================================================================
 *
 *       Filename:  genericInterfacePhy.h
 *
 *    Description:  This is a interface to be used in a physical network it
 *    has no idea of virtual channels and has a single output channel. It
 *    sends a new packet only if the output buffer is completely empty.
 *    (Wormhole).
 *    NOTE: it cannot handle multiple physical channels.
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

#ifndef  _genericInterfacePhy_h_INC
#define  _genericInterfacePhy_h_INC

#include	"../../data_types/impl/highLevelPacket.h"
#include	"../../data_types/impl/irisEvent.h"
#include	"../interfaces/interface.h"
#include	"../interfaces/networkComponent.h"
#include	"../interfaces/processor.h"
#include	"../interfaces/buffer.h"
#include	"../../../util/genericData.h"
#include	"../impl/genericLink.h"
#include	"genericEvents.h"
#include	"genericBuffer.h"
#include	<queue>
#include	<vector>
#include	<math.h>

extern uint do_two_stage_router;
extern const bool multiple_flit_in_buf;
extern vector<uint> mc_positions;

class GenericInterfacePhy : public Interface
{
    public:
        GenericInterfacePhy ();                             /* constructor */
        ~GenericInterfacePhy ();                             /* destructor */

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

    protected:

    private:
        uint vcs;
        uint buffer_size;
        int credits;
        bool in_packet_cleared;
        bool in_packet_complete;

        bool ticking;
        GenericBuffer out_buffer;
        GenericBuffer in_buffer;
        vector < int > downstream_credits;

        /* The current packet being pushed into the output buffers */
        vector < LowLevelPacket> out_packets;
        vector < uint > out_packet_flit_index;

        /* The current packet being pulled from the input buffers */
        vector < LowLevelPacket> in_packets;
        vector < uint > in_packets_flit_index;
        vector < bool> in_packets_valid;
        vector < bool> pkt_in_progress;

        vector < bool > in_ready;

        /* event handlers */
        void handle_new_packet_event( IrisEvent* e);
        void handle_ready_event( IrisEvent* e);
        void handle_tick_event( IrisEvent* e);
        void handle_link_arrival( IrisEvent* e);
        bool is_pkt_in_progress(uint vc);

        /* stats */
        ullint flits_in;
        ullint packets_in;
        ullint flits_out;
        ullint packets_out;
        ullint total_packets_in_time;

}; /* -----  end of class GenericInterfacePhy  ----- */

#endif   /* ----- #ifndef _genericInterfacePhy_h_INC  ----- */


