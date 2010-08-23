/*
 * =====================================================================================
 *
 *       Filename:  testMesh.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/22/2010 06:16:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _testmesh_cc_INC
#define  _testmesh_cc_INC

#include	"networkSource.h"
#include	"packetSource.h"
#include	"../components/impl/genericInterface.h"
#include	"../components/impl/genericSink.h"
#include	"../components/impl/genericLink.h"
#include	"../components/impl/genericRouter.h"
#include	"../components/impl/genericTPG.h"
#include	"../components/impl/genericRPG.h"
#include	"../components/impl/mcFrontEnd.h"
#include	"../MemCtrl/MC.h"
#include	"../kernel/simulator.h"

#define _DEBUG 1
#define NO_OF_NODES 4
#define NO_OF_MC 1
#define INJ_PTS NO_OF_NODES - NO_OF_MC
#define GRID_SIZE 2
#define NO_OF_UNIDIRECTIONAL_LINKS (5+(GRID_SIZE-1)*4)+((4+(GRID_SIZE-1)*3)*(GRID_SIZE-1))
#define PORTS 5

string trace;
vector<Router*> routers;
vector<Processor*> end_sink_source;
vector<Interface*> interfaces;
vector<GenericLink*> link_a;
vector<GenericLink*> link_b;

void
set_connections()
{
    // Input and output link connections
    link_a[0]->input_connection = interfaces[0];
    link_a[0]->output_connection = routers[0];
    link_b[0]->input_connection = routers[0];
    link_b[0]->output_connection = interfaces[0];

    link_a[1]->input_connection = interfaces[1];
    link_a[1]->output_connection = routers[1];
    link_b[1]->input_connection = routers[1];
    link_b[1]->output_connection = interfaces[1];

    link_a[2]->input_connection = interfaces[2];
    link_a[2]->output_connection = routers[2];
    link_b[2]->input_connection = routers[2];
    link_b[2]->output_connection = interfaces[2];

    link_a[3]->input_connection = interfaces[3];
    link_a[3]->output_connection = routers[3];
    link_b[3]->input_connection = routers[3];
    link_b[3]->output_connection = interfaces[3];

    link_a[4]->input_connection = NULL;
    link_a[4]->output_connection = routers[1];
    link_b[4]->input_connection = routers[1];
    link_b[4]->output_connection = NULL;

    link_a[5]->input_connection = routers[0];
    link_a[5]->output_connection = routers[1];
    link_b[5]->input_connection = routers[1];
    link_b[5]->output_connection = routers[0];

    link_a[6]->input_connection = NULL;
    link_a[6]->output_connection = routers[0];
    link_b[6]->input_connection = routers[0];
    link_b[6]->output_connection = NULL;

    link_a[7]->input_connection = routers[0];
    link_a[7]->output_connection = routers[2];
    link_b[7]->input_connection = routers[2];
    link_b[7]->output_connection = routers[0];

    link_a[8]->input_connection = routers[1];
    link_a[8]->output_connection = NULL;
    link_b[8]->input_connection = NULL;
    link_b[8]->output_connection = routers[1];

    link_a[9]->input_connection = NULL;
    link_a[9]->output_connection = routers[1];
    link_b[9]->input_connection = routers[1];
    link_b[9]->output_connection = NULL;

    link_a[10]->input_connection = routers[1];
    link_a[10]->output_connection = routers[3];
    link_b[10]->input_connection = routers[3];
    link_b[10]->output_connection = routers[1];

    link_a[11]->input_connection = NULL;
    link_a[11]->output_connection = routers[1];
    link_b[11]->input_connection = routers[1];
    link_b[11]->output_connection = NULL;

    link_a[12]->input_connection = routers[2];
    link_a[12]->output_connection = routers[3];
    link_b[12]->input_connection = routers[3];
    link_b[12]->output_connection = routers[2];

    link_a[13]->input_connection = routers[2];
    link_a[13]->output_connection = NULL;
    link_b[13]->input_connection = NULL;
    link_b[13]->output_connection = routers[2];

    link_a[14]->input_connection = routers[3];
    link_a[14]->output_connection = routers[3];
    link_b[14]->input_connection = routers[3];
    link_b[14]->output_connection = routers[3];

    link_a[15]->input_connection = routers[3];
    link_a[15]->output_connection = NULL;
    link_b[15]->input_connection = NULL;
    link_b[15]->output_connection = routers[3];

    //  Router input and output connections
    routers[0]->input_connections.push_back(link_a[0]);
    routers[0]->input_connections.push_back(link_a[4]);
    routers[0]->input_connections.push_back(link_b[5]);
    routers[0]->input_connections.push_back(link_a[6]);
    routers[0]->input_connections.push_back(link_b[7]);
    
    routers[0]->output_connections.push_back(link_b[0]);
    routers[0]->output_connections.push_back(link_b[4]);
    routers[0]->output_connections.push_back(link_a[5]);
    routers[0]->output_connections.push_back(link_b[6]);
    routers[0]->output_connections.push_back(link_a[7]);

    routers[1]->input_connections.push_back(link_a[1]);
    routers[1]->input_connections.push_back(link_a[5]);
    routers[1]->input_connections.push_back(link_b[8]);
    routers[1]->input_connections.push_back(link_a[9]);
    routers[1]->input_connections.push_back(link_b[10]);
    
    routers[1]->output_connections.push_back(link_b[1]);
    routers[1]->output_connections.push_back(link_b[5]);
    routers[1]->output_connections.push_back(link_a[8]);
    routers[1]->output_connections.push_back(link_b[9]);
    routers[1]->output_connections.push_back(link_a[10]);
    
    routers[2]->input_connections.push_back(link_a[2]);
    routers[2]->input_connections.push_back(link_a[11]);
    routers[2]->input_connections.push_back(link_b[12]);
    routers[2]->input_connections.push_back(link_a[7]);
    routers[2]->input_connections.push_back(link_b[13]);
    
    routers[2]->output_connections.push_back(link_b[2]);
    routers[2]->output_connections.push_back(link_b[11]);
    routers[2]->output_connections.push_back(link_a[12]);
    routers[2]->output_connections.push_back(link_b[7]);
    routers[2]->output_connections.push_back(link_a[13]);

    routers[3]->input_connections.push_back(link_a[3]);
    routers[3]->input_connections.push_back(link_a[12]);
    routers[3]->input_connections.push_back(link_b[14]);
    routers[3]->input_connections.push_back(link_a[10]);
    routers[3]->input_connections.push_back(link_b[15]);
    
    routers[3]->output_connections.push_back(link_b[3]);
    routers[3]->output_connections.push_back(link_b[12]);
    routers[3]->output_connections.push_back(link_a[14]);
    routers[3]->output_connections.push_back(link_b[10]);
    routers[3]->output_connections.push_back(link_a[15]);


}

void 
generic_mesh_test()
{

    /* Init Routers */
    for ( uint i=0 ; i<INJ_PTS ; i++ )
    {
        end_sink_source.push_back(new GenericTPG());
        interfaces.push_back(new GenericInterface());
        static_cast<GenericTPG*>(end_sink_source[i])->set_trace_filename(trace);
    }

    for ( uint i=0 ; i<NO_OF_MC ; i++ )
    {
        end_sink_source.push_back(new GenericRPG());
        interfaces.push_back(new MCFrontEnd());
    }

    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
        routers.push_back(new GenericRouter());

    for ( uint i=0 ; i<NO_OF_UNIDIRECTIONAL_LINKS ; i++ )
    {
        link_a.push_back(new GenericLink());
        link_b.push_back(new GenericLink());
    }

    // Need to model links here for both input and output connections
    // Set the address of each of the components ( NS )
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        end_sink_source[i]->interface_connections[0] = interfaces[i];
        interfaces[i]->processor_connection = static_cast<NetworkComponent*>(end_sink_source[i]);

        /* Input and output connections for the interfaces. These links are
         * reused at the connections for the router but inverted. As
         * convention all a links enter router and b links exit routers */
        interfaces[i]->input_connection = link_b[i];
        interfaces[i]->output_connection = link_a[i];
    }

    /*  Set no of ports and positions for routing */
    vector< uint > grid_x;
    vector< uint > grid_y;
    grid_x.resize(NO_OF_NODES);
    grid_y.resize(NO_OF_NODES);

    /* Limitation of only modelling squares */
    grid_x[0] = 0;
    grid_x[2] = 0;
    grid_x[1] = 1;
    grid_x[3] = 1;
    grid_y[0] = 0;
    grid_y[2] = 1;
    grid_y[1] = 0;
    grid_y[3] = 1;

    for ( uint j=0 ; j<NO_OF_NODES ; j++ )
        for( uint i=0; i < PORTS ; i++)
        {
            // k-radix where k=4
            routers[j]->set_input_ports(PORTS);
            routers[j]->set_output_ports(PORTS);

            routers[j]->set_no_nodes(NO_OF_NODES);
        }

    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
        for( uint j=0; j < PORTS ; j++)
            for( uint k=0; k < NO_OF_NODES ; k++) // Assuming is a square mesh. 
            {
                static_cast<GenericRouter*>(routers[i])->set_grid_x_location(j,k,grid_x[k]); // decoders[j].grid_xloc[k] = grid_x[k];
                static_cast<GenericRouter*>(routers[i])->set_grid_y_location(j,k,grid_y[k]); // decoders[j].grid_xloc[k] = grid_x[k];
            }

    /*  Set up the ips for components */
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        interfaces[i]->node_ip = i;
        routers[i]->node_ip = i;
        end_sink_source[i]->node_ip = i;
    }

    set_connections();
    uint comp_id = 0, alink_comp_id = 100, blink_comp_id = 200;
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        end_sink_source[i]->setComponentId(comp_id++);
        interfaces[i]->setComponentId(comp_id++);
        routers[i]->setComponentId(comp_id++);
    }

    for ( uint i=0 ; i<NO_OF_UNIDIRECTIONAL_LINKS ; i++ )
    {
        link_a[i]->setComponentId(alink_comp_id++);
        link_b[i]->setComponentId(blink_comp_id++);
    }

    for ( uint i=0 ; i<INJ_PTS; i++ )
        static_cast<GenericTPG*>(end_sink_source[i])->setup();

    static_cast<GenericRPG*>(end_sink_source[3])->setup();

    /* Call setup on all components */
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        interfaces[i]->setup();
        routers[i]->setup();
        /* for now the links dont need a setup. They may later */
        // Give the interface a credit
        interfaces[i]->set_no_credits(1);
        routers[i]->set_no_credits(1);
    }

    /*  Printing out all component addresses */
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        cout << "\nRPG: " << i << " " << end_sink_source[i]->address;
        cout << "\ninterface: " << i << " " << interfaces[i]->address;
        cout << "\nrouter: " << i << " " << routers[i]->address;
    }

        /* for now the links dont need a setup. They may later */
    for ( uint i=0 ; i<NO_OF_UNIDIRECTIONAL_LINKS ; i++ )
    {
        cout << "\nlinka_" << i << " " << link_a[i]->toString();
        cout << "\nlinkb_" << i << " " << link_b[i]->toString();
    }
    
    Simulator::StopAt(1000);
    Simulator::Run();
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        cerr << routers[i]->print_stats()<< endl;
    }
    return;
}


int
main ( int argc, char *argv[] )
{
    if( argc < 2)
    {
        cout << " Err enter trace name\n" << endl;
        return 1;
    }
    else
        trace = argv[1];

    cerr << " Running trace: " << trace << endl;

    generic_mesh_test();
    return 0;
}				/* ----------  end of function main  ---------- */

#endif   /* ----- #ifndef _testmesh_cc_INC  ----- */

