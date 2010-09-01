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
#define MAX_SIM_TIME 1000

#include	"networkSource.h"
#include	"packetSource.h"
#include	"../components/impl/genericInterface.h"
#include	"../components/impl/genericRPG.h"
#include	"../components/impl/genericSink.h"
#include	"../components/impl/genericLink.h"
#include	"../kernel/simulator.h"

void 
test_send_llp_to_interface()
{
    GenericNetworkSource* net_source = new GenericNetworkSource();
//    GenericSink* ns = new GenericSink();
    GenericRPG* ns = new GenericRPG();
    GenericInterface* my_interface = new GenericInterface();
    GenericLink* in_link = new GenericLink();
    GenericLink* out_link = new GenericLink();
    /* Need to model links here for both input and output connections */
    /* Set the address of each of the components ( NS ) */
    ns->interface_connections[0] = my_interface;

    /* Give the interfac a credit */
    my_interface->set_no_credits(1);
    /* Set the interface connections */
    my_interface->processor_connection = static_cast<NetworkComponent*>(ns);
    my_interface->input_connection = in_link;
    my_interface->output_connection = out_link;

    /* Input and output connection */
    in_link->input_connection = net_source;
    in_link->output_connection = my_interface;
    out_link->input_connection = my_interface;
    out_link->output_connection = net_source;
    net_source->output_connection = in_link;
    net_source->setComponentId(0);
    my_interface->setComponentId(1);
    net_source->setComponentId(2);
    in_link->setComponentId(1);
    out_link->setComponentId(3);

    net_source->node_ip = 0;
    net_source->node_ip = 0;
    my_interface->node_ip = 0;

    my_interface->set_no_credits(1);
    my_interface->setup();
    in_link->setup();
    out_link->setup();
    ns->setup();
    net_source->setup();

    /* 
    cout << "Network sink is: " << ns->toString() << endl;
    cout << "Network source is: " << net_source->toString() << endl;
    cout << "ns interface connections " << ns->interface_connections.size() << endl;
    cout << "In links input connection " << in_link->input_connection->toString();
    cout << "In links output connection " << in_link->output_connection->toString();
    cout << "Out links input connection " << out_link->input_connection->toString();
    cout << "Out links output connection " << out_link->output_connection->toString();
     * */

    Simulator::StopAt(1000);
    Simulator::Run();
    return;

}

int
main ( int argc, char *argv[] )
{
    GenericNetworkSource* net_source = new GenericNetworkSource();
    GenericRPG* ns = new GenericRPG();
    GenericInterface* my_interface = new GenericInterface();
    GenericLink* in_link = new GenericLink();
    GenericLink* out_link = new GenericLink();
    /* Need to model links here for both input and output connections */
    /* Set the address of each of the components ( NS ) */
    ns->interface_connections[0] = my_interface;

    /* Give the interfac a credit */
    my_interface->set_no_credits(1);
    /* Set the interface connections */
    my_interface->processor_connection = static_cast<NetworkComponent*>(ns);
    my_interface->input_connection = out_link;
    my_interface->output_connection = in_link;

    /* Input and output connection */
    in_link->input_connection = my_interface;
    in_link->output_connection = net_source;
    out_link->input_connection = net_source;
    out_link->output_connection = my_interface;
    net_source->output_connection = out_link;
    net_source->input_connection = in_link;

    net_source->setComponentId(0);
    my_interface->setComponentId(1);
    ns->setComponentId(2);
    in_link->setComponentId(3);
    out_link->setComponentId(4);
    net_source->node_ip = 0;
    my_interface->node_ip = 0;
    ns->node_ip = 0;

    my_interface->setup();
    in_link->setup();
    out_link->setup();
    ns->init();
    net_source->setup();
    my_interface->set_no_credits(1);
    net_source->out_buffer.set_no_credits(1);

    cout << "Network sink is: " << ns->toString() << endl;
    cout << "Network source is: " << net_source->toString() << endl;
    cout << "ns interface connections " << ns->interface_connections.size() << endl;
    cout << "In links input connection " << in_link->input_connection->toString();
    cout << "In links output connection " << in_link->output_connection->toString();
    cout << "Out links input connection " << out_link->input_connection->toString();
    cout << "Out links output connection " << out_link->output_connection->toString();


    Simulator::StopAt(100);
    Simulator::Run();
    cout << "\n\n**** Interface stats ****  "
        << my_interface->print_stats()
        << endl;
    return 0;
}				/* ----------  end of function main  ---------- */

#endif   /* ----- #ifndef _testinterface_cc_INC  ----- */

