/*
 * =====================================================================================
 *
 *       Filename:  simMc2Mesh.cc
 *
 *    Description:  This is a 2x2 mesh network with 3 trace packet generators
 *    and a single memory controller
 *
 *        Version:  1.0
 *        Created:  04/20/2010 02:59:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _simmc2mesh_cc_INC
#define  _simmc2mesh_cc_INC

#include	"mesh.h"
#include	"../../simIris/data_types/impl/flit.h"
#include	"../../simIris/data_types/impl/highLevelPacket.h"
#include	"../../simIris/components/impl/genericFlatMc.h"
#include	"../../util/genericData.h"
#include	"../../util/config_params.h"
#include        "../../memctrl/mshr.cc"
#include	<string.h>
#include	<sys/time.h>
#include	<algorithm>

unsigned long int net_pack_info[8][8];

string router_model_string, mc_model_string;
Mesh* mesh;

void
dump_configuration ( void )
{
    /* Creating mesh topology with the following config */
    cerr << " vcs:\t" << vcs << endl;
    cerr << " ports:\t" << ports << endl;
    cerr << " buffer_size:\t" << buffer_size << endl;
    cerr << " credits:\t" << credits << endl;
    cerr << " no_nodes( spread over a 2D Mesh topology):\t" << no_nodes << endl;
    cerr << " grid size:\t" << grid_size << endl;
    cerr << " links:  \t" << links << endl;
    cerr << " no_of_mcs:\t" << no_mcs << endl;
    cerr << " no_of_traces:\t" << traces.size() << endl;
    cerr << " max_sim_time:\t" << max_sim_time << endl;
    cerr << " max_phy_link_bits:\t" << max_phy_link_bits << endl;
    cerr << " THREAD_BITS_POSITION:\t" << THREAD_BITS_POSITION<< endl;
    cerr << " MC_ADDR_BITS:\t" << MC_ADDR_BITS<< endl;
    cerr << " TWO_STAGE_ROUTER:\t" << do_two_stage_router << endl;
    cerr << " ROUTING_SCHEME:\t" << routing_scheme << " " << rc_method << endl;
    cerr << " SW_ARBITRATION:\t" << sw_arbitration_scheme<< " " << sw_arbitration<< endl;
    cerr << " BANK_BITS:\t" << BANK_BITS << endl;
    cerr << " NETWORK_BITS:\t" << NETWORK_ADDRESS_BITS << endl;
    cerr << " COMMAND_BITS:\t" << NETWORK_THREADID_BITS << endl;
    cerr << " COREID_BITS:\t" << NETWORK_COMMAND_BITS << endl;
    cerr << " NO_OF_THREADS:\t" << NO_OF_THREADS << endl;
    cerr << " NO_OF_RANKS:\t" << NO_OF_RANKS << endl;
    cerr << " NO_OF_BANKS:\t" << NO_OF_BANKS << endl;
    cerr << " NO_OF_ROWS:\t" << NO_OF_ROWS << endl;
    cerr << " NO_OF_COLUMNS:\t" << NO_OF_COLUMNS << endl;
    cerr << " COLUMN_SIZE:\t" << COLUMN_SIZE << endl;
    cerr << " Msg_class with arbitration priority:\t" << msg_type_string << endl;
    cerr << " Router model:\t" << router_model_string << endl;
    cerr << " MC Mode:\t" << mc_model_string << endl;

    if( traces.size() < (no_nodes - no_mcs) )
    {
        cout << " Not enough trace files for simulation " << endl;
        exit(1);
    }
    return ;
}


