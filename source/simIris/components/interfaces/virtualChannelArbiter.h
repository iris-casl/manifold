/*
 * =====================================================================================
 *
 *       Filename:  virtualChannelArbiter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2010 01:01:53 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _virtualchannelarbiter_h_INC
#define  _virtualchannelarbiter_h_INC

#include	"../../data_types/impl/flit.h"
#include	<vector>

using namespace std;

/*
 * =====================================================================================
 *        Class:  VirtualChannelArbiter
 *  Description:  
 * =====================================================================================
 */
class VirtualChannelArbiter
{
    public:
        VirtualChannelArbiter (){}                             /* constructor */
        virtual void request ( Flit* f, uint vc ) = 0;
        virtual Flit* pull_winner () = 0;
//        virtual Flit* peek_winner () = 0;
        virtual bool ready ( uint ch ) = 0;
        virtual uint size() = 0;
        virtual void resize( uint ports ) = 0;
//        virtual vector< uint > pick_winners ( uint winners );
        virtual uint pick_winner () = 0;
        virtual void clear () = 0;
        virtual void clear_winner () = 0;
        virtual vector < uint > get_requests () = 0;

    protected:

    private:

}; /* -----  end of class VirtualChannelArbiter  ----- */

#endif   /* ----- #ifndef _virtualchannelarbiter_h_INC  ----- */
