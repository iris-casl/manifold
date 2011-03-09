/*
 * =====================================================================================
 *
 *       Filename:  testInterface.cc
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
#ifndef  _testinterface_cc_INC
#define  _testinterface_cc_INC

#include	"networkSource.h"
#include	"packetSource.h"
#include	"../components/impl/genericInterface.h"
#include	"../components/impl/genericSink.h"
#include	"../components/impl/genericLink.h"
#include	"../components/impl/genericRouter.h"
#include	"../kernel/simulator.h"

void 
test_two_routers()
{
    GenericRouter* router1 = new GenericRouter();
    GenericRouter* router2 = new GenericRouter();
    RandomPacketGenerator* rpg1 = new RandomPacketGenerator();
//    GenericSink* rpg1 = new GenericSink();
    RandomPacketGenerator* rpg2 = new RandomPacketGenerator();
//    GenericSink* rpg2 = new GenericSink();
    GenericInterface* interface1 = new GenericInterface();
    GenericInterface* interface2 = new GenericInterface();
    GenericLink* link1 = new GenericLink();
    GenericLink* link2 = new GenericLink();
    GenericLink* link3 = new GenericLink();
    GenericLink* link4 = new GenericLink();
    GenericLink* link5 = new GenericLink();
    GenericLink* link6 = new GenericLink();
    // Need to model links here for both input and output connections
    // Set the address of each of the components ( NS )
    rpg1->interface_connections[0] = interface1;
    rpg2->interface_connections[0] = interface2;

    // Give the interfac a credit
    interface1->set_no_credits(1);
    interface2->set_no_credits(1);
    // Set the interface connections
    interface1->processor_connection = static_cast<NetworkComponent*>(rpg1);
    interface2->processor_connection = static_cast<NetworkComponent*>(rpg2);
    interface1->input_connection = link2;
    interface1->output_connection = link1;
    interface2->input_connection = link5;
    interface2->output_connection = link6;

    // Input and output link connections
    link1->input_connection = interface1;
    link2->input_connection = router1;
    link3->input_connection = router1;
    link4->input_connection = router2;
    link5->input_connection = router2;
    link6->input_connection = interface2;

    link1->output_connection = router1;
    link2->output_connection = interface1;
    link3->output_connection = router2;
    link4->output_connection = router1;
    link5->output_connection = interface2;
    link6->output_connection = router2;

    //  Router input and output connections
    router1->input_connections.push_back(link1);
    router1->input_connections.push_back(link4);
    router1->output_connections.push_back(link2);
    router1->output_connections.push_back(link3);
    router2->input_connections.push_back(link3);
    router2->input_connections.push_back(link6);
    router2->output_connections.push_back(link4);
    router2->output_connections.push_back(link5);
    
    // k-radix where k=4
    router1->set_input_ports(2);
    router1->set_output_ports(2);
    router2->set_input_ports(2);
    router2->set_output_ports(2);

    rpg1->setComponentId(0);
    rpg2->setComponentId(5);
    interface1->setComponentId(2);
    interface2->setComponentId(3);
    router1->setComponentId(1);
    router2->setComponentId(4);

    /*  Set up the ips for components */
    rpg1->node_ip = 0;
    rpg2->node_ip = 1;
    router1->node_ip = 0;
    router2->node_ip = 1;
    interface1->node_ip = 0;
    interface2->node_ip = 1;

    link1->setComponentId(6);
    link2->setComponentId(7);
    link3->setComponentId(8);
    link4->setComponentId(9);
    link5->setComponentId(10);
    link6->setComponentId(11);

    rpg1->setup();
    rpg2->setup();
    interface1->setup();
    interface2->setup();
    router1->setup();
    router2->setup();
    link1->setup();
    link2->setup();
    link3->setup();
    link4->setup();
    link5->setup();
    link6->setup();

    cout << "link1 " << link1->toString() << endl;
    cout << "link2 " << link2->toString() << endl;
    cout << "link3 " << link3->toString() << endl;
    cout << "link4 " << link4->toString() << endl;
    cout << "link5 " << link5->toString() << endl;
    cout << "link6 " << link6->toString() << endl;

    return;
}

