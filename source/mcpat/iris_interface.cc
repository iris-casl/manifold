/*
 * =====================================================================================
 *
 *       Filename:  iris_interface.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/01/2010 05:57:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#include "io.h"
#include <iostream>
#include <string>
#include "xmlParser.h"
#include "XML_Parse.h"
#include "processor.h"
#include "globalvar.h"
#include "version.h"
#include "../util/stats.h"


using namespace std;


extern IrisStats* istat;

int interface_simiris(void)
{
    cerr<<"Using McPAT (version "<< VER_MAJOR <<"."<< VER_MINOR
        << " of " << VER_UPDATE << ") for Energy Computation.\n "<<endl;

    char* fname = "config/Xeon.xml";
    ParseXML *p1= new ParseXML();
    p1->parse(fname);
    mcpat::Processor proc(p1);
    proc.init();
    cerr<<"\n\n************* McPat per access POWER STATS *************\n";
//    proc.displayEnergy(2, 5);
    cerr<<"\n\n************* SIMULATION POWER STATS *************\n";
        cerr << "Total Router Dyn Energy: " << istat->compute_total_router_dyn_energy( proc.nocs[0]->rt_power.readOp.dynamic)*1e3<< " mJ"<< endl;
        cerr << "\tBuffer Energy: " << istat->compute_total_buffer_dyn_energy( proc.nocs[0]->router->buffer.rt_power.readOp.dynamic)*1e3<< " mJ"<< endl;
        cerr << "\tArbiter Energy: " << istat->compute_total_arbiter_dyn_energy( proc.nocs[0]->router->arbiter.rt_power.readOp.dynamic)*1e3<< " mJ"<< endl;
        cerr << "\tCrossbar Energy: " << istat->compute_total_crossbar_dyn_energy( proc.nocs[0]->router->crossbar.rt_power.readOp.dynamic)*1e3<< " mJ"<< endl;
    delete p1;
    return 0;
}
