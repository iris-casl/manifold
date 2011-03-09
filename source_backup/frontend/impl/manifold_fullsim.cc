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
#include	"../../util/genericData.h"
#include        "../../util/config_params.h"
#include        "../../zesto/zesto-uncore.h"
#include	"../../simIris/components/impl/genericFlatMc.h"



/*Declarations and default values for Network parameters
unsigned int vcs=0, ports=0, buffer_size=0, credits=0;
unsigned int no_nodes=0, no_mcs=0;
unsigned long long int max_sim_time = 10000;
unsigned int do_two_stage_router = 0;
unsigned int  max_phy_link_bits = 128;
ROUTING_SCHEME rc_method = XY;
SW_ARBITRATION sw_arbitration = ROUND_ROBIN;
message_class priority_msg_type = PRIORITY_REQ;
unsigned int print_setup = 1;
unsigned int grid_size=0;
string routing_scheme, sw_arbitration_scheme;
vector<string> traces;
vector<uint> mc_positions;
string trace_name, output_path, msg_type_string;
unsigned int links;
 * */
string data, word;
unsigned int cores_per_node=1;
/*These objects are declared externally in mesh.h*/




/*The main Mesh object*/
Mesh* mesh;
void print_state_at_deadlock(void);

char *s;
int  exit_code;

/* stats signal handler */
static void
signal_sim_stats(int sigtype)
{
  sim_dump_stats = TRUE;
}

/* exit signal handler */
static void
signal_exit_now(int sigtype)
{
  sim_exit_now = TRUE;
}

/* execution start/end times */
time_t sim_start_time;
time_t sim_end_time;
int sim_elapsed_time;

/* byte/word swapping required to execute target executable on this host */
int sim_swap_bytes;
int sim_swap_words;

/* exit when this becomes non-zero */
int sim_exit_now = FALSE;

/* longjmp here when simulation is completed */
jmp_buf sim_exit_buf;

/* set to non-zero when simulator should dump statistics */
int sim_dump_stats = FALSE;

/* options database */
struct opt_odb_t *sim_odb;

/* stats database */
struct stat_sdb_t *sim_sdb;

/* redirected program/simulator output file names */
char *sim_simout = NULL;
char *sim_progout = NULL;
FILE *sim_progfd = NULL;

/* track first argument orphan, this is the program to execute */
static int exec_index = -1;

/* dump help information */
bool help_me;

/* random number generator seed */
int rand_seed;

/* initialize and quit immediately */
bool init_quit;

/* simulator scheduling priority */
int nice_priority;


static int
orphan_fn(int i, int argc, char **argv)
{
  exec_index = i;
  return /* done */FALSE;
}

static void
banner(FILE *fd, int argc, char **argv)
{
  char *s;

  fprintf(fd,
	  "%s: SimpleScalar/%s Tool Set version %d.%d of %s.\n"
	  "Copyright (C) 2000-2002 by The Regents of The University of Michigan.\n"
          "Copyright (C) 1994-2002 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.\n"
	  "This version of SimpleScalar is licensed for academic non-commercial use only.\n"
	  "\n",
	  ((s = strrchr(argv[0], '/')) ? s+1 : argv[0]),
	  VER_TARGET, VER_MAJOR, VER_MINOR, VER_UPDATE);
}


static void
usage(FILE *fd, int argc, char **argv)
{
  fprintf(fd, "Usage: %s {-options} executable {arguments}\n", argv[0]);
  opt_print_help(sim_odb, fd);
}

static int running = FALSE;

/* print all simulator stats */
void
print_state_at_deadlock(void)
{
    for ( uint i=0; i<no_nodes; i++)
    {
        static_cast<GenericRouterVcs*>(mesh->routers[i])->dump_buffer_state();
    }
    exit(1);
}