void 
generic_mesh_test()
{
    vector<GenericRouter*> routers;
    vector<RandomPacketGenerator*> end_sink_source;
    vector<GenericInterface*> interfaces;
    vector<GenericLink*> link_a;
    vector<GenericLink*> link_b;
//    routers.resize(2);
//    end_sink_source.resize(2);
//    interfaces.resize(2);

    /* Init Routers */
    for ( uint i=0 ; i<2 ; i++ )
        routers.push_back(new GenericRouter());

    /* Packet generators */
    for ( uint i=0 ; i<2 ; i++ )
        end_sink_source.push_back(new RandomPacketGenerator());
//    GenericSink* end_sink_source[1] = new GenericSink();

    for ( uint i=0 ; i<2 ; i++ )
        interfaces.push_back(new GenericInterface());

    for ( uint i=0 ; i<5 ; i++ )
        link_a.push_back(new GenericLink());

    for ( uint i=0 ; i<5 ; i++ )
        link_b.push_back(new GenericLink());

    // Need to model links here for both input and output connections
    // Set the address of each of the components ( NS )
    for ( uint i=0 ; i<2 ; i++ )
    {
        end_sink_source[i]->interface_connections[0] = interfaces[i];
        interfaces[i]->processor_connection = static_cast<NetworkComponent*>(end_sink_source[i]);
        // Give the interface a credit
        interfaces[i]->set_no_credits(1);

        /* Input and output connections for the interfaces. These links are
         * reused at the connections for the router but inverted. As
         * convention all a links enter router and b links exit routers */
        interfaces[i]->input_connection = link_b[i];
        interfaces[i]->output_connection = link_a[i];
    }

    /*  Set no of ports and positions for routing */
    vector< uint > grid_x;
    vector< uint > grid_y;
    grid_x.resize(2);
    grid_y.resize(2);
        grid_x[0] = 0;
        grid_x[1] = 1;
        grid_y[0] = 0;
        grid_y[1] = 0;

    for ( uint j=0 ; j<2 ; j++ )
        for( uint i=0; i < 3 ; i++)
        {
            // k-radix where k=4
            routers[j]->set_input_ports(3);
            routers[j]->set_output_ports(3);

            routers[j]->decoders[i].grid_xloc.resize(2);
            routers[j]->decoders[i].grid_yloc.resize(2);
            routers[j]->decoders[i].grid_xloc[j] = grid_y[j];
        }

    for ( uint i=0 ; i<2 ; i++ )
        for( uint j=0; j < 3 ; j++)
            for( uint k=0; k < 3 ; k++)
            {
                routers[i]->decoders[j].grid_xloc[k] = grid_x[k];
                routers[i]->decoders[j].grid_yloc[k] = grid_y[k];
            }

    /*  Set up the ips for components */
    for ( uint i=0 ; i<2 ; i++ )
    {
        end_sink_source[i]->node_ip = i;
        interfaces[i]->node_ip = i;
        routers[i]->node_ip = i;
    }

    for ( uint i=0 ; i<2 ; i++ )
        for ( uint j=0 ; j<3 ; j++ )
            routers[i]->decoders[j].node_ip=i;

    // Input and output link connections
    link_a[0]->input_connection = interfaces[0];
    link_a[0]->output_connection = routers[0];
    link_b[0]->input_connection = routers[0];
    link_b[0]->output_connection = interfaces[0];

    link_a[1]->input_connection = interfaces[1];
    link_a[1]->output_connection = routers[1];
    link_b[1]->input_connection = routers[1];
    link_b[1]->output_connection = interfaces[1];

    link_a[2]->input_connection = routers[0];
    link_a[2]->output_connection = routers[0];
    link_b[2]->input_connection = routers[0];
    link_b[2]->output_connection = routers[0];

    link_a[3]->input_connection = routers[0];
    link_a[3]->output_connection = routers[1];
    link_b[3]->input_connection = routers[1];
    link_b[3]->output_connection = routers[0];

    link_a[4]->input_connection = routers[1];
    link_a[4]->output_connection = routers[1];
    link_b[4]->input_connection = routers[1];
    link_b[4]->output_connection = routers[1];


    //  Router input and output connections
    routers[0]->input_connections.push_back(link_a[0]);
    routers[0]->input_connections.push_back(link_a[2]);
    routers[0]->input_connections.push_back(link_b[3]);
    
    routers[0]->output_connections.push_back(link_b[0]);
    routers[0]->output_connections.push_back(link_b[2]);
    routers[0]->output_connections.push_back(link_a[3]);

    routers[1]->input_connections.push_back(link_a[1]);
    routers[1]->input_connections.push_back(link_a[3]);
    routers[1]->input_connections.push_back(link_b[4]);
    
    routers[1]->output_connections.push_back(link_b[1]);
    routers[1]->output_connections.push_back(link_b[3]);
    routers[1]->output_connections.push_back(link_a[4]);
    
    uint comp_id = 0, alink_comp_id = 10, blink_comp_id = 20;
    for ( uint i=0 ; i<2 ; i++ )
    {
        end_sink_source[i]->setComponentId(comp_id++);
        interfaces[i]->setComponentId(comp_id++);
        routers[i]->setComponentId(comp_id++);
    }

    for ( uint i=0 ; i<5 ; i++ )
    {
        link_a[i]->setComponentId(alink_comp_id++);
        link_b[i]->setComponentId(blink_comp_id++);
    }

    /* Call setup on all components */
    for ( uint i=0 ; i<2 ; i++ )
    {
        end_sink_source[i]->setup();
        interfaces[i]->setup();
        routers[i]->setup();
    }

    return;
}
void new_test()
{
    GenericRouter* router1 = new GenericRouter();
    GenericRouter* router2 = new GenericRouter();
    RandomPacketGenerator* rpg1 = new RandomPacketGenerator();
//    GenericSink* rpg1 = new GenericSink();
    RandomPacketGenerator* rpg2 = new RandomPacketGenerator();
//    GenericSink* rpg2 = new GenericSink();
    GenericInterface* interface1 = new GenericInterface();
    GenericInterface* interface2 = new GenericInterface();
    GenericLink* link0_a = new GenericLink();
    GenericLink* link1_a = new GenericLink();
    GenericLink* link2_a = new GenericLink();
    GenericLink* link3_a = new GenericLink();
    GenericLink* link4_a = new GenericLink();
    GenericLink* link0_b = new GenericLink();
    GenericLink* link1_b = new GenericLink();
    GenericLink* link2_b = new GenericLink();
    GenericLink* link3_b = new GenericLink();
    GenericLink* link4_b = new GenericLink();

    // Need to model links here for both input and output connections
    // Set the address of each of the components ( NS )
    rpg1->interface_connections[0] = interface1;
    rpg2->interface_connections[0] = interface2;

    // Give the interfac a credit
    interface1->set_no_credits(1);
    interface2->set_no_credits(1);
    // Set the interface connections
    interface1->processor_connection = static_cast<NetworkComponent*>(rpg1);
    interface2->processor_connection = static_cast<NetworkComponent*>(rpg2);

    interface1->input_connection = link0_b;
    interface1->output_connection = link0_a;
    interface2->input_connection = link1_b;
    interface2->output_connection = link1_a;

    // Input and output link connections
    link0_a->input_connection = interface1;
    link0_a->output_connection = router1;
    link0_b->input_connection = router1;
    link0_b->output_connection = interface1;

    link1_a->input_connection = interface2;
    link1_a->output_connection = router2;
    link1_b->input_connection = router2;
    link1_b->output_connection = interface2;

    link2_a->input_connection = router1;
    link2_a->output_connection = router1;
    link2_b->input_connection = router1;
    link2_b->output_connection = router1;

    link3_a->input_connection = router1;
    link3_a->output_connection = router2;
    link3_b->input_connection = router2;
    link3_b->output_connection = router1;

    link4_a->input_connection = router2;
    link4_a->output_connection = router2;
    link4_b->input_connection = router2;
    link4_b->output_connection = router2;

    /*  Set up the ips for components */
    rpg1->node_ip = 0;
    rpg2->node_ip = 1;
    router1->node_ip = 0;
    router2->node_ip = 1;
    interface1->node_ip = 0;
    interface2->node_ip = 1;

    // k-radix where k=4
    router1->set_input_ports(3);
    router1->set_output_ports(3);
    router2->set_input_ports(3);
    router2->set_output_ports(3);
for( uint i=0; i < 3 ; i++){
    router1->decoders[i].node_ip=0;
    router1->decoders[i].grid_xloc.resize(2);
    router1->decoders[i].grid_yloc.resize(2);
    router1->decoders[i].grid_xloc[0] = 0;
    router1->decoders[i].grid_xloc[1] = 1;
    router1->decoders[i].grid_yloc[0] = 0;
    router1->decoders[i].grid_yloc[1] = 0;
}


for( uint i=0; i < 3 ; i++)
{
    router2->decoders[i].node_ip=1;
    router2->decoders[i].grid_xloc.resize(2);
    router2->decoders[i].grid_yloc.resize(2);
    router2->decoders[i].grid_xloc[0] = 0;
    router2->decoders[i].grid_xloc[1] = 1;
    router2->decoders[i].grid_yloc[0] = 0;
    router2->decoders[i].grid_yloc[1] = 0;
}


    //  Router input and output connections
    router1->input_connections.push_back(link0_a);
    router1->input_connections.push_back(link2_a);
    router1->input_connections.push_back(link3_b);
    
    router1->output_connections.push_back(link0_b);
    router1->output_connections.push_back(link2_b);
    router1->output_connections.push_back(link3_a);

    router2->input_connections.push_back(link1_a);
    router2->input_connections.push_back(link3_a);
    router2->input_connections.push_back(link4_b);
    
    router2->output_connections.push_back(link1_b);
    router2->output_connections.push_back(link3_b);
    router2->output_connections.push_back(link4_a);
    
    rpg1->setComponentId(0);
    rpg2->setComponentId(1);
    interface1->setComponentId(2);
    interface2->setComponentId(3);
    router1->setComponentId(4);
    router2->setComponentId(5);


    link0_a->setComponentId(10);
    link1_a->setComponentId(11);
    link2_a->setComponentId(12);
    link3_a->setComponentId(13);
    link4_a->setComponentId(14);
    link0_b->setComponentId(20);
    link1_b->setComponentId(21);
    link2_b->setComponentId(22);
    link3_b->setComponentId(23);
    link4_b->setComponentId(24);

    rpg1->setup();
    rpg2->setup();
    interface1->setup();
    interface2->setup();
    router1->setup();
    router2->setup();


    return;
}

