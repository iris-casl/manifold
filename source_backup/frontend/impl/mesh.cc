/*
 * =====================================================================================
 *
 *       Filename:  mesh.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/05/2010 12:37:47 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _mesh_cc_INC
#define  _mesh_cc_INC

#include        "mesh.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
    for ( uint i=0 ; i<no_nodes; i++ )
    {
        delete processors[i];
        delete interfaces[i];
        delete routers[i];
    }

    for ( uint i=0 ; i<links; i++ )
    {
        delete link_a[i];
        delete link_b[i];
    }

}

void
Mesh::init(uint p, uint v, uint c, uint bs, uint n, uint k, uint l)
{
    ports = p;
    vcs = v;
    credits  = c;
    buffer_size = bs;
    no_nodes = n;
    grid_size = k;
    links = l;
}

string
Mesh::print_stats()
{
    stringstream str;
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        str << routers[i]->print_stats()<< endl;
        str << interfaces[i]->print_stats()<< endl;
        str << processors[i]->print_stats()<< endl;
    }

    /* Removed LT and hence there is no need for the link stats */
if(stat_print_level > 2)
    for ( uint i=0 ; i<links ; i++ )
    {
        str << link_a[i]->print_stats()<< endl;
        str << link_b[i]->print_stats()<< endl;
    }

    return str.str();
}

void
Mesh::connect_interface_processor()
{
    /* Connect the interfaces and the processors. And set the links for the
     * interfaces */
    for ( uint i=0 ; i<no_nodes; i++ )
    {
        processors[i]->interface_connections.push_back(interfaces[i]);
        interfaces[i]->processor_connection = static_cast<NetworkComponent*>(processors[i]);
        interfaces[i]->input_connection = link_b[i];
        interfaces[i]->output_connection = link_a[i];
    }

    return;
}

void
Mesh::connect_interface_routers()
{
    //Input and output link connections
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
	link_a[i]->input_connection = interfaces[i];
	link_a[i]->output_connection = routers[i];
	link_b[i]->input_connection = routers[i];
	link_b[i]->output_connection = interfaces[i];
	routers[i]->input_connections.push_back(link_a[i]);
	routers[i]->output_connections.push_back(link_b[i]);
    }
    return;
}