void
sim_print_stats(FILE *fd)		/* output stream */
{
  if (!running)
    return;

  /* get stats time */
  sim_end_time = time((time_t *)NULL);
  sim_elapsed_time = MAX(sim_end_time - sim_start_time, 1);

  /* print simulation stats */
  fprintf(fd, "\nsim: ** simulation statistics **\n");
  stat_print_stats(sim_sdb, fd);
  sim_aux_stats(fd);
  fprintf(fd, "\n");

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
    
  double max_net_bw = (no_of_non_zero_util_links * max_phy_link_bits*max_sim_time*1.0)/(3e9*8); /* Assuming 1flit=1phit */
    double used_bw = (total_link_utilization+0.0)*max_phy_link_bits/8;
    cerr << "\n\n************** Link Stats ***************\n";
    cerr << " max_phy_link_bits: " << max_phy_link_bits << endl;
    cerr << " max_sim_time: " << max_sim_time << endl;
    cerr << " Total flits passed on the links: " << total_link_utilization << endl;
    cerr << " Links used (L): " << no_of_non_zero_util_links << " of " << 2*links - edge_links << " links." << endl;
    cerr << " Available BW (L*ch_w/8*sim_time/freq): " << max_net_bw << "MB." << endl;
    cerr << " BW Used " << used_bw << endl;
    cerr << " total_link_utilization" << total_link_utilization << endl;
    cerr << " \% A/B " << used_bw/max_net_bw<< endl;

  unsigned long long int tot_pkts = 0, tot_pkts_out = 0, tot_flits = 0;
  for ( uint i=0 ; i<no_nodes ; i++ )
  {
      tot_pkts_out += mesh->interfaces[i]->get_packets_out();
      tot_pkts += mesh->interfaces[i]->get_packets();
      tot_flits += mesh->interfaces[i]->get_flits_out();
  }

  unsigned long long int sim_time_ms = (time(NULL) - sim_start_time);
  cerr << "\n\n************** Simulation Stats ***************\n";
  cerr << " Simulation Time: " << sim_time_ms << " s. " << endl;
  cerr << " No of pkts per s: " << (tot_pkts+0.0)/sim_time_ms<< endl;
  cerr << " No of flits per s: " << (tot_flits+0.0)/sim_time_ms << endl;

  cerr << " Total Mem Req serviced: " << tot_pkts_out/2 << endl;
  cerr << "------------ End SimIris ---------------------" << endl;

}

/* print stats, uninitialize simulator components, and exit w/ exitcode */
static void
exit_now(int exit_code)
{
  /* print simulation stats */
  sim_print_stats(stderr);

  /* un-initialize the simulator */
  sim_uninit();

  /* all done! */
  exit(exit_code);
}


unsigned int iris_process_options(int argc,char *argv[])
{
/* The following parameters must be specified in the config with the
     * correct knobs */
    unsigned int offset;    
    ifstream fd(argv[1]);
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
            if ( word.compare("CORES_PER_NODE") == 0) 
                iss >> cores_per_node;
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
    //        if ( word.compare("ROUTER_MODEL") == 0)
      //          iss >> router_model_string;
            NO_OF_THREADS = no_nodes;
            /* Init parameters of mc_constants*/
        //    if ( word.compare("MC_MODEL") == 0)
          //      iss >> mc_model_string;
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
            if ( word.compare("MC_LOC") == 0)
            {
                uint mc_xpos, mc_ypos;
                iss >> mc_xpos;
                iss >> mc_ypos;
                mc_positions.push_back(mc_xpos*grid_size + mc_ypos);
            }
            NO_OF_THREADS = no_nodes;
        }
    }

    for ( uint i=0; i<argc; i++)
    {
        if( strcmp(argv[i],"--thread_id_bits")==0)
        {
	    THREAD_BITS_POSITION = atoi(argv[i+1]);
	    continue;
	}
        if( strcmp(argv[i],"--mc_bits")==0)
        {
	    MC_ADDR_BITS = atoi(argv[i+1]);
	    continue;
	}
        if( strcmp(argv[i],"--router_two_stg")==0)
	{
            do_two_stage_router = atoi(argv[i+1]);
	    continue;
	}
        if( strcmp(argv[i],"--routing_scheme")==0)
        { 
	    routing_scheme = argv[i+1];
	    continue;
	}
        if( strcmp(argv[i],"--sw_arbitration")==0)
        {
	    sw_arbitration_scheme = argv[i+1];
	    continue;
	}
        if( strcmp(argv[i],"--msg_type")==0)
        {
	    msg_type_string = argv[i+1];
	    continue;
	}
        if( strcmp(argv[i],"--bank_bits")==0)
        {
	    BANK_BITS = atoi(argv[i+1]);
	    continue;
	}
	offset=i;
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

    /* Number of MC's and the size of the position vector should be the same. */
//    assert(mc_positions.size() == no_mcs);

    /* Compute additional parameters */
    links = (ports + (grid_size -1)*(ports-1)) + ( (ports-1) + (grid_size -1)*(ports-2))*(grid_size-1);
    fd.close();

        /* Creating mesh topology with the following config */
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
    cerr << " NO_OF_CHANNELS:\t" << NO_OF_RANKS << endl;
    cerr << " NO_OF_RANKS:\t" << NO_OF_RANKS << endl;
    cerr << " NO_OF_BANKS:\t" << NO_OF_BANKS << endl;
    cerr << " NO_OF_ROWS:\t" << NO_OF_ROWS << endl;
    cerr << " NO_OF_COLUMNS:\t" << NO_OF_COLUMNS << endl;
    cerr << " COLUMN_SIZE:\t" << COLUMN_SIZE << endl;
    cerr << " Msg_class with arbitration priority:\t" << msg_type_string << endl;
	return offset;	
}


