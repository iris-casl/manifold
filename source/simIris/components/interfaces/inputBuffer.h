/*
 * =====================================================================================
 *
 *       Filename:  inputBuffer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2010 07:46:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _inputbuffer_h_INC
#define  _inputbuffer_h_INC

#include	"buffer.h"

typedef unsigned int uint;

/*
 * =====================================================================================
 *        Class:  InputBuffer
 *  Description:  
 * =====================================================================================
 */
class InputBuffer : public Buffer
{
    public:
        InputBuffer (){}                            /* constructor */
        virtual ~InputBuffer(){}
        virtual void set_no_vc ( uint vc );
        virtual uint get_no_vc () const;
        virtual void change_pull_channel ( uint channel );
        virtual void change_push_channel ( uint channel );
        virtual uint get_pull_channel () const;
        virtual uint get_push_channel () const;
        virtual bool is_channel_full ( uint channel ) const;
        virtual bool is_empty ( uint channel ) const;

    protected:

    private:

}; /* -----  end of class InputBuffer  ----- */

#endif   /* ----- #ifndef _inputbuffer_h_INC  ----- */
