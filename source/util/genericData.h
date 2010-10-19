/*
 * =====================================================================================
 *
 *       Filename:  genericData.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/21/2010 04:59:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _genericdata_h_INC
#define  _genericdata_h_INC

#include	"../simIris/data_types/impl/flit.h"
#include	"simIrisComponentHeader.h"

using namespace std;

enum ROUTING_SCHEME { XY, NEGATIVE_FIRST, ODD_EVEN, WEST_FIRST, NORTH_LAST, NORTH_LAST_NON_MINIMAL, TORUS_ROUTING, RING_ROUTING };
enum SW_ARBITRATION { ROUND_ROBIN, FCFS, ROUND_ROBIN_PRIORITY };
enum RouterPipeStage { 
   /* 0 */ INVALID, 
   /* 1 */ EMPTY,
   /* 2 */ IB, 
   /* 3 */ FULL, 
   /* 4 */ ROUTED,
   /* 5 */ VCA_REQUESTED, 
   /* 6 */ SWA_REQUESTED, 
   /* 7 */ SW_ALLOCATED, 
   /* 8 */ SW_TRAVERSAL, 
   /* 9 */ REQ_OUTVC_ARB, 
   /* 10 */ VCA_COMPLETE 
};

enum  ROUTER_MODEL { PHYSICAL, VIRTUAL};
enum  MC_MODEL { GENERIC_MC, FLAT_MC, SINK};
enum  TERMINAL_MODEL { GENERIC_PKTGEN, TPG};

class InputBufferState
{
    public:
        InputBufferState();
        ~InputBufferState(){}
        uint input_port;
        uint input_channel;
        uint output_port;
        uint output_channel;
        double stat_pkt_intime;
        double arrival_time;
        int length;
        int credits_sent;
        vector < uint > possible_ovcs;
        vector < uint > possible_oports;
        RouterPipeStage pipe_stage;
        message_class msg_class;
        ullint address;
        uint destination;
        bool clear_message;
        bool sa_head_done;
        uint flits_in_ib;
        bool sent_head;
        bool has_pending_pkts;
        bool pkt_in_progress;
        int bodies_sent;
        string toString () const;

};

/*
 * =====================================================================================
 *        Class:  LinkArrivalData
 *  Description:  
 * =====================================================================================
 */
class LinkArrivalData
{
    public:
        LinkArrivalData ();                             /* constructor */
        ~LinkArrivalData ();                             /* constructor */
        uint type;
        uint vc;
        Flit* ptr;
        bool valid;

    protected:

    private:

}; /* -----  end of class LinkArrivalData  ----- */

/*
 * =====================================================================================
 *        Class:  VirtualChannelDescription
 *  Description:  
 * =====================================================================================
 */
class VirtualChannelDescription
{
    public:
        VirtualChannelDescription ();                             /* constructor */
        uint vc;
        uint port;

    protected:

    private:

}; /* -----  end of class VirtualChannelDescription  ----- */

/*
 * =====================================================================================
 *        Class:  RouteEntry
 *  Description:  
 * =====================================================================================
 */
class RouteEntry
{
    public:
        RouteEntry ();                             /* constructor */
        uint destination;
        vector< vector<uint> > ports;
        vector< vector<uint> > channels;

    protected:

    private:

}; /* -----  end of class RouteEntry  ----- */

class SA_unit
{
    public:
        SA_unit(){};
        uint port;
        uint ch;
        ullint in_time;
        ullint win_cycle;
};

#endif   /* ----- #ifndef _genericdata_h_INC  ----- */

