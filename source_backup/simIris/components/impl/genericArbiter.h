/*!
 * =====================================================================================
 *
 *       Filename:  genericvcarbiter.h
 *
 *    Description:  This is a simple arbiter class that is used with the
 *    interface to multiplex among available packets to be sent.
 *    (for usage example: genericInterfaceNB.h)
 *
 *    pick_winner is the main function that arbitration.
 *    Example of how it works:
 *              req0    req1    req2    req3    req4    req5    req6
 *              avilable 2 grants gr0 and gr1
 *              cycle3: req3 req4 req5 request
 *                      gr0  gr1
 *                      after req3 is completed
 *                      req4 req5
 *                      gr1  gr0
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


