/*
 * =====================================================================================
 *
 *       Filename:  GenericVcArbiter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/21/2010 02:25:43 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _GenericVcArbiter_h_INC
#define  _GenericVcArbiter_h_INC

#include	"../interfaces/arbiter.h"
#include	"../../data_types/impl/flit.h"
#include	<vector>

using namespace std;

/*
 * =====================================================================================
 *        Class:  GenericVcArbiter
 *  Description:  
 * =====================================================================================
 */
class GenericVcArbiter: public Arbiter
{
    public:
        uint node_ip;
        uint address;
        GenericVcArbiter ();                             /* constructor */
        ~GenericVcArbiter (); 

        bool is_requested( uint ch );
        void set_req_queue_size( uint size);
        void request( Flit* f, uint index );
        Flit* pull_winner();
        uint get_no_requests();

        uint pick_winner();
        uint pick_winner( vector<bool> ready );
        void clear_winner();
        unsigned long long int write_time;

        bool empty();
        bool empty ( vector<bool> ready);
        string toString() const;
        vector<uint > next_port;

    protected:

    private:
        string name;
        vector<bool> requests;
        vector<Flit* > flits;
        uint last_winner;
        bool done;
        bool arb;

}; /* -----  end of class GenericVcArbiter  ----- */

#endif   /* ----- #ifndef _GenericVcArbiter_h_INC  ----- */


