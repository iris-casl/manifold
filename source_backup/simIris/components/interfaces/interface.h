/*
 * =====================================================================================
 *
 *       Filename:  interface.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2010 11:28:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _interface_h_INC
#define  _interface_h_INC

#include	"networkComponent.h"
#include	"buffer.h"
#include	"irisLink.h"
#include	"../../data_types/impl/irisEvent.h"

/*
 * =====================================================================================
 *        Class:  Interface
 *  Description:  
 * =====================================================================================
 */
class Interface: public NetworkComponent
{
    public:
        NetworkComponent* processor_connection;
        IrisLink* input_connection;
        IrisLink* output_connection;
        Interface ();                             /* constructor */
        virtual ~Interface();
        virtual string toString () const;
        virtual void setup(uint a, uint cr, uint bs) = 0;
        virtual void set_no_credits( int cr ) = 0;
        virtual void set_buffer_size( uint cr ) = 0;
        virtual void set_no_vcs( uint cr ) = 0;
        virtual void process_event( IrisEvent* e) = 0;
        virtual string print_stats () = 0;
        virtual ullint get_packets_out () = 0;
        virtual ullint get_packets() = 0;
        virtual ullint get_flits_out () = 0;

    protected:

    private:

}; /* -----  end of class Interface  ----- */

#endif   /* ----- #ifndef _interface_h_INC  ----- */
