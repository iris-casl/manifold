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

#include	"packetSource.h"
#include	"../components/impl/genericInterface.h"
#include	"../components/impl/genericSink.h"
#include	"../components/impl/genericLink.h"
#include	"../components/impl/RouterDallyPeh.h"
#include	"../components/impl/genericRPG.h"
#include	"../kernel/simulator.h"

#define NO_OF_NODES 9
#define GRID_SIZE 3
#define NO_OF_UNIDIRECTIONAL_LINKS (5+(GRID_SIZE-1)*4)+((4+(GRID_SIZE-1)*3)*(GRID_SIZE-1))
#define PORTS 5

void
print_router_connections()
{
    cout << "\t//  Router input and output connections" << endl;
    for( uint i=0; i < NO_OF_NODES; i++)
        cout << "\trouters["<<i<<"]->input_connections.push_back(link_a["<<i<<"]);" << endl;

    cout << endl;
    for( uint i=0; i < NO_OF_NODES; i++)
        cout << "\trouters["<<i<<"]->output_connections.push_back(link_b["<<i<<"]);" << endl;

    cout << endl;
    /*  got to correct these to "abab" for input and "baba" for output */
    uint link_no = NO_OF_NODES;
    for( uint i=0; i < NO_OF_NODES; i++)
    {
        for( uint j= 0 ; j < 4; j++)
        {
            cout << "\trouters["<<i<<"]->input_connections.push_back(link_a["<<link_no<<"]);" << endl;
            link_no++;
        }
        cout << endl;
    }

    for( uint i=0; i < NO_OF_NODES; i++)
    {
        for( uint j= 0 ; j < 4; j++)
        {
            cout << "\trouters["<< i << "]->output_connections.push_back(link_b[" << link_no << "]);" << endl;
            link_no++;
        }
        cout << endl;
    }

        cout << endl << endl;
}

void
print_link_connections()
{
    for( uint i=0; i < NO_OF_NODES; i++)
    {
        cout << "\tlink_a[" << i << "]->input_connection = interfaces[" << i <<"];" << endl;
        cout << "\tlink_a[" << i << "]->output_connection = routers["<< i << "];" << endl;
        cout << "\tlink_b[" << i << "]->input_connection = routers[" << i << "];" << endl;
        cout << "\tlink_b[" << i << "]->output_connection = interfaces[" << i << "];" << endl;
        cout << endl;
    }

        cout << endl << endl;
    for( uint i=NO_OF_NODES; i < NO_OF_UNIDIRECTIONAL_LINKS; i++)
    {
        cout << "\tlink_a[" << i << "]->input_connection = routers[" << i <<"];" << endl;
        cout << "\tlink_a[" << i << "]->output_connection = routers["<< i << "];" << endl;
        cout << "\tlink_b[" << i << "]->input_connection = routers[" << i << "];" << endl;
        cout << "\tlink_b[" << i << "]->output_connection = routers[" << i << "];" << endl;
        cout << endl;
    }
    return;
}

