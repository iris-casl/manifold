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

#ifndef  _ptopswitcharbiter_h_INC
#define  _ptopswitcharbiter_h_INC

#include	"../interfaces/networkComponent.h"
#include	"../../data_types/impl/flit.h"
#include	"../../../util/genericData.h"
#include	<vector>
#include	<fstream>

using namespace std;
extern SW_ARBITRATION sw_arbitration;
extern message_class priority_msg_type;
class PToPSwitchArbiter
{
    public:
        PToPSwitchArbiter ();                             /* constructor */
        ~PToPSwitchArbiter();
        void resize(uint p);
        bool is_requested(uint outp, uint inp);
        void request(uint p, uint inp);
        SA_unit pick_winner( uint p);
        SA_unit do_round_robin_arbitration( uint p);
        SA_unit do_priority_round_robin_arbitration( uint p);
        SA_unit do_fcfs_arbitration( uint p);
        void clear_winner( uint p, uint ip);
        void clear_requested( uint p, uint ip);
        void request(uint oport, uint inport, message_class m);
        bool is_empty();
        string toString() const;
        uint address;
        string name;
        uint node_ip;

    protected:

    private:
        uint PORTS;
        uint CHANNELS;
        vector < bool>  locked;
        vector < bool> done;
        vector < vector <bool> > requested;
        vector < vector <bool> > priority_reqs;
        vector < bool> port_locked;
        vector < vector<SA_unit> > requesting_inputs;
        vector < SA_unit > last_winner;
        vector < uint> last_port_winner;

}; /* -----  end of class PToPSwitchArbiter  ----- */

#endif   /* ----- #ifndef ptopswitcharbiter_INC  ----- */
