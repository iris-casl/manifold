/*
 * =====================================================================================
 *
 *       Filename:  genericvcarbiternoflits.cc
 *
 *    Description:  
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
#ifndef  _genericvcarbiternoflits_cc_INC
#define  _genericvcarbiternoflits_cc_INC

#include	"genericVcArbiterNoFlits.h"

genericvcarbiternoflitsNoFlits::genericvcarbiternoflitsNoFlits()
{
    name = "genericvcarbiternoflitsNoFlits";
    address = -1;
    node_ip = -1;
    last_winner = 0;
    done = false;
}		/* -----  end of function GenericArbiter  ----- */

genericvcarbiternoflitsNoFlits::~genericvcarbiternoflitsNoFlits()
{

}

void
genericvcarbiternoflitsNoFlits::set_no_vcs(uint ch)
{
    vcs = ch;
    requests.resize(ch);
    for ( uint i=0 ; i<ch ; i++ )
        requests[i] = false;
}

void
genericvcarbiternoflitsNoFlits::request( uint ch)
{
    requests[ch] = true;
    done = false;
    return;
}

bool
genericvcarbiternoflitsNoFlits::is_requested( uint ch)
{
    assert(ch<requests.size());
    return requests[ch];
}

uint
genericvcarbiternoflitsNoFlits::get_no_requests()
{
    uint no =0;
    for ( uint i=0; i<vcs; i++)
        if ( requests[i] )
            no++;

    return no;
}

void
genericvcarbiternoflitsNoFlits::clear_winner()
{
    done = false;
    requests[last_winner] = false;
    return;
}

uint
genericvcarbiternoflitsNoFlits::pick_winner()
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
genericvcarbiternoflitsNoFlits::toString () const
{
    stringstream str;
    str << "genericvcarbiternoflitsNoFlits: "
        << "\t last_winner: " << last_winner
        << "\t no of requests: " << requests.size();
    return str.str();
}	

#endif   /* ----- #ifndef _genericvcarbiternoflits_cc_INC  ----- */

