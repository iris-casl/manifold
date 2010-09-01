/*
 * =====================================================================================
 *
 *       Filename:  portArbiter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2010 11:57:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _portarbiter_h_INC
#define  _portarbiter_h_INC

#include	<vector>
#include	"../../data_types/impl/flit.h"
#include	"networkComponent.h"

/*
 * =====================================================================================
 *        Class:  PortArbiter
 *  Description:  
 * =====================================================================================
 */
class PortArbiter
{
    public:
        PortArbiter(){}                             /* constructor */
        ~PortArbiter(){}
        string name;

    protected:

    private:

}; /* -----  end of class PortArbiter  ----- */

#endif   /* ----- #ifndef _portarbiter_h_INC  ----- */

