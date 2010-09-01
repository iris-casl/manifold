/*
 * =====================================================================================
 *
 *       Filename:  genericvcarbiter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/02/2010 01:35:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _genericvcarbiter_h_INC
#define  _genericvcarbiter_h_INC

#include	<vector>
#include	"../interfaces/arbiter.h"

/*
 * =====================================================================================
 *        Class:  GenericArbiter
 *  Description:  
 * =====================================================================================
 */
class GenericArbiter
{
    public:
        GenericArbiter ();
        ~GenericArbiter();
        uint node_ip;
        uint address;
        string name;
        bool is_requested(uint ch);
        bool is_empty();
        void set_no_requestors( uint ch);
        void request( uint ch);
        uint get_no_requestors();
        uint pick_winner();
        string toString() const;
        void clear_winner();

    protected:

    private:
        uint vcs;
        vector < bool > requests;
        uint last_winner;
        bool done;

}; /* -----  end of class genericvcarbiter  ----- */


#endif   /* ----- #ifndef _genericvcarbiter_h_INC  ----- */


