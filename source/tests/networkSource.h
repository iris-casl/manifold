/*
 * =====================================================================================
 *
 *       Filename:  netoworkSource.cc
 *
 *    Description: version 1 is used to test the interface class by generating
 *    low level packets at random times and delivering them on a link arrival
 *    to the interface.
 *
 *        Version:  1.0
 *        Created:  02/21/2010 10:28:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _netoworksource_h_INC
#define  _netoworksource_h_INC

#include	"MersenneTwister.h"
#include	"../components/interfaces/processor.h"
#include	"../components/impl/genericEvents.h"
#include	"../components/impl/genericData.h"
#include	"../components/impl/genericBuffer.h"
#include	"../components/impl/genericVcArbiter.h"
#include	"../data_types/impl/lowLevelPacket.h"
#include	"../data_types/impl/highLevelPacket.h"
#include	<deque>
#include	<fstream>

using namespace std;
/*
 * =====================================================================================
 *        Class:  GenericNetworkSource
 *  Description:  
 * =====================================================================================
 */
class GenericNetworkSource: public Processor
{
    public:
        GenericNetworkSource ();                             /* constructor */
        virtual ~GenericNetworkSource (){};
        unsigned long int max_time();
        void setup();
        void init();
        void process_event(IrisEvent* e);
        string toString() const;
        NetworkComponent* output_connection;
        NetworkComponent* input_connection;
        uint node_ip;
        GenericOutputBuffer out_buffer;

    protected:

    private:
        deque<LowLevelPacket*> packets;
        vector< deque<Flit*> > flits;
        uint current_send_vc;
        void generate_random_packets_for_interface(uint i);
        bool ticking;
        bool done;
        bool done_sending;
        uint sending_vc;
        ofstream in_file;
        string in_filename;
        unsigned long long int last_sent_cycle;
        
        /* Event handlers */
        void handle_tick_event(IrisEvent* e);
        void check_tick();
        void handle_ready_event( IrisEvent* e);
        void handle_link_arrival_event( IrisEvent* e);

}; /* -----  end of class GenericNetworkSource  ----- */

#endif   /* ----- #ifndef _netoworksource_cc_INC  ----- */

