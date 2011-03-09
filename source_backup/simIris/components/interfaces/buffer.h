/*
 * =====================================================================================
 *
 *       Filename:  buffer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2010 07:12:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _buffer_h_INC
#define  _buffer_h_INC

#include	"../../data_types/impl/flit.h"
#include	"../../../util/simIrisComponentHeader.h"

using namespace std;


/*
 * =====================================================================================
 *        Class:  Buffer
 *  Description:  
 * =====================================================================================
 */
class Buffer
{
    public:
        Buffer (){}                             /* constructor */
        ~Buffer() {}
        virtual void push ( Flit* f)  = 0;
        virtual Flit* pull () = 0;
        virtual uint get_occupancy ( uint channel ) const = 0;

    protected:

    private:

}; /* -----  end of class Buffer  ----- */

#endif   /* ----- #ifndef buffer_INC  ----- */

