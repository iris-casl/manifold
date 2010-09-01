/*
 * =====================================================================================
 *
 *       Filename:  HighLevelPacket.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/09/2010 08:23:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _highlevelpacket_h_INC
#define  _highlevelpacket_h_INC
#include	<sstream>
#include	<string>
#include	<math.h>
#include	"../../../kernel/simulator.h"
#include	"../../../util/simIrisComponentHeader.h"
#include	"lowLevelPacket.h"

#define CONTROL_VECTOR_SIZE 16

using namespace std;


/*
 * =====================================================================================
 *        Class:  HighLevelPacket
 *  Description:  
 *
 * =====================================================================================
 */
class HighLevelPacket
{
    public:
        /* ====================  LIFECYCLE     ======================================= */
        HighLevelPacket ();                             /* constructor */
        ~HighLevelPacket();

        /* ====================  STATISTICS ======================================= */
        /* Keep track of the packet transit time */
        simTime get_transit_time();

        /* ====================  OPERATIONS      ======================================= */
        void to_low_level_packet(LowLevelPacket* llp) ;
        void from_low_level_packet(LowLevelPacket* llp);

        /* ====================  OPERATORS     ======================================= */
        string toString() const;
        /* Assuming that a minimal check on the transaction id and source should suffice. In accordance with the QPI spec all packets have the transaction Id which can be mapped back to the caching agent that generated the request. Once the transaction is completed and the response arrives at the caching agent, a tear down is initiated and the transaction ids may be reused at the source.*/
        bool operator==( const HighLevelPacket* p );

        /* for debug */
        ullint addr;

        /* Stats variables */
        double avg_network_latency;
        unsigned int hop_count;
        unsigned int stat_memory_serviced_time;
        ullint req_start_time;
        ullint waiting_in_ni;

        uint source;
        uint destination;
	uint pkt_cnt;	//The number of requests clubbed together
	int reqs_left;	// requests left in outstanding_hlp
	uint startIndex;	//the index till where packets have been stripped and sent to the banks
        virtual_network vn;
        message_class msg_class;
        uint virtual_channel;
        uint transaction_id;
        simTime sent_time;
        simTime recv_time;
        unsigned int data_payload_length;       /* length of the data payload in bytes */
        vector<bool> data;

    protected:

    private:

}; /* -----  end of class HighLevelPacket  ----- */

#endif   /* ----- #ifndef _highlevelpacket_h_INC  ----- */

