/*
 * =====================================================================================
 *
 *       Filename:  ring.cc
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
#ifndef  _ring_cc_INC
#define  _ring_cc_INC

#include        "ring.h"

Ring::Ring()
{

}

Ring::~Ring()
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
Ring::init(uint p, uint v, uint c, uint bs, uint n, uint k, uint l)
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
Ring::print_stats()
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
Ring::connect_interface_processor()
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
Ring::connect_interface_routers()
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
Ring::connect_routers()
{
    uint last_link_id = no_nodes;
    for ( uint router_no=1 ; router_no<no_nodes; router_no++ )
    {
	    link_a[last_link_id]->input_connection = routers[router_no-1];
            link_a[last_link_id]->output_connection = routers[router_no];
	    link_b[last_link_id]->input_connection = routers[router_no];
	    link_b[last_link_id]->output_connection = routers[router_no-1];
            east_links.insert(make_pair(router_no, last_link_id));
            west_links.insert(make_pair(router_no-1, last_link_id));
            last_link_id++;
    }
    link_a[last_link_id]->input_connection = routers[no_nodes-1];
    link_a[last_link_id]->output_connection = routers[0];
    link_b[last_link_id]->input_connection = routers[0];
    link_b[last_link_id]->output_connection = routers[no_nodes-1];
    east_links.insert(make_pair(0, last_link_id));
    west_links.insert(make_pair(no_nodes-1, last_link_id));

    for ( uint i=0 ; i<no_nodes; i++ )
    {
        map<uint,uint>::iterator link_id = east_links.find(i);
        routers[i]->input_connections.push_back(link_a[link_id->second]);
        routers[i]->output_connections.push_back(link_b[link_id->second]);
    }
    /* ------------ Begin West links --------------------- */

    for ( uint i=0 ; i<no_nodes; i++ )
    {
        map<uint,uint>::iterator link_id = west_links.find(i);
        routers[i]->input_connections.push_back(link_b[link_id->second]);
        routers[i]->output_connections.push_back(link_a[link_id->second]);
    }

    if(last_link_id > links )
    {
        cout << "ERROR : incorrect topology last_link_id: " << last_link_id << " links: " << links <<endl;
        exit(1);
    }
    /* ------------ End South links --------------------- */

    return;
}

void
Ring::setup()
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
#endif   /* ----- #ifndef _ring_cc_INC  ----- */

