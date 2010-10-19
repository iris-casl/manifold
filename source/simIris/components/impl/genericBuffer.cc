/*!
 * =====================================================================================
 *
 *       Filename:  genericbuffer.cc
 *
 *    Description:  Implements the buffer class. 
 *
 *        Version:  1.0
 *        Created:  02/21/2010 12:55:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _genericbuffer_cc_INC
#define  _genericbuffer_cc_INC

#include	"genericBuffer.h"

GenericBuffer::GenericBuffer ()
{
    pull_channel = 0;
    push_channel = 0;
}		/* -----  end of function GenericBuffer::GenericBuffer  ----- */

GenericBuffer::~GenericBuffer ()
{
}

void
GenericBuffer::push ( Flit* f )
{
    buffers[push_channel].push_back(f);
    assert(buffers[push_channel].size() <= buffer_size);
    return;
}		/* -----  end of function GenericBuffer::push  ----- */

Flit*
GenericBuffer::pull ()
{
    if( pull_channel > buffers.size() || buffers[pull_channel].size() == 0)
    {
        fprintf( stdout, "\nERROR: Invalid pull channel pull_ch:%d  no_of_ch:%d buff_occ:%d",
                 pull_channel, buffers.size(),buffers[pull_channel].size());
        exit(1);
    }
    Flit* f = buffers[pull_channel].front();
    buffers[pull_channel].pop_front();
    assert(buffers[pull_channel].size() <= buffer_size);
    return f;
}		/* -----  end of function GenericBuffer::pull  ----- */

/*!
 * ===  FUNCTION  ======================================================================
 *         Name:  peek
 *  Description:  Uses the pull channel. Make sure to set it before peek is
 *  called. Normal usage is to get a pointer to the head of the buffer without
 *  actually pulling it out from the buffer
 * =====================================================================================
 */
Flit*
GenericBuffer::peek ()
{
    if( pull_channel > buffers.size() || buffers[pull_channel].size() == 0)
    {
        fprintf( stdout, "\nERROR: **Invalid pull channel");
        exit(1);
    }
    Flit* f = buffers[pull_channel].front();
    assert(buffers[pull_channel].size() <= buffer_size);
    return f;
}		/* -----  end of function GenericBuffer::pull  ----- */

uint
GenericBuffer::get_occupancy ( uint ch ) const
{
    return buffers[ch].size();
}		/* -----  end of function GenericBuffer::get_occupancy  ----- */

void
GenericBuffer::resize( uint v, uint bs )
{
    /* 
    for ( uint i = vcs; i<buffers.size(); i++ )
        while( !buffers[i].empty())
        {
            delete buffers[i].front();
            buffers[i].pop();
        }
     * */
    vcs =v;
    buffer_size = bs;
    buffers.resize(vcs);
    for( uint i=0; i<vcs; i++)
    {
       buffers[i].clear();
       assert(buffers[i].size()<=buffer_size);
    }

    return;
}		/* -----  end of function GenericBuffer::change_vcs  ----- */

uint
GenericBuffer::get_no_vcs() const
{
    /*  should be return the pvt variable but for now using this to ensure
     *  init was right and not accessing member variable. */
    return buffers.size();
}

void
GenericBuffer::change_pull_channel ( uint ch )
{
    assert(ch<vcs);
    pull_channel = ch;
    return;
}		/* -----  end of function GenericBuffer::change_pull_channel  ----- */

void
GenericBuffer::change_push_channel ( uint ch )
{
    push_channel = ch;
    return;
}		/* -----  end of function GenericBuffer::change_pull_channel  ----- */


uint
GenericBuffer::get_pull_channel () const
{
    return pull_channel;
}		/* -----  end of function GenericBuffer::get_pull_channel  ----- */

uint
GenericBuffer::get_push_channel () const
{
    return push_channel;
}		/* -----  end of function GenericBuffer::get_push_channel  ----- */

bool
GenericBuffer::is_channel_full ( uint ch ) const
{
    /* this is the buffer size that the router is configured for the implementation allow for a bigger buffer[i].size which is the simulation artifact and not the buffer size in the physical router */
    return buffers[ch].size() > buffer_size;  
}		/* -----  end of function GenericBuffer::full  ----- */

bool
GenericBuffer::is_empty (uint ch ) const
{
    return buffers[ch].empty();
}		/* -----  end of function GenericBuffer::empty  ----- */

string
GenericBuffer::toString () const
{
    stringstream str;
    str << "GenericBuffer"
        << "\t buffer_size: " << buffer_size
        << "\t No of buffers: " << buffers.size() << "\n";
    for( uint i=0; i<buffers.size() && !buffers[i].empty(); i++)
        str << buffers[i].front()->toString();
    return str.str();
}		/* -----  end of function GenericBuffer::toString  ----- */

#endif   /* ----- #ifndef _genericbuffer_cc_INC  ----- */

