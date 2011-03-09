/*!
 * =====================================================================================
 *
 *       Filename:  genericarbiter.cc
 *
 *    Description: Implements the class in genericArbiter.h
 *
 *        Version:  1.0
 *        Created:  05/02/2010 01:41:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _genericarbiter_cc_INC
#define  _genericarbiter_cc_INC

#include	"genericArbiter.h"

GenericArbiter::GenericArbiter()
{
    name = "GenericArbiter";
    address = -1;
    node_ip = -1;
    last_winner = -1;
    done = false;
}		/* -----  end of function GenericArbiter  ----- */

GenericArbiter::~GenericArbiter()
{

}

void
GenericArbiter::set_no_requestors(uint ch)
{
    vcs = ch;
    requests.resize(ch);
    for ( uint i=0 ; i<ch ; i++ )
        requests[i] = false;
}

void
GenericArbiter::request( uint ch)
{
    requests[ch] = true;
    done = false;
    return;
}

bool
GenericArbiter::is_requested( uint ch)
{
    assert(ch<requests.size());
    return requests[ch];
}

bool
GenericArbiter::is_empty()
{
    for ( uint i=0; i<vcs; i++)
    {
        if( requests[i] )
            return false;
    }
    return true;
}

uint
GenericArbiter::get_no_requestors()
{
    uint no =0;
    for ( uint i=0; i<vcs; i++)
        if ( requests[i] )
            no++;

    return no;
}

void
GenericArbiter::clear_winner()
{
    done = false;
    requests[last_winner] = false;
    return;
}

uint
GenericArbiter::pick_winner()
{
    if(!done)
    {
        done = true;
        for ( uint i=last_winner+1 ; i<requests.size() ; i++ )
            if ( requests[i] )
            {
                last_winner = i;
                return i;
            }

        for ( uint i=0 ; i<last_winner+1 ; i++ )
            if ( requests[i])
            {
                last_winner = i;
                return i;
            }
    }
    else
        return last_winner;

    printf("Was requested to pick winner but dint find a winner " );

    return -1;
}

string
GenericArbiter::toString () const
{
    stringstream str;
    str << "GenericArbiter: "
        << "\t last_winner: " << last_winner
        << "\t no of requests: " << requests.size();
    return str.str();
}	

#endif   /* ----- #ifndef _genericarbiter_cc_INC  ----- */

