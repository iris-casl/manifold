/*
 * =====================================================================================
 *
 *       Filename:  outputBuffer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2010 07:53:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _outputbuffer_h_INC
#define  _outputbuffer_h_INC

#include	"buffer.h"

typedef unsigned int uint;
/*
 * =====================================================================================
 *        Class:  OutputBuffer
 *  Description:  
 * =====================================================================================
 */
class OutputBuffer : public Buffer
{
    public:
        OutputBuffer (){}                             /* constructor */
        virtual ~OutputBuffer (){} 

        virtual uint get_no_vcs () const = 0;
        virtual void change_pull_channel ( uint channel ) = 0;
        virtual void change_push_channel ( uint channel ) = 0;
        virtual uint get_pull_channel () const = 0;
        virtual uint get_push_channel () const = 0;
        virtual bool is_channel_full ( uint channel ) const = 0;
        virtual bool is_empty ( uint channel ) const = 0;

    protected:

    private:

}; /* -----  end of class OutputBuffer  ----- */

#endif   /* ----- #ifndef _outputbuffer_h_INC  ----- */
