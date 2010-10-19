/*
 * =====================================================================================
 *
 *       Filename:  stats.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/01/2010 02:36:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */


#ifndef  _stats_H_INC
#define  _stats_H_INC

#include	<vector>
#include	<iostream>
using namespace std;

typedef unsigned long long int ullint;
typedef unsigned int uint;
extern uint no_nodes, links, vcs;

/*
 * =====================================================================================
 *        Class: StatRouter
 *  Description:  
 * =====================================================================================
 */
class StatRouter
{
    public:
        /* ====================  LIFECYCLE     ======================================= */
        StatRouter ();                             /* constructor */
        ~StatRouter(){}
        ullint ib_cycles;
        ullint rc_cycles;
        ullint vca_cycles;
        ullint sa_cycles;
        ullint st_cycles;


}; /* -----  end of class StatRouter  ----- */


class StatLink
{
    public:
        StatLink();
        ~StatLink(){}

        ullint credits_transferred;
        ullint flits_transferred;
};

class IrisStats 
{
    private:

    public:
        vector<StatRouter*> stat_router;
        vector<StatLink*> stat_link;
        IrisStats();
        ~IrisStats();
        void init(void);
        double compute_total_router_dyn_energy(double xx);
        double compute_total_buffer_dyn_energy(double en);
        double compute_total_arbiter_dyn_energy( double en );
        double compute_total_vc_arbiter_dyn_energy( double en );
        double compute_total_crossbar_dyn_energy( double en );
        double compute_total_link_dyn_energy(double en );
        ullint get_total_ib_cycles(void);
        ullint get_total_rc_cycles(void);
        ullint get_total_vca_cycles(void);
        ullint get_total_sa_cycles(void);
        ullint get_total_st_cycles(void);
        ullint get_total_credits_passed(void);
        ullint get_total_flits_passed(void);
};

#endif   /* ----- #ifndef _stats_H_INC  ----- */
