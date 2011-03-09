/*
 * =====================================================================================
 *
 *       Filename:  stats.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/01/2010 02:37:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _STATS_CC_INC
#define  _STATS_CC_INC

#include	"stats.h"
using namespace std;

StatRouter::StatRouter()
{
    ib_cycles = 0;
    rc_cycles = 0;
    vca_cycles = 0;
    sa_cycles = 0;
    st_cycles = 0;
}

StatLink::StatLink()
{
    credits_transferred = 0;
    flits_transferred = 0;
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  IrisStats
 *      Method:  IrisStats
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
IrisStats::IrisStats ()
{
    stat_router.clear();
    stat_link.clear();
}  /* -----  end of method IrisStats::IrisStats  (constructor)  ----- */

IrisStats::~IrisStats ()
{
}		/* -----  end of method IrisStats::~IrisStats  ----- */

void
IrisStats::init( void )
{
    for( uint i=0; i<no_nodes; i++)
        stat_router.push_back( new StatRouter() );
    for( uint i=0; i<2*links; i++)
        stat_link.push_back( new StatLink() );

}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  compute_total_buffer_dyn_energy
 *  Description:  
 * =====================================================================================
 */
double
IrisStats::compute_total_buffer_dyn_energy ( double en )
{
    double total_en = 0.0;
    for( uint i=0; i<no_nodes; i++)
        total_en += stat_router[i]->ib_cycles * en;

    return total_en;
}		/* -----  end of method IrisStats::compute_total_buffer_dyn_energy  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  compute_total_router_dyn_energy
 *  Description:  
 * =====================================================================================
 */
double
IrisStats::compute_total_router_dyn_energy ( double en )
{
    double total_en = 0.0;
    for( uint i=0; i<no_nodes; i++)
        total_en += (stat_router[i]->ib_cycles + stat_router[i]->rc_cycles + stat_router[i]->vca_cycles + stat_router[i]->sa_cycles + stat_router[i]->st_cycles)* en;

    return total_en;
}		/* -----  end of method IrisStats::compute_total_router_dyn_energy  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  compute_total_arbiter_dyn_energy
 *  Description:  
 * =====================================================================================
 */
double
IrisStats::compute_total_arbiter_dyn_energy ( double en )
{
    double total_en = 0.0;
    for( uint i=0; i<no_nodes; i++)
        total_en += (stat_router[i]->vca_cycles + stat_router[i]->sa_cycles/vcs )* en;

    return total_en;
}		/* -----  end of method IrisStats::compute_total_arbiter_dyn_energy  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  compute_total_vc_arbiter_dyn_energy
 *  Description:  
 * =====================================================================================
 */
double
IrisStats::compute_total_vc_arbiter_dyn_energy ( double en )
{
    double total_en = 0.0;
    for( uint i=0; i<no_nodes; i++)
        total_en += (stat_router[i]->vca_cycles)* en;

    return total_en;
}		/* -----  end of method IrisStats::compute_total_vc_arbiter_dyn_energy  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  compute_total_crossbar_dyn_energy
 *  Description:  
 * =====================================================================================
 */
double
IrisStats::compute_total_crossbar_dyn_energy ( double en )
{
    double total_en = 0.0;
    for( uint i=0; i<no_nodes; i++)
        total_en += (stat_router[i]->st_cycles * en);

    return total_en;
}		/* -----  end of method IrisStats::compute_total_crossbar_dyn_energy  ----- */

/*! 
 * ===  FUNCTION  ======================================================================
 *         Name:  compute_total_link_dyn_energy
 *  Description:  
 * =====================================================================================
 */
double
IrisStats::compute_total_link_dyn_energy ( double en )
{
    double total_en = 0.0;
    for( uint i=0; i<2*links; i++)
        total_en += (stat_link[i]->credits_transferred + stat_link[i]->flits_transferred)* en;

    return total_en;
}		/* -----  end of method IrisStats::compute_total_link_dyn_energy  ----- */

ullint
IrisStats::get_total_ib_cycles ( void )
{
    ullint total = 0;
    for( uint i=0; i<no_nodes; i++)
        total += stat_router[i]->ib_cycles;

    return total;
}

ullint
IrisStats::get_total_rc_cycles ( void )
{
    ullint total = 0;
    for( uint i=0; i<no_nodes; i++)
        total += stat_router[i]->rc_cycles;

    return total;
}

ullint
IrisStats::get_total_vca_cycles ( void )
{
    ullint total = 0;
    for( uint i=0; i<no_nodes; i++)
        total += stat_router[i]->vca_cycles;

    return total;
}

ullint
IrisStats::get_total_sa_cycles ( void )
{
    ullint total = 0;
    for( uint i=0; i<no_nodes; i++)
        total += stat_router[i]->sa_cycles;

    return total;
}

ullint
IrisStats::get_total_st_cycles ( void )
{
    ullint total = 0;
    for( uint i=0; i<no_nodes; i++)
        total += stat_router[i]->st_cycles;

    return total;
}

ullint
IrisStats::get_total_flits_passed( void )
{
    ullint total = 0;
    for( uint i=0; i<2*links; i++)
        total += stat_link[i]->flits_transferred;

    return total;
}

ullint
IrisStats::get_total_credits_passed( void )
{
    ullint total = 0;
    for( uint i=0; i<2*links; i++)
        total += stat_link[i]->credits_transferred;

    return total;
}

#endif   /* ----- #ifndef _STATS_CC_INC  ----- */
