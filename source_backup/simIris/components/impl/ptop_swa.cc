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

#ifndef  _ptopswitcharbiter_cc_INC
#define  _ptopswitcharbiter_cc_INC

#include	"ptop_swa.h"

PToPSwitchArbiter::PToPSwitchArbiter()
{
    name = "swa";
}

PToPSwitchArbiter::~PToPSwitchArbiter()
{
}

void
PToPSwitchArbiter::resize(uint port)
{
    PORTS = port;
    requested.resize(PORTS);
    priority_reqs.resize(PORTS);
    locked.resize(PORTS);
    done.resize(PORTS);
    last_port_winner.resize(PORTS);
    requesting_inputs.resize(PORTS);
    last_winner.resize(PORTS);
    port_locked.resize(PORTS);

    for ( uint i=0; i<PORTS; i++)
    {
        requested[i].resize(PORTS);
        priority_reqs[i].resize(PORTS);
        requesting_inputs[i].resize(PORTS);
    }

    for ( uint i=0; i<PORTS; i++)
        for ( uint j=0; j<PORTS; j++)
        {
            requested[i][j]=false;
            priority_reqs[i][j]=false;
        }

    for ( uint i=0; i<PORTS; i++)
        {
            locked[i] = false;
            done[i] = false;
            last_port_winner[i] = 0;
        }

}

bool
PToPSwitchArbiter::is_requested( uint oport, uint inport )
{
    return requested[oport][inport];
}

void
PToPSwitchArbiter::request(uint oport, uint inport )
{
    requested[oport][inport] = true;
    done[oport] = false;
    requesting_inputs[oport][inport].port = inport;
    requesting_inputs[oport][inport].in_time = (ullint)Simulator::Now();
    return;
}

void
PToPSwitchArbiter::request(uint oport, uint inport, message_class m )
{
    requested[oport][inport] = true;
    done[oport] = false;
    requesting_inputs[oport][inport].port = inport;
    requesting_inputs[oport][inport].in_time = (ullint)Simulator::Now();
    if( m == priority_msg_type)
        priority_reqs[oport][inport] = true;
    else
        priority_reqs[oport][inport] = false;
    return;
}

SA_unit
PToPSwitchArbiter::pick_winner( uint oport)
{
    switch ( sw_arbitration )
    {
        case ROUND_ROBIN:
            return do_round_robin_arbitration(oport);
            break;
        case ROUND_ROBIN_PRIORITY:
            return do_priority_round_robin_arbitration(oport);
            break;
        case FCFS:
            return do_fcfs_arbitration(oport);
            break;
        default:
            cout << " ERROR: Invalid switch arbitration" << endl;
            break;
    }

}

SA_unit
PToPSwitchArbiter::do_round_robin_arbitration( uint oport)
{
    if(!done[oport]) 
    {
        done[oport] = true;
        if(locked[oport] ) 
            return last_winner[oport];
/* 
        else if ( requested[oport][0])
        {
            last_port_winner[oport] = 0;
            last_winner[oport].port = requesting_inputs[oport][0].port;
        }
*/
        else
        {
            locked[oport]= true;

                /* Now look at contesting input ports on this channel and pick
                 * a winner*/
                bool winner_found = false;
                for( uint i=last_port_winner[oport]+1; i<(PORTS); i++)
                {
                    if(requested[oport][i])
                    {
                        last_port_winner[oport] = i;
                        winner_found = true;
                        last_winner[oport].port = requesting_inputs[oport][i].port;
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
                        return last_winner[oport];
                    }
                }
                if(!winner_found)
                {
                    _DBG_NOARG("ERROR: RR Cant find port winner" );
                    exit(1);
                }

            }
    }

    return last_winner[oport];
}

SA_unit
PToPSwitchArbiter::do_priority_round_robin_arbitration( uint oport)
{
    if(!done[oport]) 
    {
        done[oport] = true;
        if(locked[oport] ) 
            return last_winner[oport];

        else
        {
            locked[oport]= true;

            /* Use priority vector to modify requested vector */
            for ( uint i=0; i<PORTS; i++)
            {
                bool set_priority = false;
                for ( uint j=0; j<PORTS; j++)
                    if ( priority_reqs[i][j])
                    {
                        set_priority = true;
                        break;
                    }
                        
                if ( set_priority )
                    for ( uint j=0; j<PORTS; j++)
                    {
                        requested[i][j] = priority_reqs[i][j];
                    }
            }

                /* Now look at contesting input ports on this channel and pick
                 * a winner*/
                bool winner_found = false;
                for( uint i=last_port_winner[oport]+1; i<(PORTS); i++)
                {
                    if(requested[oport][i])
                    {
                        last_port_winner[oport] = i;
                        winner_found = true;
                        last_winner[oport].port = requesting_inputs[oport][i].port;
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
                        return last_winner[oport];
                    }
                }
                if(!winner_found)
                {
                    _DBG_NOARG("ERROR: RR Cant find port winner" );
                    exit(1);
                }

            }
    }

    return last_winner[oport];
}

SA_unit
PToPSwitchArbiter::do_fcfs_arbitration( uint oport)
{
    if(!done[oport]) 
    {
        done[oport] = true;
        if(locked[oport] ) 
            return last_winner[oport];

        else
        {
            locked[oport]= true;

            /* Now look at contesting input ports on this channel and pick
            * a winner*/
            uint max_time_waiting = 0;
            uint winner_inport = -1;
            for( uint i=0; i<PORTS; i++)
            {
                if(requested[oport][i])
                {
                    if( (Simulator::Now() - requesting_inputs[oport][i].in_time) > max_time_waiting )
                    {
                        max_time_waiting = (ullint)Simulator::Now() - requesting_inputs[oport][i].in_time;
                        winner_inport = i;
                    }
                }
            }
            
                if(winner_inport == -1)
                {
                    _DBG_NOARG("ERROR: FCFS Cant find port winner" );
                    exit(1);
                }

            last_winner[oport].port = requesting_inputs[oport][winner_inport].port;
            return last_winner[oport];
        }
    }

    return last_winner[oport];
}


void
PToPSwitchArbiter::clear_winner( uint oport, uint inport)
{
    done[oport]= false;
    locked[oport] = false;

    requested[oport][inport] = false;
    priority_reqs[oport][inport] = false;
    return;
}

void
PToPSwitchArbiter::clear_requested( uint oport, uint inport)
{
    requested[oport][inport] = false;
    priority_reqs[oport][inport] = false;
    return;
}

bool
PToPSwitchArbiter::is_empty()
{

    for( uint i=0; i<PORTS; i++)
        for( uint j=0; j<PORTS; j++)
            if(requested[i][j] )
                return false;

    return true;
    
}

string
PToPSwitchArbiter::toString() const
{
    stringstream str;
    str << "PToPSwitchArbiter: matrix size "
        << "\t requested_qu row_size: " << requested.size();
    if( requested.size())
       str << " col_size: " << requested[0].size()
        ;
    return str.str();
}
#endif   /* ----- #ifndef _ptopswitcharbiter_cc_INC  ----- */