void iris_init()
{
   
    mesh = new Mesh();
    mesh->init( ports, vcs, credits, buffer_size, no_nodes, grid_size, links);
    mesh->max_sim_time = max_sim_time;

    /* Create the mesh->routers and mesh->interfaces */
    for( uint i=0; i<no_nodes; i++)
    {
        mesh->routers.push_back( new GenericRouterVcs());
        mesh->interfaces.push_back ( new GenericInterfaceVcs());
    }

    /*  Create the mc modules and link to the uncores */
    vector<unsigned int>::iterator itr;
    /*j is the index of the uncores that have been linked in the Mesh*/
    unsigned int j=0;	
    for(unsigned int i=0; i<no_nodes; i++)
    {
        itr = find(mc_positions.begin(), mc_positions.end(), i);
        if( itr != mc_positions.end())
        {
            mesh->processors.push_back( new NI() );
        }
        else
        {
            mesh->processors.push_back( static_cast <Processor*> (uncores[j]) );
	    j++;
            for ( unsigned int jj=0; jj<mc_positions.size(); jj++)
            {
                static_cast<uncore_t*>(mesh->processors[i])->mc_node_ip.push_back(mc_positions[jj]);
            }
        }
    }

    /* Create the links connecting all nodes in the mesh */
    for (unsigned int  i=0; i<links; i++)
    { 
        mesh->link_a.push_back(new GenericLink());
        mesh->link_b.push_back(new GenericLink());
    }

    /*Coonects the interfaces to the processors which are either Memory Controller NI's or Zesto Uncores*/
    mesh->connect_interface_processor();

    /* Set all the component ids */
    unsigned int comp_id = 0, alink_comp_id = 1000, blink_comp_id = 5000;
    for ( uint i=0 ; i<no_nodes; i++ )
    {
        mesh->processors[i]->setComponentId(comp_id++);
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

    /*  Set up the node ips for components Node ip's are the actual number of the node in the mesh */
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
	//mesh->processors[i]->set_output_path(output_path);
    }

    /*Sets up the routers,links,interfaces*/
    mesh->setup();
    //for ( uint i=0 ; i<no_nodes ; i++ )
        //mesh->processors[i]->set_output_path(output_path);

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
                static_cast<GenericRouterVcs*>(mesh->routers[i])->set_grid_x_location(j,k, grid_x[k]);
                static_cast<GenericRouterVcs*>(mesh->routers[i])->set_grid_y_location(j,k, grid_y[k]);
            }

    mesh->connect_interface_routers();
    mesh->connect_routers();

    /*  Printing out all component after setup */
    if( print_setup )
    {
    //for ( uint i=0 ; i<no_nodes; i++ )
    //{
      //  cout << "\nUncore: " << i << " " << mesh->processors[i]->toString();
        //cout << "\ninterface: " << i << " " << mesh->interfaces[i]->toString();
        //cout << "\nrouter: " << i << " " << mesh->routers[i]->toString();
    //}

    //for ( uint i=0 ; i<links ; i++ )
    //{
      //  cout << "\nlinka_" << i << " " << mesh->link_a[i]->toString();
        //cout << "\nlinkb_" << i << " " << mesh->link_b[i]->toString();
    //}
    }
}




