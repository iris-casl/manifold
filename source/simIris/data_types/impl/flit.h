/*
 * =====================================================================================
 *
 *       Filename:  flit.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2010 07:05:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _flit_h_INC
#define  _flit_h_INC

#include	<vector>
#include	<string>
#include	<sstream>
#include	<math.h>
#include	"../../../util/simIrisComponentHeader.h"
#include	"../../../kernel/simulator.h"

using namespace std;

enum flit_type { HEAD, BODY, TAIL};
extern uint max_phy_link_bits;
/*
 * =====================================================================================
 *        Class:  Phit
 *  Description:  
 *
 * =====================================================================================
 */
class Phit
{
    public:
        Phit ();                             /* constructor */
        ~Phit ();
        vector<bool> data;
        string toString() const;

    protected:

    private:

}; /* -----  end of class Phit  ----- */

/*
 * =====================================================================================
 *        Class:  Flit
 *  Description:  
 *
 * =====================================================================================
 */
class Flit
{
    public:
        /* ====================  LIFECYCLE     ======================================= */
        Flit ();                             /* constructor */
        ~Flit ();
        vector<Phit*> phits;
        void populate_phit_data( vector<bool>* c);
        string toString() const;
        flit_type type;
        uint vc;
        bool is_single_flit_pkt;

    protected:

    private:

}; /* -----  end of class Flit  ----- */

/*
 * =====================================================================================
 *        Class:  HeadFlit
 *  Description:  
 * =====================================================================================
 */
class HeadFlit : public Flit
{
    public:
        HeadFlit ();                             /* constructor */
        ~HeadFlit();
        uint src_address;                   /*  Restricting the max number of src to 2^32 */
        uint dst_address;                   /*  32 bits src addr 32 bits dest addr and 32 bits of transaction id max */
        uint transaction_id;
	uint pkt_cnt;			/*Number of packets clubbed in this*/                
        ullint addr;
        message_class msg_class;
        uint length;
        vector<bool> control_bits;
        vector<bool> payload;
        /* Two bits for virtual netwrok, two bits for the virtual channel, three bits for the message class. All this comprises a header control bitfield. */
        void populate_head_flit();
        string toString() const;
        void route();
        pair<uint, uint> next();
        simTime packet_originated_time;
        uint inport;

        /* Stats variables */
        double avg_network_latency;
        unsigned int hop_count;
        ullint stat_memory_serviced_time;
        ullint req_start_time;
        ullint waiting_in_ni;

    protected:

    private:

}; /* -----  end of class HeadFlit  ----- */

/*
 * =====================================================================================
 *        Class:  BodyFlit
 *  Description:  
 *
 * =====================================================================================
 */
class BodyFlit : public Flit
{
    public:
        BodyFlit ();                             /* constructor */
        ~BodyFlit ();                             /* constructor */
        void populate_body_flit();
        vector <bool> bf_data;
        std::string toString() const;

    protected:

    private:

}; /* -----  end of class BodyFlit  ----- */

/*
 * =====================================================================================
 *        Class:  TailFlit
 *  Description:  
 *
 * =====================================================================================
 */
class TailFlit : public Flit
{
    public:
        TailFlit ();                             /* constructor */
        ~TailFlit ();                             /* constructor */
        void populate_tail_flit();
        simTime packet_originated_time;
        simTime scratch_pad_time;
        std::string toString() const;

        /* Stats variables */
        double avg_network_latency;
        unsigned int hop_count;

    protected:

    private:

}; /* -----  end of class TailFlit  ----- */

#endif   /* ----- #ifndef _flit_h_INC  ----- */

