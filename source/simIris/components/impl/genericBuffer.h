/*
 * =====================================================================================
 *
 *       Filename:  genericbuffer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/20/2010 11:48:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericbuffer_h_INC
#define  _genericbuffer_h_INC

#include	"../interfaces/buffer.h"
#include	"../../data_types/impl/flit.h"
#include	<queue>
#include	<vector>
#define BUFFER_SIZE 20

/*
 * =====================================================================================
 *        Class:  GenericBuffer
 *  Description:  
 * =====================================================================================
 */
class GenericBuffer: public Buffer
{
    public:
        GenericBuffer ();                             /* constructor */
        ~GenericBuffer ();                             /* constructor */
        void push( Flit* f );
        Flit* pull();
        Flit* peek();
        uint get_occupancy( uint ch ) const;
        void resize ( uint vcs, uint buffer_size );
        uint get_no_vcs() const;
        void change_pull_channel( uint ch );
        void change_push_channel( uint ch );
        uint get_pull_channel() const;
        uint get_push_channel() const;
        bool is_channel_full( uint ch ) const;
        bool is_empty( uint ch ) const;

        string toString() const;

        void got_credit( uint ch ) ;
        uint get_no_credits( uint ch ) const;
        void set_no_credits( uint no );
        vector < queue<Flit*> > buffers;
        vector < int > next_port;

    protected:

    private:
        vector < int > credits;
        uint vcs;
        uint buffer_size;
        uint max_credits;
        uint pull_channel;
        uint push_channel;

}; /* -----  end of class GenericBuffer  ----- */

#endif   /* ----- #ifndef _genericbuffer_h_INC  ----- */

