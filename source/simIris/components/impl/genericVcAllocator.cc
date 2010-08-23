/*
 * =====================================================================================
 *
 *       Filename:  genericVcAllocator.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/21/2010 03:22:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericvcallocator_cc_INC
#define  _genericvcallocator_cc_INC

#include "genericVcAllocator.h"
using namespace std;

GenericVcAllocator::GenericVcAllocator()
{
    name = "genericVcAllocator";
}

GenericVcAllocator::~GenericVcAllocator()
{

}

void
GenericVcAllocator::setup( uint p, uint v )
{
    ports = p;
    vcs = v;
    current_winners.resize(ports*vcs);
    done.resize(ports*vcs);
    locked.resize(ports*vcs);
    last_winner.resize(ports*vcs);

    requested.resize(ports*vcs);
    requestor_inport.resize(ports*vcs);
    last_inport_winner.resize(ports*vcs);

    for ( uint i=0; i<(vcs*ports); i++ )
    {
        requested[i].resize( ports );
        requestor_inport[i].resize(ports);
        last_inport_winner[i].resize(ports);

        done[i] = false;
        locked[i] = false;
        last_winner[i] = 0;
        current_winners[i] = -1;
    }

    for ( uint i=0; i<(ports*vcs); i++ )
        for ( uint j=0; j<ports; j++ )
        {
            requested[i][j] = false;
            last_inport_winner[i][j] = 0;
            requestor_inport[i][j].resize(vcs);
        }

    for ( uint i=0; i<(ports*vcs); i++ )
        for ( uint j=0; j<ports; j++ )
            for ( uint k=0; k<vcs; k++)
                requestor_inport[i][j][k] = false;
}

bool
GenericVcAllocator::request( uint op , uint ovc, uint inp, uint invc )
{ 
/* 
    cout << "BEFORE " << endl;
    if( Simulator::Now() > 33 )
    {
        cout << endl << " VCA PICK WINNER REQ_IN MATRIX\n";
        for ( uint i=0; i<(ports*vcs); i++)
        {
            for ( uint j=0; j<ports; j++)
            {
                for ( uint k=0; k<vcs; k++)
                    cout << " " << requestor_inport[i][j][k];
                cout << "\t";
            }
            cout << "\n";
        }

        cout << "\n REQUEST MATRIX\n";
        for ( uint i=0; i<(ports*vcs); i++)
        {
            for ( uint j=0; j<ports; j++)
                cout << requested[i][j] << " ";
            cout << "\n";
        }
    }
 * */
    if( !locked[op*vcs+ovc] )
    {
        requestor_inport[op*vcs+ovc][inp][invc] = true;
        return true;
    }
    else
        return false;
//    done[op*vcs+ovc] = false;
//    locked[op*vcs+ovc] = false;
}

bool
GenericVcAllocator::is_empty()
{
    for ( uint i=0; i<(ports*vcs); i++ )
        for ( uint j=0; j<ports; j++)
            for ( uint k=0; k<vcs; k++)
           if ( requestor_inport[i][j][k] )
               return false;

    return true;
}

bool
GenericVcAllocator::is_empty(uint i)
{
    for ( uint j=0; j<ports; j++)
        for ( uint k=0; k<vcs; k++)
           if ( requestor_inport[i][j][k] )
               return false;

    return true;
}

void
GenericVcAllocator::pick_winner()
{
    if( !is_empty())
    {
    set_request_matrix();
    /* 
    if( Simulator::Now() > 110 )
    {
    cout << endl << " VCA PICK WINNER REQ_IN MATRIX\n";
    for ( uint i=0; i<(ports*vcs); i++)
    {
        for ( uint j=0; j<ports; j++)
        {
            for ( uint k=0; k<vcs; k++)
                cout << " " << requestor_inport[i][j][k];
            cout << "\t";
        }
            cout << "\n";
    }

            cout << "\n REQUEST MATRIX\n";
            for ( uint i=0; i<(ports*vcs); i++)
            {
            for ( uint j=0; j<ports; j++)
            cout << requested[i][j] << " ";
            cout << "\n";
            }
            }
     * */
    do_round_robin_arbitration();
    /* 
       if ( Simulator::Now()>110)
       {
       for ( uint i=0; i<current_winners.size(); i++)
       if ( current_winners[i] != -1 )
       cout << "\t" << i << " " << current_winners[i];
       }
     * */
    }
    return;
}

void
GenericVcAllocator::set_request_matrix()
{
    for ( uint i=0; i<(ports*vcs); i++ )
    {
        if( !locked[i])
        {
            for ( uint j=0; j<ports; j++)
            {
                requested[i][j] = false;
                for ( uint k=last_inport_winner[i][j]+1; k<vcs; k++)
                    if ( requestor_inport[i][j][k])
                    {
                        requested[i][j] = true;
                        last_inport_winner[i][j] = k;
                        break;
                    }

                if( !requested[i][j])
                {
                    for ( uint k=0 ; k<=last_inport_winner[i][j]; k++)
                    {
                        if ( requestor_inport[i][j][k])
                        {
                            requested[i][j] = true;
                            last_inport_winner[i][j] = k;
                            break;
                        }
                    }
                }
            }
        }
        /* 
           else
           if( Simulator::Now() > 130 )
           cout << endl << "locked on " << i << " by " << current_winners[i] ;
           else
           {
           for ( uint k=0; k<ports; k++)
           requested[i][k] = false;
           }
         * */
    }

    return;

}

