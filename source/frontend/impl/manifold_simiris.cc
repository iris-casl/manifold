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

#include	"topology.h"
#include	"mesh.h"
#include	"torus.h"
#include	"visual.h"
#include	"../../simIris/data_types/impl/flit.h"
#include	"../../util/stats.h"
#include	"../../simIris/data_types/impl/highLevelPacket.h"
#include	"../../util/genericData.h"
#include	"../../util/config_params.h"
#include        "../../memctrl/mshr.cc"
#include	<string.h>
#include	<sys/time.h>
#include	<algorithm>

unsigned long int net_pack_info[8][8];

extern void interface_simiris(void);

Topology * topology_ptr = NULL;
string router_model_string, mc_model_string, terminal_model_string;

void
dump_configuration ( void )
{
    /* Creating mesh topology with the following config */
    cerr << " type:\t" << network_type << endl;
    cerr << " vcs:\t" << vcs << endl;
    cerr << " ports:\t" << ports << endl;
    cerr << " buffer_size:\t" << buffer_size << endl;
    cerr << " credits:\t" << credits << endl;
    cerr << " no_nodes( spread over a 2D Mesh topology):\t" << no_nodes << endl;
    cerr << " grid size:\t" << grid_size << endl;
    cerr << " links:  \t" << links << endl;
    cerr << " no_of_mcs:\t" << no_mcs << endl;
    cerr << " no_of_cores:\t" << no_of_cores << endl;
    cerr << " concentration:\t" << concentration << endl;
    cerr << " no_of_traces:\t" << traces.size() << endl;
    cerr << " interfaces:\t" << concentration << endl;
    cerr << " cores:\t" << no_of_cores << endl;
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
    cerr << " NO_OF_CHANNELS:\t" << NO_OF_RANKS << endl;
    cerr << " NO_OF_RANKS:\t" << NO_OF_RANKS << endl;
    cerr << " NO_OF_BANKS:\t" << NO_OF_BANKS << endl;
    cerr << " NO_OF_ROWS:\t" << NO_OF_ROWS << endl;
    cerr << " NO_OF_COLUMNS:\t" << NO_OF_COLUMNS << endl;
    cerr << " COLUMN_SIZE:\t" << COLUMN_SIZE << endl;
    cerr << " Router Model String:\t" << router_model_string<< endl;
    cerr << " mc_model :\t" << mc_model_string<< endl;
    cerr << " terminal_model :\t" << terminal_model_string<< endl;
    cerr << " Msg_class with arbitration priority:\t" << msg_type_string << endl;

    if( traces.size() < (no_nodes - no_mcs) )
    {
        cout << " Not enough trace files for simulation " << endl;
        exit(1);
    }
    return ;
}


