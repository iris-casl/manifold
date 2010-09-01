/*
 * =====================================================================================
 *
 *       Filename:  addressDecoder.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2010 07:06:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _addressdecoder_h_INC
#define  _addressdecoder_h_INC

#include	"../../data_types/impl/flit.h"
using namespace std;

/*
 * =====================================================================================
 *        Class:  AddressDecoder
 *  Description:  
 * =====================================================================================
 */
class AddressDecoder
{
    public:
        AddressDecoder ();                             /* constructor */
        virtual void push ( Flit* f, unsigned int vc) = 0;
        virtual Flit* pull ( unsigned int vc )  = 0;
        virtual unsigned int get_putput_port ( unsigned int channe ) = 0;
        virtual unsigned int get_vc ( unsigned int channel ) = 0;
        virtual void no_channels ( unsigned int channels ) = 0;
        virtual unsigned int set_no_channels () = 0;
        virtual bool is_channel_empty ( unsigned int channel ) = 0;
        virtual bool is_empty () = 0;
        virtual unsigned int speculate_port ( Flit* f, unsigned int port ) = 0;
        virtual unsigned int speculate_channel ( Flit* f, unsigned int channel ) = 0;


    protected:

    private:

}; /* -----  end of class AddressDecoder  ----- */

#endif   /* ----- #ifndef _addressdecoder_h_INC  ----- */