void
GenericVcAllocator::do_round_robin_arbitration()
{
    for ( uint aa=0; aa<(ports*vcs); aa++)
    {
        if ( !done[aa] )
        {
            if (!locked[aa])
            {
                current_winners[aa] = -1;

                bool found_winner = false;
                for ( uint i=last_winner[aa]+1; i <ports; i++ )
                    if ( requested[aa][i] )
                    {
                        found_winner = true;
                        last_winner[aa] = i;
                        current_winners[aa] = i*vcs+last_inport_winner[aa][i];
                        done[aa] = true;
                        locked[aa] = true;
                        break;
                    }

                if ( !found_winner )
                    for ( uint i=0; i<=last_winner[aa]; i++ )
                    {
                        if( requested [aa][i] )
                        {
                            found_winner = true;
                            last_winner[aa] = i;
                            current_winners[aa] = i*vcs+last_inport_winner[aa][i];
                            done[aa] = true;
                            locked[aa] = true;
                            break;
                        }
                    }
#ifdef _DEBUG
                if (!found_winner )
                    cout << " VCA: no winner for this port " << (int)aa/vcs<< " vc: " << aa%vcs<< endl;
#endif
            }
        }
    }

    return ;

}

void
GenericVcAllocator::clear_winner(uint op, uint ovc, uint ip, uint ivc)
{
    if( current_winners[op*vcs+ovc] != ip*vcs+ivc )
    {
        _DBG("Error Clear incorrect winner! winner is %d requesting %d",
             current_winners[op*vcs+ovc], ip*vcs+ivc );
        exit(1);
    }

    requested[op*vcs+ovc][ip] = false;
    requestor_inport[op*vcs+ovc][ip][ivc] = false;
    done[op*vcs+ovc] = false;
    locked[op*vcs+ovc] = false;
    current_winners[op*vcs+ovc] = -1;
/* 
    if( Simulator::Now() > 33 )
    {
        cout << endl << " VCA PICK WINNER REQ_IN MATRIX\n";
        for ( uint i=0; i<(ports*vcs); i++)
        {
            for ( uint j=0; j<ports; j++)
            {
                for ( uint k=0; k<vcs; k++)
                    cout << " " << requestor_inport[i][j][k];
                cout << "\t";
            }
            cout << "\n";
        }

        cout << "\n REQUEST MATRIX\n";
        for ( uint i=0; i<(ports*vcs); i++)
        {
            for ( uint j=0; j<ports; j++)
                cout << requested[i][j] << " ";
            cout << "\n";
        }
    }
 * */
}

bool
GenericVcAllocator::is_requested( uint op, uint ovc, uint ip, uint ivc )
{
    return requestor_inport[op*vcs+ovc][ip][ivc];
}

void
GenericVcAllocator::squash_requests( uint op, uint ovc, uint ip, uint ivc)
{
    for ( uint i=0; i<current_winners.size(); i++)
        if (current_winners[i] == (ip*vcs+ivc) && i!=op*vcs+ovc) 
        {
            /* Entry needs to be squashed to allow for other requestors */
//            cout << " \n Sq: " << i ;
            requestor_inport[i][ip][ivc] = false;
            requested[i][ip] = false;
            done[i] = false;
            locked[i] = false;
            current_winners[i] = -1;
        }

    bool is_there_winner = false;
    for ( uint i=op*vcs; i<(op*vcs+vcs); i++)
        if( current_winners[i] == ip*vcs+ivc )
            is_there_winner = true;
    if(is_there_winner)
        for ( uint i=op*vcs; i<(op*vcs+vcs); i++)
            if( current_winners[i] != ip*vcs+ivc )
                requestor_inport[i][ip][ivc] = false;
    /* 
    if( Simulator::Now() > 42 )
    {
        cout << endl << " VCA PICK WINNER REQ_IN MATRIX\n";
        for ( uint i=0; i<(ports*vcs); i++)
        {
            for ( uint j=0; j<ports; j++)
            {
                for ( uint k=0; k<vcs; k++)
                    cout << " " << requestor_inport[i][j][k];
                cout << "\t";
            }
            cout << "\n";
        }

        cout << "\n REQUEST MATRIX\n";
        for ( uint i=0; i<(ports*vcs); i++)
        {
            for ( uint j=0; j<ports; j++)
                cout << requested[i][j] << " ";
            cout << "\n";
        }
    }
     * */
}
#endif   /* ----- #ifndef _genericvcallocator_cc_INC  ----- */

