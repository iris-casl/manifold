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
 *    port 1: Connects to direction east (p1)
 *    port 2: Connects to direction west (p2)
 *    port 3: Connects to direction north (p3)
 *    port 4: Connects to direction south (p4)
 *
 *              a links
 *                    p3
 *              ----> R0 p2----> R1 ---->
 *              <---p1  |^ <---  |^   <---
 *              blinks p4
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

#include 	"topology.h"


#ifdef USE_ZESTO
#include 	"../../zesto/host.h"
#include 	"../../zesto/misc.h"
#include 	"../../zesto/sim.h"
#include 	"../../zesto/machine.h"
#include 	"../../zesto/endian.h"
#include 	"../../zesto/version.h"
#include 	"../../zesto/options.h"
#include 	"../../zesto/stats.h"
#include 	"../../zesto/regs.h"
#include 	"../../zesto/memory.h"
#include 	"../../zesto/thread.h"
#endif

extern vector<unsigned int> mc_positions;
extern uint no_of_cores;
extern uint concentration;

class Mesh : public Topology
{
    public:
        Mesh ();
        ~Mesh ();

        void init(uint ports, uint vcs, uint credits, uint buffer_size, uint no_nodes, uint grid_size, uint links);
        void setup(void);
        void connect_interface_processor(void);
        void connect_interface_routers(void);
        void connect_routers(void);
        string print_stats(void);
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
