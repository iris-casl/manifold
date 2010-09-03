/*
 * =====================================================================================
 *
 *       Filename: Visual.cc
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/28/2010 3:31:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sharda Murthi, smurthi3@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _visual_cc_INC
#define  _visual_cc_INC
#define DEBUG 0

#include "visual.h"

Visual::Visual()
{
	topo_ptr = NULL;
	nodes = 0;
	links = 0;
}

Visual::Visual(Topology* topo, int n, int l, int g): topo_ptr(topo), nodes(n), links(l), grid_size(g)
{
	link_ptr = new link_connection *[links]; // declaring an array of pointers
	for (uint l = 0; l < links; l++)
		link_ptr[l] = new link_connection;

#ifdef DEBUG
	std::cout << "In Parameterized Visual ctor\n";
	cout << "Nodes = " << nodes << "\t" << "Links = " << links << "\n";
#endif
}

Visual::~Visual()
{
	for ( uint l = 0; l < links; l++)
		delete[] link_ptr[l];
#ifdef DEBUG
	std::cout << "In Visual dtor\n";
#endif
}

void Visual::create_new_connections() /* Function to get links as {link_id, <source, destination>} format */
{
	map<uint , uint >::iterator it1;
	map<uint , uint >::iterator it2;

	it1 = topo_ptr->east_links.begin();
	uint k = 0;

	/* For East and West connections */
	for ( ; it1 != topo_ptr->east_links.end(); it1++ )
	{
#ifdef DEBUG
		cout << "in for loop east-west " << (*it1).second << " \n";
#endif
		it2 = topo_ptr->west_links.begin();
		for (; it2 != topo_ptr->west_links.end(); it2++ ) // we got a match
		{
			if ( (*it1).second == (*it2).second )
			{
#ifdef DEBUG
				cout << "source = " << (*it2).first << "\n";
				cout << "destination = " << (*it1).first << "\n";
#endif
				link_ptr[k]->link_id = (*it1).second;
				link_ptr[k]->source = (*it2).first;
				link_ptr[k]->destination = (*it1).first;

				new_east_links.push_back(link_ptr[k]);
				k++;

#ifdef DEBUG
				cout << "source = " << (*it1).first << "\n";
				cout << "destination = " << (*it2).first << "\n";
#endif
				link_ptr[k]->link_id = (*it1).second + 5000;
				link_ptr[k]->source = (*it1).first;
				link_ptr[k]->destination = (*it2).first;

				new_west_links.push_back(link_ptr[k]);
				k++;
			}
		}
	}

	/* For North and South connections */

	it1 = topo_ptr->north_links.begin();

	for ( ; it1 != topo_ptr->north_links.end(); it1++ )
	{
#ifdef DEBUG
		cout << "in for loop north-south " << (*it1).second << " \n";
#endif
		it2 = topo_ptr->south_links.begin();
		for (; it2 != topo_ptr->south_links.end(); it2++ ) // we got a match
		{
			if ( (*it1).second == (*it2).second )
			{
#ifdef DEBUG
				cout << "source = " << (*it2).first << "\n";
				cout << "destination = " << (*it1).first << "\n";
#endif
				link_ptr[k]->link_id = (*it1).second;
				link_ptr[k]->source = (*it2).first;
				link_ptr[k]->destination = (*it1).first;

				new_east_links.push_back(link_ptr[k]);
				k++;

#ifdef DEBUG
				cout << "source = " << (*it1).first << "\n";
				cout << "destination = " << (*it2).first << "\n";
#endif
				link_ptr[k]->link_id = (*it1).second + 5000;
				link_ptr[k]->source = (*it1).first;
				link_ptr[k]->destination = (*it2).first;

				new_west_links.push_back(link_ptr[k]);
				k++;
			}
		}
	}

}