int
main ( int argc, char *argv[] )
{
    ullint sim_start_time = time(NULL);
    if(argc<2)
    {
        cout << "Error: Requires config file for input parameters\n";
        return 1;
    }


    for(int ii=0;ii<8;ii++)
        for(int jj=0;jj<8;jj++)
            net_pack_info[ii][jj]=0;

    ifstream fd(argv[1]);
    string data, word;
    while(!fd.eof())
    {
        getline(fd,data);
        string::size_type position = -1;
        position = data.find("#");
        istringstream iss( data, istringstream::in);
        while ( position > data.size() && iss >> word )
        {
            if ( word.compare("PRINT_SETUP") == 0)   
                iss >> print_setup;
            if ( word.compare("VCS") == 0)   
                iss >> vcs;
            if ( word.compare("PORTS") == 0) 
                iss >> ports;
            if ( word.compare("BUFFER_SIZE") == 0)
                iss >> buffer_size;
            if ( word.compare("CREDITS") == 0)
                iss >> credits;
            if ( word.compare("GRID_SIZE") == 0)
                iss >> grid_size;
            if ( word.compare("NO_OF_NODES") == 0)
                iss >> no_nodes;
            if ( word.compare("MCS") == 0)
                iss >> no_mcs;
            if ( word.compare("MAX_SIM_TIME") == 0)
                iss >> max_sim_time;
            if ( word.compare("OUTPUT_PATH") == 0)
                iss >> output_path;
            if ( word.compare("PHY_LINK_WIDTH") == 0)
                iss >> max_phy_link_bits;
            if ( word.compare("ROUTER_MODEL") == 0)
                iss >> router_model_string;
            NO_OF_THREADS = no_nodes;
            if ( word.compare("MC_MODEL") == 0)
                iss >> mc_model_string;
            /* Init parameters of mc_constants
            if ( word.compare("THREAD_BITS_POSITION") == 0)
                iss >> THREAD_BITS_POSITION;
            if ( word.compare("MC_ADDR_BITS") == 0)
                iss >> MC_ADDR_BITS;
            if ( word.compare("BANK_BITS") == 0)
                iss >> BANK_BITS;
            if ( word.compare("NO_OF_CHANNELS") == 0)
                iss >> NO_OF_CHANNELS;
                */
            if ( word.compare("NO_OF_RANKS") == 0)
                iss >> NO_OF_RANKS;
            if ( word.compare("NO_OF_BANKS") == 0)
                iss >> NO_OF_BANKS;
                
            if ( word.compare("NO_OF_ROWS") == 0)
                iss >> NO_OF_ROWS;
            if ( word.compare("NO_OF_COLUMNS") == 0)
                iss >> NO_OF_COLUMNS;
            if ( word.compare("COLUMN_SIZE") == 0)
                iss >> COLUMN_SIZE;
            if ( word.compare("MSHR_SIZE") == 0)
                iss >> MSHR_SIZE;
                
            if ( word.compare("MAX_BUFFER_SIZE") == 0)
                iss >> MAX_BUFFER_SIZE;
            if ( word.compare("MAX_CMD_BUFFER_SIZE") == 0)
                iss >> MAX_CMD_BUFFER_SIZE;
            if ( word.compare("RESPONSE_BUFFER_SIZE") == 0)
                iss >> RESPONSE_BUFFER_SIZE;
             
            if ( word.compare("NETWORK_ADDRESS_BITS") == 0)
                iss >> NETWORK_ADDRESS_BITS;
            if ( word.compare("NETWORK_THREADID_BITS") == 0)
                iss >> NETWORK_THREADID_BITS ;
            if ( word.compare("NETWORK_COMMAND_BITS") == 0)
                iss >> NETWORK_COMMAND_BITS;

            /*  ******************************************* */
            if ( word.compare("TWO_STAGE_ROUTER") == 0)
                iss >> do_two_stage_router;
            if ( word.compare("ROUTING_SCHEME") == 0)
                iss >> routing_scheme;
            if ( word.compare("SW_ARBITRATION") == 0)
                iss >> sw_arbitration_scheme;
            if ( word.compare("PRIORITY_MSG_TYPE") == 0)
                iss >> msg_type_string;
            if ( word.compare("DRAM_PAGE_POLICY") == 0)
                iss >> dram_page_policy_string;
            if ( word.compare("ADDRESS_MAP_SCHEME") == 0)
                iss >> addr_map_scheme_string;
            if ( word.compare("MC_SCHEDULING_ALGORITHM") == 0)
                iss >> mc_scheduling_algorithm_string;
            if ( word.compare("TRACE") == 0)
            {
                iss >> trace_name;
                traces.push_back(trace_name);
            }
            if ( word.compare("MC_LOC") == 0)
            {
                uint mc_xpos, mc_ypos;
                iss >> mc_xpos;
                iss >> mc_ypos;
                mc_positions.push_back(mc_xpos*grid_size + mc_ypos);
            }
        }
    }

    for ( uint i=0; i<argc; i++)
    {
        if( strcmp(argv[i],"--thread_id_bits")==0)
            THREAD_BITS_POSITION = atoi(argv[i+1]);
        if( strcmp(argv[i],"--mc_bits")==0)
            MC_ADDR_BITS = atoi(argv[i+1]);
        if( strcmp(argv[i],"--router_two_stg")==0)
            do_two_stage_router = atoi(argv[i+1]);
        if( strcmp(argv[i],"--routing_scheme")==0)
            routing_scheme = argv[i+1];
        if( strcmp(argv[i],"--sw_arbitration")==0)
            sw_arbitration_scheme = argv[i+1];
        if( strcmp(argv[i],"--msg_type")==0)
            msg_type_string = argv[i+1];
        if( strcmp(argv[i],"--bank_bits")==0)
            BANK_BITS = atoi(argv[i+1]);
    }

    if( routing_scheme.compare("odd-even") == 0)
        rc_method = ODD_EVEN;
    if( strcmp(routing_scheme.c_str(),"negative-first") == 0)
        rc_method = NEGATIVE_FIRST;
    if( strcmp(routing_scheme.c_str(),"west-first") == 0)
        rc_method = WEST_FIRST;
    if( strcmp(routing_scheme.c_str(),"north-last") == 0)
        rc_method = NORTH_LAST;
    if( strcmp(routing_scheme.c_str(),"north-last-nm") == 0)
        rc_method = NORTH_LAST_NON_MINIMAL;
    if( routing_scheme.compare("xy") == 0)
        rc_method = XY;

    if( strcmp(sw_arbitration_scheme.c_str(),"round-robin") == 0)
        sw_arbitration = ROUND_ROBIN;
    if( strcmp(sw_arbitration_scheme.c_str(),"round-robin-p") == 0)
        sw_arbitration = ROUND_ROBIN_PRIORITY;
    if( strcmp(sw_arbitration_scheme.c_str(),"fcfs") == 0)
        sw_arbitration = FCFS;

    if( strcmp(msg_type_string.c_str(),"PRIORITY_REQ") == 0)
        priority_msg_type = PRIORITY_REQ;
    if( strcmp(msg_type_string.c_str(),"ONE_FLIT_REQ") == 0)
        priority_msg_type = ONE_FLIT_REQ;
    if( strcmp(msg_type_string.c_str(),"RESPONSE_PKT") == 0)
        priority_msg_type = RESPONSE_PKT;
 
    if( dram_page_policy_string.compare("OPEN_PAGE_POLICY") == 0)
        dram_page_policy = OPEN_PAGE_POLICY;
    if( dram_page_policy_string.compare("CLOSE_PAGE_POLICY") == 0)
        dram_page_policy = CLOSE_PAGE_POLICY;
    
    if( mc_scheduling_algorithm_string.compare("FR_FCFS") == 0)
        mc_scheduling_algorithm = FR_FCFS;
    if( mc_scheduling_algorithm_string.compare("FC_FS") == 0)
        mc_scheduling_algorithm = FC_FS;
    if( mc_scheduling_algorithm_string.compare("PAR_BS") == 0)
        mc_scheduling_algorithm = PAR_BS;
    
    if( addr_map_scheme_string.compare("PAGE_INTERLEAVING") == 0)
        addr_map_scheme = PAGE_INTERLEAVING;
    if( addr_map_scheme_string.compare("PERMUTATION") == 0)
        addr_map_scheme = PERMUTATION;
    if( addr_map_scheme_string.compare("CACHELINE_INTERLEAVING") == 0)
        addr_map_scheme = CACHELINE_INTERLEAVING;
    if( addr_map_scheme_string.compare("GENERIC") == 0)
        addr_map_scheme = GENERIC;
    if( addr_map_scheme_string.compare("NO_SCHEME") == 0)
        addr_map_scheme = NO_SCHEME;
    if( addr_map_scheme_string.compare("LOCAL_ADDR_MAP") == 0)
        addr_map_scheme = LOCAL_ADDR_MAP;
 
    if( router_model_string.compare("VIRTUAL_4STAGE") == 0)
        router_model = VIRTUAL_4STAGE;
    if( router_model_string.compare("PHYSICAL_3STAGE") == 0)
        router_model = PHYSICAL_3STAGE;
    if( mc_model_string.compare("GENERIC_MC") == 0)
        mc_model = GENERIC_MC;
    if( mc_model_string.compare("FLAT_MC") == 0)
        mc_model = FLAT_MC;

    /* Number of MC's and the size of the position vector should be the same. */
        assert(mc_positions.size() == no_mcs);

    /* Compute additional parameters */
    links = (ports + (grid_size -1)*(ports-1)) + ( (ports-1) + (grid_size -1)*(ports-2))*(grid_size-1);
    fd.close();

    cerr << "\n-----------------------------------------------------------------------------------\n";
    cerr << "** CAPSTONE - Cycle Accurate Parallel Simulator Technologgy for On-Chip Networks **\n";
    cerr << " This is simIris. A second version of Capstone." << endl;
    cerr << "-- Computer Architecture and Systems Lab                                         --\n"
        << "-- Georgia Institute of Technology                                               --\n"
        << "-----------------------------------------------------------------------------------\n";
    cerr << "Cmd line: ";
    for( int i=0; i<argc; i++)
        cerr << argv[i] << " ";
    cerr << endl;

    dump_configuration();
    init_dram_timing_parameters();
    mesh = new Mesh();
    mesh->init( ports, vcs, credits, buffer_size, no_nodes, grid_size, links);
    mesh->max_sim_time = max_sim_time;

    /* Create the mesh->routers and mesh->interfaces */
    for( uint i=0; i<no_nodes; i++)
    {
        switch ( router_model )
        {
            case PHYSICAL_3STAGE:
                mesh->routers.push_back( new GenericRouter3Stg());
                break;
            case VIRTUAL_4STAGE:
                mesh->routers.push_back( new GenericRouter4Stg());
                break;
            default:
                cout << " Incorrect router model " << endl;
                exit(1);
                break;
        }
        mesh->interfaces.push_back ( new GenericInterfaceVcs());
    }

    /*  Create the TPG and mc modules */
    vector<uint>::iterator itr;
    for( uint i=0; i<no_nodes; i++)
    {
        itr = find(mc_positions.begin(), mc_positions.end(), i);
        if( itr != mc_positions.end())
        {
            switch ( mc_model )
            {
                case GENERIC_MC:
                    mesh->processors.push_back( new NI() );
                    break;
                case FLAT_MC:
                    mesh->processors.push_back( new GenericFlatMc());
                    break;
                default:
                    cout << " Unknown MC model " << endl;
                    exit(1);
                    break;
            }
        }
        else
        {
            mesh->processors.push_back( new GenericTPGVcs() );
            static_cast<GenericTPGVcs*>(mesh->processors[i])->set_trace_filename(traces[i]);
            for ( uint j=0; j<mc_positions.size(); j++)
            {
                static_cast<GenericTPGVcs*>(mesh->processors[i])->mc_node_ip.push_back(mc_positions[j]);;
            }
        }
    }

    /* Create the links */
    for ( uint i=0; i<links; i++)
    { 
        mesh->link_a.push_back(new GenericLink());
        mesh->link_b.push_back(new GenericLink());
    }

    mesh->connect_interface_processor();

    /* Set all the component ids */
    uint comp_id = 0, alink_comp_id = 1000, blink_comp_id = 5000;
    for ( uint i=0 ; i<no_nodes; i++ )
    {
        mesh->processors[i]->setComponentId(comp_id++);
        //        mesh->processors[i]->my_mesh = (void*)mesh;
        mesh->interfaces[i]->setComponentId(comp_id++);
        mesh->routers[i]->setComponentId(comp_id++);
    }

    for ( uint i=0 ; i<links; i++ )
    {
        mesh->link_a[i]->setComponentId(alink_comp_id++);
        mesh->link_b[i]->setComponentId(blink_comp_id++);
        mesh->link_a[i]->setup();
        mesh->link_b[i]->setup();
    }
    cout << " ******************** SETUP COMPLETE " << endl;
    /*  Set up the node ips for components */
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        mesh->interfaces[i]->node_ip = i;
        mesh->routers[i]->node_ip = i;
        mesh->processors[i]->node_ip = i;
    }

    /* Set the number of ports, vcs, credits and buffer sizes for the
     * components */
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        mesh->interfaces[i]->set_no_vcs(vcs);
        mesh->interfaces[i]->set_no_credits(credits);
        mesh->interfaces[i]->set_buffer_size(credits);
        //        mesh->processors[i]->set_output_path(output_path);
    }

    mesh->setup();
    for ( uint i=0 ; i<no_nodes ; i++ )
        mesh->processors[i]->set_output_path(output_path);

    /*  Set no of ports and positions for routing */
    vector< uint > grid_x;
    vector< uint > grid_y;
    grid_x.resize(no_nodes);
    grid_y.resize(no_nodes);

    /* Limitation of only modelling squares */
    for ( uint i=0 ; i<grid_size ; i++ )
        for ( uint j=0 ; j<grid_size ; j++ )
        {
            grid_x[(i*grid_size)+j] = j;
            grid_y[(i*grid_size)+j] = i;
        }


    for ( uint i=0 ; i<no_nodes ; i++ )
        mesh->routers[i]->set_no_nodes(no_nodes);

    for ( uint i=0 ; i<no_nodes ; i++ )
        for( uint j=0; j < ports ; j++)
            for( uint k=0; k < no_nodes ; k++) // Assuming is a square mesh. 
            {
                static_cast<Router*>(mesh->routers[i])->set_grid_x_location(j,k, grid_x[k]);
                static_cast<Router*>(mesh->routers[i])->set_grid_y_location(j,k, grid_y[k]);
            }

    mesh->connect_interface_routers();
    mesh->connect_routers();

    /*  Printing out all component after setup */
    if( print_setup )
    {
        for ( uint i=0 ; i<no_nodes; i++ )
        {
            cout << "\nTPG: " << i << " " << mesh->processors[i]->toString();
            cout << "\ninterface: " << i << " " << mesh->interfaces[i]->toString();
            cout << "\nrouter: " << i << " " << mesh->routers[i]->toString();
        }

        for ( uint i=0 ; i<links ; i++ )
        {
            cout << "\nlinka_" << i << " " << mesh->link_a[i]->toString();
            cout << "\nlinkb_" << i << " " << mesh->link_b[i]->toString();
        }
    }

    Simulator::StopAt(max_sim_time);
    Simulator::Run();

    cerr << mesh->print_stats();
    ullint total_link_utilization = 0;
    uint no_of_non_zero_util_links = 0;
    uint edge_links = 0;
    for ( uint i=0 ; i<links ; i++ )
    {
        double link_util = mesh->link_a[i]->get_flits_utilization();
        if( link_util )
            no_of_non_zero_util_links++;
        if( mesh->link_a[i]->input_connection == NULL || mesh->link_a[i]->output_connection == NULL )
            edge_links++;

        total_link_utilization += (ullint)link_util;
    }

    for ( uint i=0 ; i<links ; i++ )
    {
        double link_util = mesh->link_b[i]->get_flits_utilization();
        total_link_utilization += (ullint)link_util;
        if( link_util )
            no_of_non_zero_util_links++;
        if( mesh->link_b[i]->input_connection == NULL || mesh->link_b[i]->output_connection == NULL )
            edge_links++;
    }

    double bytes_delivered = (total_link_utilization + 0.0 )*max_phy_link_bits/(8*max_sim_time*0.33e-9); /* Assuming 1flit=1phit */
    double available_bw = ((2*links - edge_links )*max_phy_link_bits + 0.0)/(8*0.33e-9);
    cerr << "\n\n************** Link Stats ***************\n";
    cerr << " Total flits passed on the links: " << total_link_utilization << endl;
    cerr << " Links used: " << no_of_non_zero_util_links << " of " << 2*links - edge_links << " links." << endl;
    cerr << " Avg flits per link used " << (total_link_utilization+0.0)/no_of_non_zero_util_links << endl;
    cerr << " Net bytes delivered:(A) " << bytes_delivered*1e-9 << " MB. " << endl;
    cerr << " Max BW available in the network:(B) " << available_bw*1e-9 << " MB. " << endl;
    cerr << " \% A/B " << bytes_delivered/available_bw << endl;

    ullint tot_pkts = 0, tot_pkts_out = 0, tot_flits = 0;
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        tot_pkts_out += mesh->interfaces[i]->get_packets_out();
        tot_pkts += mesh->interfaces[i]->get_packets();
        tot_flits += mesh->interfaces[i]->get_flits_out();
    }

    ullint sim_time_ms = (time(NULL) - sim_start_time);
    cerr << "\n\n************** Simulation Stats ***************\n";
    cerr << " Simulation Time: " << sim_time_ms << " s. " << endl;
    cerr << " No of pkts per s: " << (tot_pkts+0.0)/sim_time_ms<< endl;
    cerr << " No of flits per s: " << (tot_flits+0.0)/sim_time_ms << endl;
    cerr << " Total Mem Req serviced: " << tot_pkts_out/2 << endl;
    cerr << "------------ End SimIris ---------------------" << endl;

    delete mesh;

    return 0;
}				/* ----------  end of function main  ---------- */


#endif   /* ----- #ifndef _simmc2mesh_cc_INC  ----- */