void 
generic_mesh_test()
{
    vector<RouterDallyPeh*> routers;
    vector<GenericRPG*> end_sink_source;
    vector<GenericInterface*> interfaces;
    vector<GenericLink*> link_a;
    vector<GenericLink*> link_b;

    /* Init Routers */
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        end_sink_source.push_back(new GenericRPG());
        interfaces.push_back(new GenericInterface());
        routers.push_back(new RouterDallyPeh());
    }

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

    uint comp_id = 0, alink_comp_id = 100, blink_comp_id = 200;
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        end_sink_source[i]->setComponentId(comp_id++);
        interfaces[i]->setComponentId(comp_id++);
        routers[i]->setComponentId(comp_id++);
    }

    /*  Set up the ips for components */
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        interfaces[i]->node_ip = i;
        routers[i]->node_ip = i;
        static_cast<GenericRPG*>(end_sink_source[i])->node_ip = i;
    }

    for ( uint i=0 ; i<NO_OF_UNIDIRECTIONAL_LINKS ; i++ )
    {
        link_a[i]->setComponentId(alink_comp_id++);
        link_b[i]->setComponentId(blink_comp_id++);
    }

    /* Call setup on all components */
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        end_sink_source[i]->setup();
        interfaces[i]->setup();
        routers[i]->init();
        /* for now the links dont need a setup. They may later */
        // Give the interface a credit
        interfaces[i]->set_no_credits(1);
    }

    /*  Set no of ports and positions for routing */
    vector< uint > grid_x;
    vector< uint > grid_y;
    grid_x.resize(NO_OF_NODES);
    grid_y.resize(NO_OF_NODES);

    /* Limitation of only modelling squares */
    for ( uint i=0 ; i<GRID_SIZE ; i++ )
        for ( uint j=0 ; j<GRID_SIZE ; j++ )
        {
            grid_x[(i*GRID_SIZE)+j] = j;
            grid_y[(i*GRID_SIZE)+j] = i;
        }


    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
        routers[i]->set_no_nodes(NO_OF_NODES);

    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
        for( uint j=0; j < PORTS ; j++)
            for( uint k=0; k < NO_OF_NODES ; k++) // Assuming is a square mesh. 
            {
                routers[i]->set_grid_x_location(j,k, grid_x[k]);
                routers[i]->set_grid_y_location(j,k, grid_y[k]);
            }



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

	link_a[4]->input_connection = interfaces[4];
	link_a[4]->output_connection = routers[4];
	link_b[4]->input_connection = routers[4];
	link_b[4]->output_connection = interfaces[4];

	link_a[5]->input_connection = interfaces[5];
	link_a[5]->output_connection = routers[5];
	link_b[5]->input_connection = routers[5];
	link_b[5]->output_connection = interfaces[5];

	link_a[6]->input_connection = interfaces[6];
	link_a[6]->output_connection = routers[6];
	link_b[6]->input_connection = routers[6];
	link_b[6]->output_connection = interfaces[6];

	link_a[7]->input_connection = interfaces[7];
	link_a[7]->output_connection = routers[7];
	link_b[7]->input_connection = routers[7];
	link_b[7]->output_connection = interfaces[7];

	link_a[8]->input_connection = interfaces[8];
	link_a[8]->output_connection = routers[8];
	link_b[8]->input_connection = routers[8];
	link_b[8]->output_connection = interfaces[8];


	link_a[9]->input_connection = NULL;
	link_a[9]->output_connection = routers[0];
	link_b[9]->input_connection = routers[0];
	link_b[9]->output_connection = NULL;

	link_a[10]->input_connection = routers[0];
	link_a[10]->output_connection = routers[1];
	link_b[10]->input_connection = routers[1];
	link_b[10]->output_connection = routers[0];

	link_a[11]->input_connection = NULL ;
	link_a[11]->output_connection = routers[0];
	link_b[11]->input_connection = routers[0];
	link_b[11]->output_connection = NULL ;

	link_a[12]->input_connection = routers[0];
	link_a[12]->output_connection = routers[3];
	link_b[12]->input_connection = routers[3];
	link_b[12]->output_connection = routers[0];

	link_a[13]->input_connection = routers[1];
	link_a[13]->output_connection = routers[2];
	link_b[13]->input_connection = routers[2];
	link_b[13]->output_connection = routers[1];

	link_a[14]->input_connection = NULL;
	link_a[14]->output_connection = routers[1];
	link_b[14]->input_connection = routers[1];
	link_b[14]->output_connection = NULL;

	link_a[15]->input_connection = routers[1];
	link_a[15]->output_connection = routers[4];
	link_b[15]->input_connection = routers[4];
	link_b[15]->output_connection = routers[1];

	link_a[16]->input_connection = routers[2];
	link_a[16]->output_connection = NULL;
	link_b[16]->input_connection = NULL;
	link_b[16]->output_connection = routers[2];

	link_a[17]->input_connection = NULL;
	link_a[17]->output_connection = routers[2];
	link_b[17]->input_connection = routers[2];
	link_b[17]->output_connection = NULL;

	link_a[18]->input_connection = routers[2];
	link_a[18]->output_connection = routers[5];
	link_b[18]->input_connection = routers[5];
	link_b[18]->output_connection = routers[2];

	link_a[19]->input_connection = NULL;
	link_a[19]->output_connection = routers[3];
	link_b[19]->input_connection = routers[3];
	link_b[19]->output_connection = NULL;

	link_a[20]->input_connection = routers[3];
	link_a[20]->output_connection = routers[4];
	link_b[20]->input_connection = routers[4];
	link_b[20]->output_connection = routers[3];

	link_a[21]->input_connection = routers[3];
	link_a[21]->output_connection = routers[6];
	link_b[21]->input_connection = routers[6];
	link_b[21]->output_connection = routers[3];

	link_a[22]->input_connection = routers[4];
	link_a[22]->output_connection = routers[5];
	link_b[22]->input_connection = routers[5];
	link_b[22]->output_connection = routers[4];

	link_a[23]->input_connection = routers[4];
	link_a[23]->output_connection = routers[7];
	link_b[23]->input_connection = routers[7];
	link_b[23]->output_connection = routers[4];

	link_a[24]->input_connection = routers[5];
	link_a[24]->output_connection = NULL;
	link_b[24]->input_connection = NULL;
	link_b[24]->output_connection = routers[5];

	link_a[25]->input_connection = routers[5];
	link_a[25]->output_connection = routers[8];
	link_b[25]->input_connection = routers[8];
	link_b[25]->output_connection = routers[5];

	link_a[26]->input_connection = NULL;
	link_a[26]->output_connection = routers[6];
	link_b[26]->input_connection = routers[6];
	link_b[26]->output_connection = NULL;

	link_a[27]->input_connection = routers[6];
	link_a[27]->output_connection = routers[7];
	link_b[27]->input_connection = routers[7];
	link_b[27]->output_connection = routers[6];

	link_a[28]->input_connection = routers[6];
	link_a[28]->output_connection = NULL;
	link_b[28]->input_connection = NULL;
	link_b[28]->output_connection = routers[6];

	link_a[29]->input_connection = routers[7];
	link_a[29]->output_connection = routers[8];
	link_b[29]->input_connection = routers[8];
	link_b[29]->output_connection = routers[7];

	link_a[30]->input_connection = routers[7];
	link_a[30]->output_connection = NULL;
	link_b[30]->input_connection = NULL;
	link_b[30]->output_connection = routers[7];

	link_a[31]->input_connection = routers[8];
	link_a[31]->output_connection = NULL;
	link_b[31]->input_connection = NULL;
	link_b[31]->output_connection = routers[8];

	link_a[32]->input_connection = routers[8];
	link_a[32]->output_connection = NULL;
	link_b[32]->input_connection = NULL;
	link_b[32]->output_connection = routers[8];

	//  Router input and output connections
	routers[0]->input_connections.push_back(link_a[0]);
	routers[1]->input_connections.push_back(link_a[1]);
	routers[2]->input_connections.push_back(link_a[2]);
	routers[3]->input_connections.push_back(link_a[3]);
	routers[4]->input_connections.push_back(link_a[4]);
	routers[5]->input_connections.push_back(link_a[5]);
	routers[6]->input_connections.push_back(link_a[6]);
	routers[7]->input_connections.push_back(link_a[7]);
	routers[8]->input_connections.push_back(link_a[8]);

	routers[0]->output_connections.push_back(link_b[0]);
	routers[1]->output_connections.push_back(link_b[1]);
	routers[2]->output_connections.push_back(link_b[2]);
	routers[3]->output_connections.push_back(link_b[3]);
	routers[4]->output_connections.push_back(link_b[4]);
	routers[5]->output_connections.push_back(link_b[5]);
	routers[6]->output_connections.push_back(link_b[6]);
	routers[7]->output_connections.push_back(link_b[7]);
	routers[8]->output_connections.push_back(link_b[8]);

	routers[0]->input_connections.push_back(link_a[9]);
	routers[0]->input_connections.push_back(link_b[10]);
	routers[0]->input_connections.push_back(link_a[11]);
	routers[0]->input_connections.push_back(link_b[12]);

	routers[1]->input_connections.push_back(link_a[10]);
	routers[1]->input_connections.push_back(link_b[13]);
	routers[1]->input_connections.push_back(link_a[14]);
	routers[1]->input_connections.push_back(link_b[15]);

	routers[2]->input_connections.push_back(link_a[13]);
	routers[2]->input_connections.push_back(link_b[16]);
	routers[2]->input_connections.push_back(link_a[17]);
	routers[2]->input_connections.push_back(link_b[18]);

	routers[3]->input_connections.push_back(link_a[19]);
	routers[3]->input_connections.push_back(link_b[20]);
	routers[3]->input_connections.push_back(link_a[12]);
	routers[3]->input_connections.push_back(link_b[21]);

	routers[4]->input_connections.push_back(link_a[20]);
	routers[4]->input_connections.push_back(link_b[22]);
	routers[4]->input_connections.push_back(link_a[15]);
	routers[4]->input_connections.push_back(link_b[23]);

	routers[5]->input_connections.push_back(link_a[22]);
	routers[5]->input_connections.push_back(link_b[24]);
	routers[5]->input_connections.push_back(link_a[18]);
	routers[5]->input_connections.push_back(link_b[25]);

	routers[6]->input_connections.push_back(link_a[26]);
	routers[6]->input_connections.push_back(link_b[27]);
	routers[6]->input_connections.push_back(link_a[21]);
	routers[6]->input_connections.push_back(link_b[28]);

	routers[7]->input_connections.push_back(link_a[27]);
	routers[7]->input_connections.push_back(link_b[29]);
	routers[7]->input_connections.push_back(link_a[23]);
	routers[7]->input_connections.push_back(link_b[30]);

	routers[8]->input_connections.push_back(link_a[29]);
	routers[8]->input_connections.push_back(link_b[31]);
	routers[8]->input_connections.push_back(link_a[25]);
	routers[8]->input_connections.push_back(link_b[32]);

	routers[0]->output_connections.push_back(link_b[9]);
	routers[0]->output_connections.push_back(link_a[10]);
	routers[0]->output_connections.push_back(link_b[11]);
	routers[0]->output_connections.push_back(link_a[12]);

	routers[1]->output_connections.push_back(link_b[10]);
	routers[1]->output_connections.push_back(link_a[13]);
	routers[1]->output_connections.push_back(link_b[14]);
	routers[1]->output_connections.push_back(link_a[15]);

	routers[2]->output_connections.push_back(link_b[13]);
	routers[2]->output_connections.push_back(link_a[16]);
	routers[2]->output_connections.push_back(link_b[17]);
	routers[2]->output_connections.push_back(link_a[18]);

	routers[3]->output_connections.push_back(link_b[19]);
	routers[3]->output_connections.push_back(link_a[20]);
	routers[3]->output_connections.push_back(link_b[12]);
	routers[3]->output_connections.push_back(link_a[21]);

	routers[4]->output_connections.push_back(link_b[20]);
	routers[4]->output_connections.push_back(link_a[22]);
	routers[4]->output_connections.push_back(link_b[15]);
	routers[4]->output_connections.push_back(link_a[23]);

	routers[5]->output_connections.push_back(link_b[22]);
	routers[5]->output_connections.push_back(link_a[24]);
	routers[5]->output_connections.push_back(link_b[18]);
	routers[5]->output_connections.push_back(link_a[25]);

	routers[6]->output_connections.push_back(link_b[26]);
	routers[6]->output_connections.push_back(link_a[27]);
	routers[6]->output_connections.push_back(link_b[21]);
	routers[6]->output_connections.push_back(link_a[28]);

	routers[7]->output_connections.push_back(link_b[27]);
	routers[7]->output_connections.push_back(link_a[29]);
	routers[7]->output_connections.push_back(link_b[23]);
	routers[7]->output_connections.push_back(link_a[30]);

	routers[8]->output_connections.push_back(link_b[29]);
	routers[8]->output_connections.push_back(link_a[31]);
	routers[8]->output_connections.push_back(link_b[25]);
	routers[8]->output_connections.push_back(link_a[32]);


    /*  Printing out all component addresses */
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        cout << "\nRPG: " << i << " " << end_sink_source[i]->address;
        cout << "\ninterface: " << i << " " << interfaces[i]->address;
        cout << "\nrouter: " << i << " " << routers[i]->address;
        cout << "R" << routers[i]->address << routers[i]->toString() << endl;
    }

        /* for now the links dont need a setup. They may later */
    for ( uint i=0 ; i<NO_OF_UNIDIRECTIONAL_LINKS ; i++ )
    {
        cout << "\nlinka_" << i << " " << link_a[i]->toString();
        cout << "\nlinkb_" << i << " " << link_b[i]->toString();
    }
    
    Simulator::StopAt(100);
    Simulator::Run();
    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        cerr << routers[i]->print_stats()<< endl;
    }

    for ( uint i=0 ; i<NO_OF_NODES ; i++ )
    {
        delete end_sink_source[i];
        delete interfaces[i];
        delete routers[i];
    }

    for ( uint i=0 ; i<NO_OF_UNIDIRECTIONAL_LINKS ; i++ )
    {
        delete link_a[i];
        delete link_b[i];
    }
    
    return;
}


int
main ( int argc, char *argv[] )
{

    /*  Need to read in a config file */
    generic_mesh_test();
    return 0;
}				/* ----------  end of function main  ---------- */

#endif   /* ----- #ifndef _testmesh_cc_INC  ----- */