void Visual::create_graphml()
{
#ifdef DEBUG
	std::cout << "Create graphml file function\n";
	cout << "cores and interfaces = " << concentration << "\t" << no_of_cores << "\n";
#endif
	ofstream graphml_file;
	graphml_file.open("output.graphml");

    vector<uint>::iterator itr;

	graphml_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n";
	graphml_file << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"" << "\n";
	graphml_file << "\t" << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" << "\n";
	graphml_file << "\t" << "xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns" << "\n";
	graphml_file << "\t" << "http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">" << "\n";
	graphml_file << "\t" << "<graph id=\"G\" edgedefault=\"directed\">" << "\n";

	graphml_file << "<!-- data schema -->\n";
	graphml_file << "<key id=\"name\" for=\"node\" attr.name=\"name\" attr.type=\"string\"/>" << "\n";
	graphml_file << "<key id=\"type\" for=\"node\" attr.name=\"type\" attr.type=\"string\"/>" << "\n";
	graphml_file << "<key id=\"name\" for=\"edge\" attr.name=\"name\" attr.type=\"string\"/>" << "\n";

	unsigned int i=0;
	static unsigned int j = 0; // edge count
	static unsigned int node_count = 0; // node count
	bool flag = false;

	for (; i < nodes; i++)
	{
		flag = false;
		graphml_file << "\t" << "<node id=\"n" << i << "\">" << "\n";
		graphml_file << "\t \t" << "<data key=\"name\">" << i << "</data>\"" << "\n";
		for (itr = mc_positions.begin(); itr!=mc_positions.end(); itr++)
	    {
			if ( i == *itr )
			{
				graphml_file << "\t \t" << "<data key=\"type\">mc</data>\"" << "\n";
				flag = true;
			}
	    }
		if (flag == false )
			graphml_file << "\t \t" << "<data key=\"type\">router</data>\"" << "\n";
		graphml_file << "\t" << "</node>" << "\n";
	}

	node_count = nodes;

	if ( grid_size == 2 )
	{
		graphml_file << "\t" << "<edge source=\"n" << 0 << "\"" << " target=\"n" << 1 << "\">" << "\n";
		graphml_file << "\t \t" << "<data key=\"name\">" << j << "</data>" << "\n";
		graphml_file << "\t" << "</edge>" << "\n";

		graphml_file << "\t" << "<edge source=\"n" << 1 << "\"" << " target=\"n" << 0 << "\">" << "\n";
		graphml_file << "\t \t" << "<data key=\"name\">" << j++ << "</data>" << "\n";
		graphml_file << "\t" << "</edge>" << "\n";

		graphml_file << "\t" << "<edge source=\"n" << 2 << "\"" << " target=\"n" << 3 << "\">" << "\n";
		graphml_file << "\t \t" << "<data key=\"name\">" << j << "</data>" << "\n";
		graphml_file << "\t" << "</edge>" << "\n";

		graphml_file << "\t" << "<edge source=\"n" << 3 << "\"" << " target=\"n" << 2 << "\">" << "\n";
		graphml_file << "\t \t" << "<data key=\"name\">" << j++ << "</data>" << "\n";
		graphml_file << "\t" << "</edge>" << "\n";

		graphml_file << "\t" << "<edge source=\"n" << 0 << "\"" << " target=\"n" << 2 << "\">" << "\n";
		graphml_file << "\t \t" << "<data key=\"name\">" << j << "</data>" << "\n";
		graphml_file << "\t" << "</edge>" << "\n";

		graphml_file << "\t" << "<edge source=\"n" << 2 << "\"" << " target=\"n" << 0 << "\">" << "\n";
		graphml_file << "\t \t" << "<data key=\"name\">" << j++ << "</data>" << "\n";
		graphml_file << "\t" << "</edge>" << "\n";

		graphml_file << "\t" << "<edge source=\"n" << 1 << "\"" << " target=\"n" << 3 << "\">" << "\n";
		graphml_file << "\t \t" << "<data key=\"name\">" << j << "</data>" << "\n";
		graphml_file << "\t" << "</edge>" << "\n";

		graphml_file << "\t" << "<edge source=\"n" << 3 << "\"" << " target=\"n" << 1 << "\">" << "\n";
		graphml_file << "\t \t" << "<data key=\"name\">" << j++ << "</data>" << "\n";
		graphml_file << "\t" << "</edge>" << "\n";
	}
	else
	{
		vector<link_connection *>::iterator it;

		graphml_file << "<!-- east links -->\n";
		for ( it = new_east_links.begin(); it != new_east_links.end(); it++ )
		{
			graphml_file << "\t" << "<edge source=\"n" << (*it)->source << "\"" << " target=\"n" << (*it)->destination << "\">" << "\n";
			graphml_file << "\t \t" << "<data key=\"name\">" << (*it)->link_id << "</data>" << "\n";
			graphml_file << "\t" << "</edge>" << "\n";
		}

		graphml_file << "<!-- west links -->\n";
		for ( it = new_west_links.begin(); it != new_west_links.end(); it++ )
		{
			graphml_file << "\t" << "<edge source=\"n" << (*it)->source << "\"" << " target=\"n" << (*it)->destination << "\">" << "\n";
			graphml_file << "\t \t" << "<data key=\"name\">" << (*it)->link_id << "</data>" << "\n";
			graphml_file << "\t" << "</edge>" << "\n";
		}

		graphml_file << "<!-- south links -->\n";
		for ( it = new_south_links.begin(); it != new_south_links.end(); it++ )
		{
			graphml_file << "\t" << "<edge source=\"n" << (*it)->source << "\"" << " target=\"n" << (*it)->destination << "\">" << "\n";
			graphml_file << "\t \t" << "<data key=\"name\">" << (*it)->link_id << "</data>" << "\n";
			graphml_file << "\t" << "</edge>" << "\n";
		}

		graphml_file << "<!-- north links -->\n";
		for ( it = new_north_links.begin(); it != new_north_links.end(); it++ )
		{
			graphml_file << "\t" << "<edge source=\"n" << (*it)->source << "\"" << " target=\"n" << (*it)->destination << "\">" << "\n";
			graphml_file << "\t \t" << "<data key=\"name\">" << (*it)->link_id << "</data>" << "\n";
			graphml_file << "\t" << "</edge>" << "\n";
		}
	}

	/* now generating the links per node - interfaces and processors */
	uint start_interface = node_count;

	graphml_file << "<!-- interface edges -->\n";
	for ( i = 0; i < nodes; i++ )
	{
		for ( uint k = 0; k < concentration ; k++ ) // generate all the interfaces
		{
			/* generate the interface node */
			graphml_file << "\t" << "<node id=\"n" << node_count << "\">" << "\n";
			graphml_file << "\t \t" << "<data key=\"name\">" << node_count << "</data>\"" << "\n";
			graphml_file << "\t \t" << "<data key=\"type\">interface</data>\"" << "\n";
			graphml_file << "\t" << "</node>" << "\n";

			/* generate the interface edge */
			graphml_file << "\t" << "<edge source=\"n" << i << "\"" << " target=\"n" << node_count << "\">" << "\n";
			graphml_file << "\t \t" << "<data key=\"name\">" << j++ << "</data>" << "\n";
			graphml_file << "\t" << "</edge>" << "\n";

			graphml_file << "\t" << "<edge source=\"n" << node_count++ << "\"" << " target=\"n" << i << "\">" << "\n";
			graphml_file << "\t \t" << "<data key=\"name\">" << j++ << "</data>" << "\n";
			graphml_file << "\t" << "</edge>" << "\n";
		}
	}

	uint end_interface = node_count;

	/* generating processors */
	graphml_file << "<!-- processor edges -->\n";
	for ( i = start_interface ; i < end_interface ; i++)
	{
		for ( uint k = 0; k < no_of_cores ; k++ ) // generate all the interfaces
		{
			/* generate the processor node */
			graphml_file << "\t" << "<node id=\"n" << node_count << "\">" << "\n";
			graphml_file << "\t \t" << "<data key=\"name\">" << node_count << "</data>\"" << "\n";
			graphml_file << "\t \t" << "<data key=\"type\">core</data>\"" << "\n";
			graphml_file << "\t" << "</node>" << "\n";

			/* generate the processor edge */
			graphml_file << "\t" << "<edge source=\"n" << i << "\"" << " target=\"n" << node_count++ << "\">" << "\n";
			graphml_file << "\t \t" << "<data key=\"name\">" << j++ << "</data>" << "\n";
			graphml_file << "\t" << "</edge>" << "\n";
		}
	}

	graphml_file << "\t" << "</graph>" << "\n";
	graphml_file << "</graphml>" << "\n";
	graphml_file.close();

	new_east_links.clear();
	new_west_links.clear();
	new_north_links.clear();
	new_south_links.clear();
}


#endif

