/*
 * =====================================================================================
 *
 *       Filename:  vcaMsgAware.cc
 *
 *    Description:  This is a pseudo-component that does vca in a noc with
 *    multiple msg_classes.
 *    Every msg_class is said to heave a msg_id which indicates the priority.
 *    Vca is done for input packets based on this priority.
 *
 *    Need to add: If a winner includes the highest priority pkt then no more winners for
 *    that output port are picked even if there are empty vcs to ensure that
 *    the packet is never blocked.
 *
 *        Version:  1.0
 *        Created:  10/18/2010 01:03:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#include	"vcaMsgAware.h"

VcaMsgAware::VcaMsgAware()
{
}  /* -----  end of method VcaMsgAware::VcaMsgAware  (constructor)  ----- */

VcaMsgAware::~VcaMsgAware()
{
}  /* -----  end of method VcaMsgAware::~VcaMsgAware  (destructor)  ----- */

void
VcaMsgAware::set_no_msg_classes ( uint no )
{
    no_mclasses = no;
    return ;
}		/* -----  end of method VcaMsgAware::set_no_msg_classes  ----- */

void
VcaMsgAware::init ( uint op, uint vc )
{
    ports = op;
    vcs = vc;
    arbiter.resize(no_mclasses);
    arbiter_arrival_time.resize(no_mclasses);
    available_vcs.resize(no_mclasses);
    winners.resize(no_mclasses);

    for ( uint i=0; i<no_mclasses ;i++ ) {
        arbiter[i].resize(ports);
        arbiter_arrival_time[i].resize(ports);
        winners[i].resize(ports);
        available_vcs[i].resize(ports);
    }

    for ( uint i=0; i<no_mclasses ;i++ ) 
        for ( uint j=0; j<ports;j++ ) 
        {
            arbiter[i][j].clear();
            arbiter_arrival_time[i][j].clear();
            winners[i][j].clear();
            available_vcs[i][j].clear();
            for ( uint k=0; k<(vcs/no_mclasses);k++ ) 
                available_vcs[i][j].push_back(i*no_mclasses+k);
        }
    return ;
}		/* -----  end of method VcaMsgAware::init  ----- */


void
VcaMsgAware::request ( uint op, uint msg_id, uint req_id, ullint at )
{
    /* pool all the requestors based on msg_id and outport. Msg id 0 has
     * highest priority */
    vector<uint>::iterator it;
    it = find (arbiter[msg_id][op].begin(), arbiter[msg_id][op].end(), req_id);
    if( it == arbiter[msg_id][op].end())
    {
        arbiter[msg_id][op].push_back(req_id);
        arbiter_arrival_time[msg_id][op].push_back(at);
    }

#ifdef _DEBUG
    else
        cout	<< "already in requestor list" << req_id << endl;
#endif

    return ;
}		/* -----  end of method VcaMsgAware::request  ----- */


/*!
 *--------------------------------------------------------------------------------------
 *       Class:  VcaMsgAware
 *      Method:  pick_winners
 * Description:  Single vca unit per router. picks all possible winners in a
 * given invocation.
 * returns the number of winners that were picked in a given cycle
 *--------------------------------------------------------------------------------------
 */
uint
VcaMsgAware::pick_winners ()
{
    uint winner_count = 0;
    /* Msg id 0 has highest priority. Sort pooled mssgs on arrival time and
     * assign vcs. return the number of msgs that got allocated a vc in this
     * invocation of the method. */
    for ( uint i=0; i<no_mclasses ; i++ ) 
        for ( uint j=0; j<ports ; j++ ) 
        {
            ullint min_t = -1;
            vector<ullint> sorted_msgs;
            if(available_vcs[i][j].size())
            {
                uint msg_index  = 999;
                for ( uint k=0; k<arbiter[i][j].size();k++ ) 
                {
                    if ( arbiter_arrival_time[i][j][k] < min_t )
                        msg_index = arbiter[i][j][k];
                }

                if(msg_index != 999 )
                {
                    uint winner_oc = available_vcs[i][j].front();
                    available_vcs[i][j].pop_front();
                    winners[i][j].insert(winners[i][j].end(),make_pair(msg_index,winner_oc));
                    vector<uint>::iterator it = find (arbiter[i][j].begin(), arbiter[i][j].end(), msg_index);
                    arbiter[i][j].erase(it);
                    winner_count++;
                }
            }

        }

    return winner_count;
}		/* -----  end of method VcaMsgAware::pick_winners  ----- */

uint
VcaMsgAware::get_no_requestors ( void )
{
    uint count = 0;
    for ( uint i=0; i<no_mclasses ; i++ ) 
        for ( uint j=0; j<ports ; j++ ) 
            count += arbiter[i][j].size();

    return count ;
}		/* -----  end of method VcaMsgAware::get_no_requestors  ----- */

/*  Assumes tht the input requestor was a winner for one op and one mc only */
void
VcaMsgAware::clear_winner ( uint winner, uint oc)
{
    for ( uint i=0; i<no_mclasses ; i++ ) 
        for ( uint j=0; j<ports ; j++ ){ 
            map<uint,uint>::iterator it = winners[i][j].find(winner);
            if ( it != winners[i][j].end() )
            {
                winners[i][j].erase(it);
                available_vcs[i][j].push_back(oc);
                break;
            }
        }

    return ;
}		/* -----  end of method VcaMsgAware::clear_winner  ----- */