IrisStats* istat = new IrisStats();
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
	    if ( word.compare("TYPE") == 0)
		iss >> network_type;
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
            if ( word.compare("CORES_PER_NODE") == 0)
                iss >> no_of_cores;
            if ( word.compare("CONCENTRATION") == 0)
                iss >> concentration;
            if ( word.compare("MAX_SIM_TIME") == 0)
                iss >> max_sim_time;
            if ( word.compare("OUTPUT_PATH") == 0)
                iss >> output_path;
            if ( word.compare("PHY_LINK_WIDTH") == 0)
                iss >> max_phy_link_bits;
            if ( word.compare("ROUTER_MODEL") == 0)
                iss >> router_model_string;
            NO_OF_THREADS = no_nodes;
            /* Init parameters of mc_constants*/
            if ( word.compare("MC_MODEL") == 0)
                iss >> mc_model_string;
            if ( word.compare("TERMINAL_MODEL") == 0)
                iss >> terminal_model_string;
            if ( word.compare("THREAD_BITS_POSITION") == 0)
                iss >> THREAD_BITS_POSITION;
            if ( word.compare("MC_ADDR_BITS") == 0)
                iss >> MC_ADDR_BITS;
            if ( word.compare("BANK_BITS") == 0)
                iss >> BANK_BITS;
            if ( word.compare("NO_OF_CHANNELS") == 0)
                iss >> NO_OF_CHANNELS;
                
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

    if( terminal_model_string.compare("GENERIC") == 0)
        terminal_model = GENERIC_PKTGEN;
    if( terminal_model_string.compare("TPG") == 0)
        terminal_model = TPG;

    /* Number of MC's and the size of the position vector should be the same. */
        assert(mc_positions.size() == no_mcs);

    /* Compute additional parameters */
        uint edge_links = 0;
    if ( network_type == "MESH" || network_type == "Mesh" || network_type == "mesh")
    {
//	links = (ports + (grid_size -1)*(ports-1)) + ( (ports-1) + (grid_size -1)*(ports-2))*(grid_size-1);
	/* For a 2D mesh [Gs*(GS-1)*k] internal links + edge links [4*GS] +
	terminal connections [GS*GS] */
	links = (grid_size * (grid_size-1) *2 /*k*/ ) + (grid_size*4 /* 2D mesh as 4 edges */ )+(grid_size*grid_size);
        edge_links = grid_size*4;
	cout << "Links = " << links << endl;
    }
    else if (network_type == "TORUS" || network_type == "Torus" || network_type == "torus" )
    {
	links = grid_size * grid_size * ports ;
	cout << "Links = " << links << endl;
    }
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
    if ( network_type == "MESH" || network_type == "Mesh" || network_type == "mesh" )
	topology_ptr = new Mesh();
    else if ( network_type == "TORUS" || network_type == "Torus" || network_type == "torus" )
	topology_ptr = new Torus();
    else if (network_type == "NONE" )
    {
	cout << "Topology not specified...exiting \n" ;
	exit(1);
    }
    topology_ptr->init( ports, vcs, credits, buffer_size, no_nodes, grid_size, links);
    topology_ptr->max_sim_time = max_sim_time;

    Visual* vis = new Visual(topology_ptr, no_nodes, links, grid_size);
    
    /* Create the mesh->routers and mesh->interfaces */
    for( uint i=0; i<no_nodes; i++)
    {
        switch ( router_model )
        {
            case PHYSICAL_3STAGE:
                topology_ptr->routers.push_back( new GenericRouter3Stg());
                break;
            case VIRTUAL_4STAGE:
                topology_ptr->routers.push_back( new GenericRouter4Stg());
                break;
            default:
                cout << " Incorrect router model " << endl;
                exit(1);
                break;
        }
        topology_ptr->interfaces.push_back ( new GenericInterfaceVcs());
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
                    topology_ptr->processors.push_back( new NI() );
                    break;
                case FLAT_MC:
                    topology_ptr->processors.push_back( new GenericFlatMc());
                    break;
                default:
                    cout << " Unknown MC model " << endl;
                    exit(1);
                    break;
            }
        }
        else
        {
            switch ( terminal_model )
            {
                case GENERIC_PKTGEN:
                    topology_ptr->processors.push_back( new GenericPktGen() );
                    break;
                case TPG:
                    topology_ptr->processors.push_back( new GenericTracePktGen() );
                    static_cast<GenericTracePktGen*>(topology_ptr->processors[i])->set_trace_filename(traces[i]);
                    for ( uint j=0; j<mc_positions.size(); j++)
                    {
                        static_cast<GenericTracePktGen*>(topology_ptr->processors[i])->mc_node_ip.push_back(mc_positions[j]);;
                    }
                    break;
                default:
                    cout << " Unknown Terminal model " << endl;
                    exit(1);
                    break;

            }
        }
    }

    /* Create the links */
    for ( uint i=0; i<links; i++)
    { 
        topology_ptr->link_a.push_back(new GenericLink());
        topology_ptr->link_b.push_back(new GenericLink());
    }

    topology_ptr->connect_interface_processor();

    /* Set all the component ids */
    uint comp_id = 0, alink_comp_id = 1000, blink_comp_id = 5000;
    for ( uint i=0 ; i<no_nodes; i++ )
    {
        topology_ptr->processors[i]->setComponentId(comp_id++);
        //        topology_ptr->processors[i]->my_mesh = (void*)topology_ptr;
        topology_ptr->interfaces[i]->setComponentId(comp_id++);
        topology_ptr->routers[i]->setComponentId(comp_id++);
    }

    for ( uint i=0 ; i<links; i++ )
    {
        topology_ptr->link_a[i]->setComponentId(alink_comp_id++);
        topology_ptr->link_b[i]->setComponentId(blink_comp_id++);
        topology_ptr->link_a[i]->setup();
        topology_ptr->link_b[i]->setup();
    }
    cout << " ******************** SETUP COMPLETE *****************\n" << endl;
    /*  Set up the node ips for components */
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        topology_ptr->interfaces[i]->node_ip = i;
        topology_ptr->routers[i]->node_ip = i;
        topology_ptr->processors[i]->node_ip = i;
    }

    /* Set the number of ports, vcs, credits and buffer sizes for the
     * components */
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        topology_ptr->interfaces[i]->set_no_vcs(vcs);
        topology_ptr->interfaces[i]->set_no_credits(credits);
        topology_ptr->interfaces[i]->set_buffer_size(credits);
        //        topology_ptr->processors[i]->set_output_path(output_path);
    }

    topology_ptr->setup();
    istat->init();
    for ( uint i=0 ; i<no_nodes ; i++ )
        topology_ptr->processors[i]->set_output_path(output_path);

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
        topology_ptr->routers[i]->set_no_nodes(no_nodes);

    for ( uint i=0 ; i<no_nodes ; i++ )
        for( uint j=0; j < ports ; j++)
            for( uint k=0; k < no_nodes ; k++) // Assuming is a square topology_ptr. 
            {
                static_cast<Router*>(topology_ptr->routers[i])->set_grid_x_location(j,k, grid_x[k]);
                static_cast<Router*>(topology_ptr->routers[i])->set_grid_y_location(j,k, grid_y[k]);
            }

    topology_ptr->connect_interface_routers();
    topology_ptr->connect_routers();

    /*  Printing out all component after setup */
    if( print_setup )
    {
        for ( uint i=0 ; i<no_nodes; i++ )
        {
            cout << "\nTPG: " << i << " " << topology_ptr->processors[i]->toString();
            cout << "\ninterface: " << i << " " << topology_ptr->interfaces[i]->toString();
            cout << "\nrouter: " << i << " " << topology_ptr->routers[i]->toString();
        }

        for ( uint i=0 ; i<links ; i++ )
        {
            cout << "\nlinka_" << i << " " << topology_ptr->link_a[i]->toString();
            cout << "\nlinkb_" << i << " " << topology_ptr->link_b[i]->toString();
        }
    }

    vis->create_new_connections();
    vis->create_graphml();

    Simulator::StopAt(max_sim_time);
    Simulator::Run();

    cerr << topology_ptr->print_stats();
    /* Init McPat for Energy model and use the counters to compute energy */
    interface_simiris();
    ullint total_link_utilization = 0;
    ullint total_link_cr_utilization = 0;
    for ( uint i=0 ; i<links ; i++ )
    {
        total_link_utilization += topology_ptr->link_a[i]->get_flits_utilization();
        total_link_cr_utilization += topology_ptr->link_a[i]->get_credits_utilization();
    }

    for ( uint i=0 ; i<links ; i++ )
    {
        total_link_utilization += topology_ptr->link_b[i]->get_flits_utilization();
        total_link_cr_utilization += topology_ptr->link_a[i]->get_credits_utilization();
    }

    double bytes_delivered = (total_link_utilization + 0.0 )*max_phy_link_bits/(network_frequency*1e6); /* Assuming 1flit=1phit */
    double available_bw = (links - edge_links+0.0)*max_phy_link_bits*max_sim_time/(network_frequency*1e6);
    cerr << "\n\n************** Link Stats ***************\n";
    cerr << " Total flits passed on the links: " << total_link_utilization << endl;
    cerr << " Total credits passed on the links: " << total_link_cr_utilization << endl;
    cerr << " Links not used: " << 2*edge_links << " of " << 2*links << " links." << endl;
    cerr << " Avg flits per link used " << (total_link_utilization+0.0)/(2*(links - edge_links)) << endl;
    cerr << " Utilized BW (data only) :(A) " << bytes_delivered *1e-6<< " Mbps. " << endl;
    cerr << " Max BW available in the network:(B) " << available_bw *1e-6<< " Mbps. " << endl;
    cerr << " \% A/B " << bytes_delivered/available_bw << endl;

    ullint tot_pkts = 0, tot_pkts_out = 0, tot_flits = 0;
    for ( uint i=0 ; i<no_nodes ; i++ )
    {
        tot_pkts_out += topology_ptr->interfaces[i]->get_packets_out();
        tot_pkts += topology_ptr->interfaces[i]->get_packets();
        tot_flits += topology_ptr->interfaces[i]->get_flits_out();
    }

    ullint sim_time_ms = (time(NULL) - sim_start_time);
    cerr << "\n\n************** Simulation Stats ***************\n";
    cerr << " Simulation Time: " << sim_time_ms << " s. " << endl;
    cerr << " No of pkts/terminal/s: " << (tot_pkts+0.0)/(no_nodes*sim_time_ms)<< endl;
    cerr << " No of flits/terminal/s: " << (tot_flits+0.0)/(no_nodes*sim_time_ms) << endl;
    cerr << " Total Mem Req serviced: " << tot_pkts_out/2 << endl;
    cerr << "------------ End SimIris ---------------------" << endl;

    delete topology_ptr;

    return 0;
}				/* ----------  end of function main  ---------- */


#endif   /* ----- #ifndef _simmc2mesh_cc_INC  ----- */