void 
test_send_llp_to_router()
{
    GenericNetworkSource* net_source = new GenericNetworkSource();
    GenericRouter* router = new GenericRouter();
//    GenericSink* ns = new GenericSink();
    RandomPacketGenerator* ns = new RandomPacketGenerator();
    GenericInterface* my_interface = new GenericInterface();
    GenericLink* link1 = new GenericLink();
    GenericLink* link2 = new GenericLink();
    GenericLink* link3 = new GenericLink();
    GenericLink* link4 = new GenericLink();
    /* Need to model links here for both input and output connections */
    /* Set the address of each of the components ( NS ) */
    ns->interface_connections[0] = my_interface;

    /* Give the interfac a credit */
    my_interface->set_no_credits(1);
    /* Set the interface connections */
    my_interface->processor_connection = static_cast<NetworkComponent*>(ns);
    my_interface->input_connection = link3;
    my_interface->output_connection = link4;

    /* Input and output link connections */
    link1->input_connection = net_source;
    link2->input_connection = router;
    link3->input_connection = router;
    link4->input_connection = my_interface;

    link1->output_connection = router;
    link2->output_connection = net_source;
    link3->output_connection = my_interface;
    link4->output_connection = router;

    /*  Router input and output connections */
    router->output_connections.push_back(link2);
    router->output_connections.push_back(link4);
    router->input_connections.push_back(link1);
    router->input_connections.push_back(link3);
    router->set_input_ports(2);
    router->set_output_ports(2);

    net_source->output_connection = link1;
    net_source->setComponentId(0);
    router->setComponentId(1);
    my_interface->setComponentId(2);
    ns->setComponentId(3);
    link1->setComponentId(4);
    link2->setComponentId(5);
    link3->setComponentId(6);
    link4->setComponentId(7);

    my_interface->setup();
    link1->setup();
    link2->setup();
    link3->setup();
    link4->setup();
    ns->setup();
    net_source->setup();
    router->setup();

    cout << "Network sink is: " << ns->toString() << endl;
    cout << "Network source is: " << net_source->toString() << endl;
    cout << "ns interface connections " << ns->interface_connections.size() << endl;
    cout << "Router: " << router->toString() << endl;

    return;

}

int
main ( int argc, char *argv[] )
{

//    test_send_llp_to_router();
    generic_mesh_test();
    Simulator::StopAt(10000);
    Simulator::Run();
    return 0;
}				/* ----------  end of function main  ---------- */

#endif   /* ----- #ifndef _testinterface_cc_INC  ----- */