int main( int argc, char *argv[] )
{
  unsigned long long int sim_start_time = time(NULL);
  if(argc<2)
  {
      cout << "Error: Requires config file for input parameters\n";
      return 1;
  }


  /* catch SIGUSR1 and dump intermediate stats */
  signal(SIGUSR1, signal_sim_stats);

  /* catch SIGUSR2 and dump final stats and exit */
  signal(SIGUSR2, signal_exit_now);

  /* register an error handler */
  fatal_hook(sim_print_stats);


  /* set up a non-local exit point */
  if ((exit_code = setjmp(sim_exit_buf)) != 0)
  {
     /* special handling as longjmp cannot pass 0 */
     exit_now(exit_code-1);
  }


  /* Sets up default values for  core knobs*/
  sim_pre_init();

  /* register global options - Initialising the options database */
  sim_odb = opt_new(orphan_fn);

  /* register all simulator-specific options in the options database */
  sim_reg_options(sim_odb);

  /*IRIS: Processing the commandline options and config file of IRIS*/
  unsigned int offset = iris_process_options(argc,argv);
  
  /*num_threads is set by the -cores option in Zesto, we need to set it to no_nodes*cores_per_node */
  num_threads= no_nodes*cores_per_node;

  /* parse zesto simulator options, options for Network and MC are parsed ahead */
  exec_index = -1;

  /*opt_process options looks for the keyword zesto and process only the commandline after that*/
  opt_process_options(sim_odb, argc, argv );

  /* redirect I/O? */
  if (sim_simout != NULL)
    {
      /* send simulator non-interactive output (STDERR) to file SIM_SIMOUT */
      fflush(stderr);
      if (!freopen(sim_simout, "w", stderr))
	fatal("unable to redirect simulator output to file `%s'", sim_simout);
    }

  if (sim_progout != NULL)
    {
      /* redirect simulated program output to file SIM_PROGOUT */
      sim_progfd = fopen(sim_progout, "w");
      if (!sim_progfd)
	fatal("unable to redirect program output to file `%s'", sim_progout);
    }

  /* need at least two argv values to run */
  if (argc < 2)
    {
      banner(stderr, argc, argv);
      usage(stderr, argc, argv);
      exit(1);
    }

  /* opening banner */
  banner(stderr, argc, argv);

  if(help_me)
  {
    /* print help message and exit */
    usage(stderr, argc, argv);
    exit(1);
  }

  /* seed the random number generator */
  if (rand_seed == 0)
  {
    /* seed with the timer value, true random */
    mysrand(time((time_t *)NULL));
  }
  else
  {
    /* seed with default or user-specified random number generator seed */
    mysrand(rand_seed);
  }

  /* check simulator-specific options - Located in file zesto-opts.c*/
  sim_check_options(sim_odb, argc-offset, argv+offset);

  /* set simulator scheduling priority */
  if (nice(0) < nice_priority)
    {
      if (nice(nice_priority - nice(0)) < 0)
        fatal("could not renice simulator process");
    }

  /* initialize the instruction decoder - sets up macros in file:machine.c , no code executed */
  md_init_decoder();

  opt_print_options(sim_odb, stderr, /* long */TRUE, /* !notes */TRUE);

  /* initialize all simulation modules - thread structures, uncore & core structures,register files, oracle/exec/alloc/decode models for all cores */
  sim_post_init();
  
  init_dram_timing_parameters();
  /*IRIS: Instatiates the mesh,routers,MC's,interfaces and links them to the uncore*/
  iris_init();

  /* register all simulator stats */
  sim_sdb = stat_new();
  sim_reg_stats(threads,sim_sdb);

  /* record start of execution time, used in rate stats */
  sim_start_time = time((time_t *)NULL);

  /* output simulation conditions */
  //s = ctime(&sim_start_time);
  //if (s[strlen(s)-1] == '\n')
  //  s[strlen(s)-1] = '\0';
  //fprintf(stderr, "\nsim: simulation started @ %s, options follow:\n", s);
  opt_print_options(sim_odb, stderr, /* short */TRUE, /* notes */TRUE);
  sim_aux_config(stderr);
  fprintf(stderr, "\n");

  /* omit option dump time from rate stats */
  sim_start_time = time((time_t *)NULL);

  if (init_quit)
    exit_now(0);

  running = TRUE;
  sim_main();//Register the timer tick component, turn sim_main into a component

  class zesto_component *tick = new zesto_component();

  Simulator::StopAt(1000000000);
  Simulator::Run();

  sim_print_stats(stderr);

  delete mesh;

  return(0);
}				/* ----------  end of function main  ---------- */

#endif   /* ----- #ifndef _simmc2mesh_cc_INC  ----- */
