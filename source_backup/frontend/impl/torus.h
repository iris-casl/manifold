/*
 * =====================================================================================
 *
 *! \brief Filename:  torus.h
 *
 *    Description: This class describes the class for a torus topology -  a torus is similar to a 
 *    mesh, except that the edge nodes are also connected.
 *	
 *        Version:  1.0
 *        Created:  07/19/2010 10:10:55 AM
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author: Sharda Murthi, smurthi3@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * ===================================================================================== 
*/


#ifndef TORUS_H_
#define TORUS_H_

#include "topology.h"

extern vector<unsigned int> mc_positions;
extern uint no_of_cores;
extern uint concentration;

class Torus : public Topology
{
    public:
        Torus ();
        ~Torus ();

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

}; /* -----  end of class Torus  ----- */

#endif /* TORUS_H_ */

