/*
 * =====================================================================================
 *
 *! \brief Filename:  torus.h
 *
 *    Description: This class describes the members and functions for generating the  
 *    visualization for a given topology.
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


#ifndef VISUAL_H_
#define VISUAL_H_

#include <iostream>
#include <fstream>
#include "mesh.h"
#include "torus.h"
#include "topology.h"

using namespace std;

struct link_connection
{
	uint link_id;
	uint source;
	uint destination;
};

class Visual
{
	public:
		Visual();
		Visual(Topology*, int nodes, int links, int grid_size);
		~Visual();
		void create_new_connections();
		void create_graphml();
		vector< link_connection* > new_east_links;
		vector< link_connection* > new_west_links;
		vector< link_connection* > new_north_links;
		vector< link_connection* > new_south_links;

	private:
		Topology* topo_ptr; // pointer to mesh object
		unsigned int nodes; // number of nodes in the topology
		unsigned int links; // number of links in the topology
		unsigned int grid_size; // grid size - required to create links
		link_connection ** link_ptr;
};

#endif /* VISUAL_H_ */

