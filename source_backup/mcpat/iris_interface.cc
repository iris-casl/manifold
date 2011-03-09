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

int interface_simiris(ullint sim_time)
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

    delete p1;
    return 0;
}
