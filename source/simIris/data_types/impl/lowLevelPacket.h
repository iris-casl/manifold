/*
 * =====================================================================================
 *
 *       Filename:  LowLevelPacket.h
 *
 *    Description: This is the desription of the lower link layer protocol
 *    class. It translates from the higher message protocol definition of the
 *    packet to flits. Phits are further handled for the physical layer within
 *    this definition. Assuming a link of 20 bits for phits according to QPI
 *    specs for now. However it would be useful to have these parameters
 *    configurable to be able to do link level modulation at some point.
 *
 *        Version:  1.0
 *        Created:  02/09/2010 08:56:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _lowlevelpacket_h_INC
#define  _lowlevelpacket_h_INC

#include	<vector>
#include	<string>
#include	<sstream>
#include        <deque>
#include	<stdio.h>
#include	"../../../kernel/simulator.h"
#include	"../../../util/simIrisComponentHeader.h"
#include	"flit.h"

using namespace std;

/*
 * =====================================================================================
 *        Class:  LowLevelPacket
 *  Description:  
 *
 * =====================================================================================
 */
class LowLevelPacket
{
    public:
        /* ====================  LIFECYCLE     ======================================= */
        LowLevelPacket ();                             /* constructor */
        ~LowLevelPacket();

        deque<Flit*> flits;
        uint source;
        uint destination;
		uint pkt_cnt;	//Number of requests in this
        uint transaction_id;
        message_class msg_class;
        /* for debug */
        ullint addr;

        /* Stats variables */
        double avg_network_latency;
        unsigned int hop_count;
        unsigned int stat_memory_serviced_time;
        ullint req_start_time;
        ullint waiting_in_ni;

        short int virtual_channel;
        unsigned long int sent_time;
        unsigned int length;  /* Determines the length of the packet in terms of flits */
        vector<bool> control_bits;
        vector<bool> payload;

        void clear();
        void add(Flit* ptr);
        Flit* at(uint index);
        Flit* get_next_flit();  /* This will pop the flit from the queue as well */
        string toString() const;
        bool valid_packet();
        uint size();
        void operator=( const LowLevelPacket* p );

    protected:

    private:

}; /* -----  end of class LowLevelPacket  ----- */

#endif   /* ----- #ifndef _lowlevelpacket_h_INC  ----- */
