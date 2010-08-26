/*
 * =====================================================================================
 *
 *! \brief Filename:  topology.h
 *
 *    Description: This class describes the abstract base class for a generic topology
 *
 *        Version:  1.0
 *        Created:  07/19/2010 10:01:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author: Sharda Murthi, smurthi3@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * ===================================================================================== 
*/

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include	<iostream>
#include	<fstream>
#include	"../../simIris/components/impl/genericRouterVcs.h"
#include	"../../simIris/components/impl/genericInterfaceVcs.h"
#include	"../../simIris/components/impl/genericTPGVcs.h"
//#include	"../../simIris/components/impl/mcFrontEnd.h"
#include        "../../simIris/components/impl/genericFlatMc.h"
#include        "../../simIris/components/impl/genericLink.h"
#include 	"../../simIris/components/impl/memctrlFE.h"

#include	"../../simIris/data_types/impl/flit.h"
#include	"../../simIris/data_types/impl/highLevelPacket.h"
#include	"../../simIris/components/impl/genericFlatMc.h"
#include	"../../simIris/components/impl/genericRouterAdaptive.h"
#include	"../../simIris/components/impl/genericRouterVcs.h"
#include	"../../util/genericData.h"

#include	<string.h>
#include	<sys/time.h>
#include	<algorithm>
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<time.h>
#include 	<setjmp.h>
#include 	<signal.h>
#include 	<sys/types.h>
#include 	<unistd.h>
#include 	<sys/time.h>
#include 	<sys/io.h>

#include 	"../../zesto/host.h"
#include 	"../../zesto/misc.h"
#include 	"../../zesto/sim.h"
#include 	"../../zesto/machine.h"
#include 	"../../zesto/endian.h"
#include 	"../../zesto/version.h"
#include 	"../../zesto/options.h"
#include 	"../../zesto/stats.h"
//#include 	"../../zesto/loader.h"
#include 	"../../zesto/regs.h"
#include 	"../../zesto/memory.h"
#include 	"../../zesto/thread.h"

class Topology
{
	public:
		virtual void init(uint ports, uint vcs, uint credits, uint buffer_size, uint no_nodes, uint grid_size, uint links) = 0;
		virtual void setup(void) = 0;
		virtual void connect_interface_processor(void) = 0;
		virtual void connect_interface_routers(void) = 0;
		virtual void connect_routers(void) = 0;
		virtual string print_stats(void) = 0;
		Topology() {}
		virtual ~Topology() {}

		unsigned long long int max_sim_time;
		map< uint , uint > east_links;
		map< uint , uint > west_links;
		map< uint , uint > north_links;
		map< uint , uint > south_links;
		vector <Router*> routers;
		vector <Interface*> interfaces;
		vector <Processor*> processors;
		vector <GenericLink*> link_a;
		vector <GenericLink*> link_b;
};

#endif /* TOPOLOGY_H_ */

