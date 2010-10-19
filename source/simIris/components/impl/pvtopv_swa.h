/*
 * =====================================================================================
 *
 *       Filename:  myFullyVirtualArbiter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2010 01:52:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _pvtopv_swa_h_INC
#define  _pvtopv_swa_h_INC

#include	"../interfaces/networkComponent.h"
#include	"../../data_types/impl/flit.h"
#include	"../../../util/genericData.h"
#include	<vector>

class PVToPV_swa
{
    public:
        PVToPV_swa ();                             /* constructor */
        ~PVToPV_swa();
        void resize(uint p, uint c);
        bool is_requested(uint inp, uint inch, uint p, uint c);
        void request(uint p, uint c, uint inp, uint inch);
        SA_unit pick_winner( uint p, uint c);
        void clear_winner( uint p, uint c, uint ip, uint ic);
        bool is_empty();
        bool is_empty_for_ch(uint ch);
        uint no_requests_ch(uint ch);
        string toString() const;
        uint address;
        uint name;
        uint node_ip;

    protected:

    private:
        uint PORTS;
        uint CHANNELS;
        vector < vector<bool> > locked;
        vector < vector<bool> > done;
        vector < vector<bool> > requested;
        vector < bool> port_locked;
        vector < vector<SA_unit> > requesting_inputs;
        vector < vector<SA_unit> > last_winner;
        vector < vector<uint> > last_port_winner;
        vector < vector<uint> > last_ch_winner;


}; /* -----  end of class PVToPV_swa  ----- */

#endif   /* ----- #ifndef pvtopv_swa_INC  ----- */
