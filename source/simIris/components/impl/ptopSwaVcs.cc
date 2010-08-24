/*
 * =====================================================================================
 *
 *       Filename:  myFullyVirtualArbiter.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2010 12:58:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _ptopswitcharbitervcs_cc_INC
#define  _ptopswitcharbitervcs_cc_INC

#include	"ptopSwaVcs.h"

PToPSwitchArbiterVcs::PToPSwitchArbiterVcs()
{
    name = "swa";
}

PToPSwitchArbiterVcs::~PToPSwitchArbiterVcs()
{
}

void
PToPSwitchArbiterVcs::resize(uint p, uint v)
{
    ports = p;
    vcs = v;
    requested.resize(ports);
    priority_reqs.resize(ports);
    last_port_winner.resize(ports);
    requesting_inputs.resize(ports);
    last_winner.resize(ports);

    for ( uint i=0; i<ports; i++)
    {
        requested[i].resize(ports*vcs);
        requesting_inputs[i].resize(ports*vcs);
        last_winner[i].win_cycle= 0;
    }

    for ( uint i=0; i<ports; i++)
        for ( uint j=0; j<(ports*vcs); j++)
        {
            requested[i][j]=false;
        }

    for ( uint i=0; i<ports; i++)
    {
        last_port_winner[i] = -1;
    }
}

bool
PToPSwitchArbiterVcs::is_requested( uint oport, uint inport, uint och )
{
    if( oport >= ports || inport >= ports || och >= vcs)
    {
        cout << " Error in SWA oport: "<< oport <<" inp: " << inport <<" och: " << och << endl;
        exit(1);
    }
    return requested[oport][inport*vcs+och];
}

void
PToPSwitchArbiterVcs::request(uint oport, uint ovc, uint inport, uint ivc )
{
    requested[oport][inport*vcs+ovc] = true;
    requesting_inputs[oport][inport*vcs+ovc].port = inport;
    requesting_inputs[oport][inport*vcs+ovc].ch=ivc;
    requesting_inputs[oport][inport*vcs+ovc].in_time = (ullint)Simulator::Now();
    return;
}

SA_unit
PToPSwitchArbiterVcs::pick_winner( uint oport)
{
    return do_round_robin_arbitration(oport);
}

SA_unit
PToPSwitchArbiterVcs::do_round_robin_arbitration( uint oport)
{

    if( last_winner[oport].win_cycle >= Simulator::Now())
        return last_winner[oport];

    /* Now look at contesting input ports on this channel and pick
     * a winner*/
    bool winner_found = false;
    for( uint i=last_port_winner[oport]+1; i<(ports*vcs); i++)
    {
        if(requested[oport][i])
        {
            last_port_winner[oport] = i;
            winner_found = true;
            last_winner[oport].port = requesting_inputs[oport][i].port;
            last_winner[oport].ch= requesting_inputs[oport][i].ch;
            last_winner[oport].win_cycle= Simulator::Now();
            return last_winner[oport];
        }
    }

    if(!winner_found)
        for( uint i=0; i<=last_port_winner[oport]; i++)
        {
            if(requested[oport][i])
            {
                last_port_winner[oport] = i;
                winner_found = true;
                last_winner[oport].port = requesting_inputs[oport][i].port;
                last_winner[oport].ch= requesting_inputs[oport][i].ch;
                last_winner[oport].win_cycle= Simulator::Now();
                return last_winner[oport];
            }
        }
    if(!winner_found)
    {
        _DBG_NOARG("ERROR: RR Cant find port winner" );
        exit(1);
    }


    return last_winner[oport];
}

void
PToPSwitchArbiterVcs::clear_requestor( uint oport, uint inport, uint och)
{
    requested[oport][inport*vcs+och] = false;
    return;
}


bool
PToPSwitchArbiterVcs::is_empty()
{

    for( uint i=0; i<ports; i++)
        for( uint j=0; j<(ports*vcs); j++)
            if(requested[i][j] )
                return false;

    return true;

}

string
PToPSwitchArbiterVcs::toString() const
{
    stringstream str;
    str << "PToPSwitchArbiterVcs: matrix size "
        << "\t requested_qu row_size: " << requested.size();
    if( requested.size())
        str << " col_size: " << requested[0].size()
            ;
    return str.str();
}
#endif   /* ----- #ifndef _ptopswitcharbitervcs_cc_INC  ----- */
