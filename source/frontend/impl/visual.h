/*
 * =====================================================================================
 *
 *! \brief Filename:  visual.h
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
#include <cstring>

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
		Visual(Topology*, int nodes, int links, int grid_size, string, string, int , int, double, double, int, int, double);
		~Visual();
		void create_new_connections();
		void create_graphml_mesh();
		void create_graphml_torus();
		void create_graphml();
		void add_entry(int i, int j, int x, int y, int interface_id, int processor_id );
		vector< link_connection* > new_east_links;
		vector< link_connection* > new_west_links;
		vector< link_connection* > new_north_links;
		vector< link_connection* > new_south_links;

		ofstream graphml_file;
	
		/* Final stats */
		int tot_link_utln, tot_link_cr_utln, total_pkts, total_flits;
		double bytes_del, av_bw, sim_time;

	private:
		Topology* topo_ptr; // pointer to mesh object
		unsigned int nodes; // number of nodes in the topology
		unsigned int links; // number of links in the topology
		unsigned int grid_size; // grid size - required to create links
		link_connection ** link_ptr;
		string type; // type of network - required to set grid layout
		string rtr_type;
};

#endif /* VISUAL_H_ */

