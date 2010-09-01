/*
 * =====================================================================================
 *
 *       Filename:  crossbar.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2010 07:16:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _crossbar_h_INC
#define  _crossbar_h_INC

#include	"../../data_types/impl/flit.h"

using namespace std;

/*
 * =====================================================================================
 *        Class:  Crossbar
 *  Description:  
 * =====================================================================================
 */
class Crossbar
{
    public:
        Crossbar (){}                             /* constructor */
        virtual ~Crossbar (){}
        virtual unsigned int get_no_input_ports () = 0;
        virtual unsigned int get_no_output_ports () = 0;
//        virtual void set_no_virtual_channels ( unsigned int number ) = 0;
        virtual unsigned int get_no_channels () = 0;
        virtual unsigned int get_map ( unsigned int input_port, unsigned int channel ) = 0;
        virtual void configure_crossbar ( unsigned int input_port, unsigned int output_port, unsigned int channel ) = 0;
        virtual bool is_full ( unsigned int input_port, unsigned int channel ) = 0;
        virtual bool is_empty ( unsigned int output_port, unsigned int channel ) = 0;

    protected:

    private:

}; /* -----  end of class Crossbar  ----- */

#endif   /* ----- #ifndef _crossbar_h_INC  ----- */


