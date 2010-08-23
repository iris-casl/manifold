/*
 * =====================================================================================
 *
 *! \brief Filename:  mesh.h
 *
 *    Description: The class defines functions for a generic k-ary 2D mesh
 *    with the network being a direct network of size n=k^2.
 *    The links have the following naming convention 
 *    
 *    links going left to right are a links
 *    links going from right to left are b links
 *    links going downwards are a links
 *    links going upwards are b links
 *
 *    Router ports
 *    port 0: Connects to interface
 *    port 1: Connects to direction east
 *    port 2: Connects to direction west
 *    port 3: Connects to direction north
 *    port 4: Connects to direction south
 *
 *              a links
 *              ----> R0 ----> R1 ---->
 *              <---  |^ <---  |^   <---
 *              blinks
 *                    ||       ||
 *                    v|       v| 
 *              ----> R2 ----> R3 ---->
 *                  a  |^  b     
 *                     ||   
 *                  l  ||  l
 *                  i  V|  i
 *                  n      n
 *                  k      k
 *                  s      s
 *
 *        Version:  1.0
 *        Created:  05/05/2010 12:01:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _mesh_h_INC
#define  _mesh_h_INC

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
extern vector<unsigned int> mc_positions;

class Mesh
{
    public:
        Mesh ();
        ~Mesh ();
        vector <Router*> routers;
        vector <Interface*> interfaces;
        vector <Processor*> processors;
        vector <GenericLink*> link_a;
        vector <GenericLink*> link_b;

        void init(uint ports, uint vcs, uint credits, uint buffer_size, uint no_nodes, uint grid_size, uint links);
        void setup(void);
        void connect_interface_processor(void);
        void connect_interface_routers(void);
        void connect_routers(void);
        string print_stats(void);
        unsigned long long int max_sim_time;
        void set_max_phy_link_bits ( uint a);

    protected:

    private:
        uint ports;
        uint vcs;
        uint credits;
        uint buffer_size;
        uint no_nodes;
        uint links;
        uint grid_size;

}; /* -----  end of class Mesh  ----- */

#endif   /* ----- #ifndef _mesh_h_INC  ----- */
