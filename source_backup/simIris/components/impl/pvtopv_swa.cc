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

#ifndef  _pvtopv_swa_cc_INC
#define  _pvtopv_swa_cc_INC

#include	"pvtopv_swa.h"

PVToPV_swa::PVToPV_swa()
{

}

PVToPV_swa::~PVToPV_swa()
{

}

void
PVToPV_swa::resize(uint port, uint ch)
{
    PORTS = port;
    CHANNELS = ch;
    requested.resize(PORTS*CHANNELS);
    locked.resize(PORTS);
    done.resize(PORTS);
    last_port_winner.resize(PORTS);
    last_ch_winner.resize(PORTS);
    requesting_inputs.resize(PORTS*CHANNELS);
    last_winner.resize(PORTS);
    port_locked.resize(PORTS);

    for ( uint i=0; i<(CHANNELS*PORTS); i++)
    {
        requested[i].resize(PORTS*CHANNELS);
        requesting_inputs[i].resize(PORTS*CHANNELS);
    }

    for ( uint i=0; i<PORTS; i++)
    {
        done[i].resize(CHANNELS);
        locked[i].resize(CHANNELS);
        locked[i].resize(CHANNELS);
        last_port_winner[i].resize(CHANNELS);
        last_ch_winner[i].resize(CHANNELS);
        last_winner[i].resize(CHANNELS);
        port_locked[i] = false;
    }

    for ( uint i=0; i<PORTS*CHANNELS; i++)
        for ( uint j=0; j<PORTS*CHANNELS; j++)
        {
            requested[i][j]=false;
        }

    for ( uint i=0; i<PORTS; i++)
        for ( uint j=0; j<CHANNELS; j++)
        {
            locked[i][j] = false;
            done[i][j] = false;
            last_port_winner[i][j] = 0;
            last_ch_winner[i][j] = 0;
        }
}

bool
PVToPV_swa::is_requested( uint oport, uint och, uint inport, uint inch)
{
    //return requested[oport*CHANNELS+och][inport*CHANNELS+inch] & done [oport][och];
    return locked [oport][och];
}

void
PVToPV_swa::request(uint oport, uint och, uint inport, uint inch )
{
    requested[oport*CHANNELS+och][inport*CHANNELS+inch] = true;
    done[oport][och] = false;
    requesting_inputs[CHANNELS*oport+och][CHANNELS*inport+inch].port = inport;
    requesting_inputs[CHANNELS*oport+och][CHANNELS*inport+inch].ch = inch;

    return;
}

SA_unit
PVToPV_swa::pick_winner( uint oport, uint och)
{
    if(!done[oport][och]) 
    {
        done[oport][och] = true;
        if(locked[oport][och] ) 
            return last_winner[oport][och];

        else
        {
            locked[oport][och]= true;

                /* Now look at contesting input ports on this channel and pick
                 * a winner*/
                bool winner_found = false;
                for( uint i=last_port_winner[oport][och]+1; i<(PORTS*CHANNELS); i++)
                    if(requested[oport*CHANNELS+och][i])
                    {
                        last_port_winner[oport][och] = i;
                        winner_found = true;
                        last_winner[oport][och].port = requesting_inputs[oport*CHANNELS+och][i].port;
                        last_winner[oport][och].ch = requesting_inputs[oport*CHANNELS+och][i].ch;
                        return last_winner[oport][och];
                    }

                if(!winner_found)
                for( uint i=0; i<=last_port_winner[oport][och]; i++)
                {
                    if(requested[oport*CHANNELS+och][i])
                    {
                        last_port_winner[oport][och] = i;
                        winner_found = true;
                        last_winner[oport][och].port = requesting_inputs[oport*CHANNELS+och][i].port;
                        last_winner[oport][och].ch = requesting_inputs[oport*CHANNELS+och][i].ch;
                        return last_winner[oport][och];
                    }
                }
                if(!winner_found)
                {
                    printf("ERROR: Cant find port winner" );
                    exit(1);
                }

            }
    }

    return last_winner[oport][och];
}

void
PVToPV_swa::clear_winner( uint oport, uint och, uint inport, uint inch)
{
    done[oport][och] = false;
    locked[oport][och] = false;

    requested[oport*CHANNELS+och][inport*CHANNELS+inch] = false;
    return;
}

bool
PVToPV_swa::is_empty()
{

    for( uint i=0; i<PORTS*CHANNELS; i++)
        for( uint j=0; j<CHANNELS*PORTS; j++)
            if(requested[i][j] )
            {
                return false;
            }
    return true;
    
}

bool
PVToPV_swa::is_empty_for_ch( uint ch)
{
    for( uint i=0; i<requested[ch].size(); i++)
        if(requested[ch][i] )
            return true;
   return false; 
}

uint
PVToPV_swa::no_requests_ch( uint ch)
{
    uint count = 0;
    for( uint i=0; i<requested[ch].size(); i++)
        if(requested[ch][i] )
        {
            count++;
        }
   return count; 
}

string
PVToPV_swa::toString() const
{
    stringstream str;
    str << "PVToPV_swa: "
        << "\t requested_qu row_size: " << requested.size();
    if( requested.size())
       str << " col_size: " << requested[0].size()
        ;
    return str.str();
}
#endif   /* ----- #ifndef _pvtopv_swa_cc_INC  ----- */
