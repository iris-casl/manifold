/*
 * =====================================================================================
 *
 *       Filename:  genericrc.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2010 11:54:57 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _genericrc_h_INC
#define  _genericrc_h_INC

#include	"../interfaces/addressDecoder.h"
#include	"../../../util/genericData.h"
#include        "../../data_types/impl/flit.h"
#include	"../../data_types/impl/lowLevelPacket.h"

using namespace std;

extern ROUTING_SCHEME rc_method;
extern uint req_rep;
extern uint grid_size;
extern uint no_nodes;
extern bool do_request_reply_network;
/*
 * =====================================================================================
 *        Class:  GenericRC
 *  Description:  
 * =====================================================================================
 */
class GenericRC
{
    public:
        GenericRC ();
        ~GenericRC(){}
        void push( Flit* f, uint vc );
        uint get_output_port ( uint channel);
        uint speculate_port ( Flit* f, uint ch );
        uint speculate_channel ( Flit* f, uint ch );
        uint get_virtual_channel ( uint ch );
        void resize ( uint ch );
        uint get_no_channels ();
        uint no_adaptive_ports( uint ch );
        uint no_adaptive_vcs( uint ch );
        bool is_empty();
        string toString() const;
        uint node_ip;
        uint address;
        vector < uint > grid_xloc;
        vector < uint > grid_yloc;

    protected:

    private:
        string name;
        uint vcs;
        uint route_x_y( uint addr );
        void route_negative_first(HeadFlit* hf);
        void route_west_first(HeadFlit* hf);
        void route_north_last(HeadFlit* hf);
        void route_north_last_non_minimal(HeadFlit* hf);
        void route_odd_even(HeadFlit* hf);
        vector < uint > possible_out_ports;
        vector < uint > possible_out_vcs;

        /*
         * =====================================================================================
         *        Class:  Address
         *  Description:  
         * =====================================================================================
         */
        class Address
        {
            public:
                bool route_valid;
                unsigned int channel;
                unsigned int out_port;
                uint last_adaptive_port;
                uint last_vc;
                vector < uint > possible_out_ports;
                vector < uint > possible_out_vcs;

            protected:

            private:

        }; /* -----  end of class Address  ----- */
        vector<Address> addresses;

}; /* -----  end of class GenericRC  ----- */

#endif   /* ----- #ifndef _genericrc_h_INC  ----- */