void
Mesh::connect_routers()
{
    //  Router input and output connections
    /* ------------- Configure east links for the router ----------- */
    uint last_link_id = no_nodes;
    for ( uint router_no=0 ; router_no<no_nodes; router_no++ )
    {
        if( (router_no%grid_size) == 0 ) /* Left side end node */
        {
            // east going link for router_no
	    link_a[last_link_id]->input_connection = NULL;
	    link_a[last_link_id]->output_connection = routers[router_no];
            // west going link for router_no. This is router_no -1's second
            // input connection but since router_no-1 is out of the mesh you
            // dont put it in the map.
	    link_b[last_link_id]->input_connection = routers[router_no];
	    link_b[last_link_id]->output_connection = NULL;
            east_links.insert(make_pair(router_no, last_link_id));
            last_link_id++;
        }
        else
        {
	    link_a[last_link_id]->input_connection = routers[router_no-1];
            link_a[last_link_id]->output_connection = routers[router_no];
	    link_b[last_link_id]->input_connection = routers[router_no];
	    link_b[last_link_id]->output_connection = routers[router_no-1];
            east_links.insert(make_pair(router_no, last_link_id));
            west_links.insert(make_pair(router_no-1, last_link_id));
            last_link_id++;
        }
    }

    for ( uint i=0 ; i<no_nodes; i++ )
    {
        map<uint,uint>::iterator link_id = east_links.find(i);
	routers[i]->input_connections.push_back(link_a[link_id->second]);
	routers[i]->output_connections.push_back(link_b[link_id->second]);
    }
    //cout << "\n End East links " << last_link_id << endl;
/* ------------ End East links --------------------- */

/* ------------ Begin West links --------------------- */
    //Missing west going links that were missed above
    for ( uint router_no=0 ; router_no<no_nodes; router_no++ )
    {
        if ( (router_no%grid_size) == (grid_size-1))
        {
	    link_a[last_link_id]->input_connection = routers[router_no];
	    link_a[last_link_id]->output_connection = NULL;
            link_b[last_link_id]->input_connection = NULL;
	    link_b[last_link_id]->output_connection = routers[router_no];
            west_links.insert(make_pair(router_no, last_link_id));
            last_link_id++;
        }
    }

    for ( uint i=0 ; i<no_nodes; i++ )
    {
        map<uint,uint>::iterator link_id = west_links.find(i);
	routers[i]->input_connections.push_back(link_b[link_id->second]);
	routers[i]->output_connections.push_back(link_a[link_id->second]);
    }
    //cout << "\n End West links " << last_link_id << endl;
/* ------------ End West links --------------------- */

/* ------------ Begin North links --------------------- */
    // This is the same as above after transposing the mesh
    map < uint, uint > col_major_ordering;
    for ( uint i=0; i<grid_size; i++)
        for ( uint j=0; j<grid_size; j++)
            col_major_ordering.insert(make_pair(i*grid_size+j, j*grid_size+i));

    for ( uint router_no=0 ; router_no<no_nodes; router_no++ )
    {
        map<uint,uint>::iterator iter = col_major_ordering.find(router_no);
        uint col_major_router_no = iter->second;
        if( (col_major_router_no%grid_size) == 0 ) /* Top end node */
        {
	    link_a[last_link_id]->input_connection = NULL ;
            link_a[last_link_id]->output_connection = routers[router_no];
	    link_b[last_link_id]->input_connection = routers[router_no];
	    link_b[last_link_id]->output_connection = NULL ;
            north_links.insert(make_pair(router_no, last_link_id));
            last_link_id++;
        }
        else
        {
	    link_a[last_link_id]->input_connection = routers[router_no - grid_size];
	    link_a[last_link_id]->output_connection = routers[router_no];
	    link_b[last_link_id]->input_connection = routers[router_no];
	    link_b[last_link_id]->output_connection = routers[router_no - grid_size];
            north_links.insert(make_pair(router_no, last_link_id));
            south_links.insert(make_pair(router_no - grid_size, last_link_id));
            last_link_id++;
        }
    }

    for ( uint i=0 ; i<no_nodes; i++ )
    {
        map<uint,uint>::iterator link_id = north_links.find(i);
	routers[i]->input_connections.push_back(link_a[link_id->second]);
	routers[i]->output_connections.push_back(link_b[link_id->second]);
    }
    //cout << "\n End North links " << last_link_id << endl;
/* ------------ End North links --------------------- */

/* ------------ Begin South links --------------------- */

    for ( uint router_no=0 ; router_no<no_nodes; router_no++ )
    {
        map<uint,uint>::iterator iter = col_major_ordering.find(router_no);
        uint col_major_router_no = iter->second;
        if ( (col_major_router_no%grid_size) == (grid_size-1))
        {
	    link_a[last_link_id]->input_connection = routers[router_no];
            link_a[last_link_id]->output_connection = NULL;
	    link_b[last_link_id]->input_connection = NULL;
	    link_b[last_link_id]->output_connection = routers[router_no];
            south_links.insert(make_pair(router_no, last_link_id));
            last_link_id++;
        }
    }

    for ( uint i=0 ; i<no_nodes; i++ )
    {
        map<uint,uint>::iterator link_id = south_links.find(i);
	routers[i]->input_connections.push_back(link_b[link_id->second]);
	routers[i]->output_connections.push_back(link_a[link_id->second]);
    }
    //cout << "\n End North links " << last_link_id << endl;

    if(last_link_id > links )
    {
        cout << "ERROR : incorrect topology last_link_id: " << last_link_id << " links: " << links <<endl;
        exit(1);
    }
/* ------------ End South links --------------------- */

    return;
}

void
Mesh::setup()
{
    /* Call setup on all components */
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        processors[i]->setup( no_nodes, vcs, max_sim_time);
        interfaces[i]->setup(vcs, credits, buffer_size);
        routers[i]->init(ports, vcs, credits, buffer_size);
    }

    for ( uint i=0 ; i<links; i++ )
    {
        link_a[i]->setup();
        link_b[i]->setup();
    }
    return;
}
#endif   /* ----- #ifndef _mesh_cc_INC  ----- */

