/*
 * =====================================================================================
 *
 *       Filename:  mc_constants.cc
 *
 *    Description:  This file inits all the dram parameters based on clock
 *    speeds chosen.
 *
 *        Version:  1.0
 *        Created:  08/10/2010 01:11:02 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  mc_constants_cc_INC
#define  mc_constants_cc_INC

#include	"simIrisComponentHeader.h"
#include	"genericData.h"
#include	"mc_constants.h"
#include	"stats.h"

//#define DEBUG
//#define DEEP_DEBUG
/* Generic topology parameters */
uint no_nodes = 16, 
     no_mcs = 2,
     do_two_stage_router = 0,
     max_phy_link_bits = 128,
     links = 0;
uint no_msg_classes = 1;

uint network_frequency = 1200; //MHz
uint stat_print_level = 1;

ullint max_sim_time = 10000000;

IrisStats* istat = new IrisStats();
//Sharda
string network_type = "NONE";
uint no_of_cores = 14;
uint concentration = 1;

ROUTING_SCHEME rc_method = XY;
SW_ARBITRATION sw_arbitration = ROUND_ROBIN;
ROUTER_MODEL router_model = PHYSICAL;
string router_model_string = "PHYSICAL";
MC_MODEL mc_model = SINK;
string mc_model_string = "SINK";
TERMINAL_MODEL terminal_model = GENERIC_PKTGEN;
string terminal_model_string= "GENERIC_PKTGEN";
message_class priority_msg_type = PRIORITY_REQ;
message_class terminal_msg_class = RESPONSE_PKT;
string terminal_msg_class_string = "RESPONSE_PKT";
uint print_setup = 0;
uint grid_size=4; 
const bool multiple_flit_in_buf = true;
vector<uint> mc_positions;
vector<string> traces;
uint vcs=1, ports=5, buffer_size=2, credits=2;
string trace_name, output_path, msg_type_string;
string routing_scheme, sw_arbitration_scheme;

/* TPG parameters */
uint mean_irt = 50;
uint pkt_payload_length = 128;

/* Flat mc and other mc knobs */
uint mc_response_pkt_payload_length = 512;

string addr_map_scheme_string,mc_scheduling_algorithm_string,dram_page_policy_string;
uint THREAD_BITS_POSITION = 25;
uint MC_ADDR_BITS = 12;
uint BANK_BITS = 13;
bool do_request_reply_network = false;

DRAM_CONFIG dram_config_string = DDR3_1600_10;


DRAM_PAGE_POLICY dram_page_policy = OPEN_PAGE_POLICY; 
MC_SCHEDULLING_ALGO mc_scheduling_algorithm = FR_FCFS;
ADDR_MAP_SCHEME addr_map_scheme = PAGE_INTERLEAVING;
uint NO_OF_THREADS=16;
uint MAX_BUFFER_SIZE = 8;
uint MAX_CMD_BUFFER_SIZE = 16;
uint RESPONSE_BUFFER_SIZE = 56*8; 


uint NO_OF_CHANNELS=1;		//  (int)log2() = k bits. 
uint NO_OF_RANKS=1;       		//  (int)log2() = l bits.

uint NO_OF_BANKS=8;         		//  (int)log2() = b bits. 
//   uint NO_OF_BUFFERS = NO_OF_BANKS;

uint NO_OF_ROWS = 8192; 		//4096 //  (int)log2() = r bits. 
uint NO_OF_COLUMNS = 128;         	//  (int)log2() = c bits. 
uint COLUMN_SIZE = 64;        		//  (int)log2() = v bits.  Column Size = 2bytes
/* uint BLOCKS_PER_ROW = 128;          	//  (int)log2() = n bits.  Cache line Per Row
   uint CACHE_BLOCK_SIZE = 64;         	//  (int)log2() = z bits.  L2 Cache Block Size
   uint ROW_SIZE = NO_OF_COLUMNS*COLUMN_SIZE; //(Also equal to BLOCKS_PER_ROW*CACHE_BLOCK_SIZE)
   uint DRAM_SIZE =  NO_OF_CHANNELS*NO_OF_RANKS*NO_OF_BANKS*NO_OF_ROWS*ROW_SIZE;
 */
uint NETWORK_ADDRESS_BITS = 32;
uint NETWORK_THREADID_BITS = 6;
uint NETWORK_COMMAND_BITS = 3;

uint MSHR_SIZE= 8;

float CORE_SPEED = 3000;
float CYCLE_2_NS = (CORE_SPEED*1.0 / 1000);

uint DDR_BUS_WIDTH;
float BUS_SPEED;
float MEM_SPEED;
float MEM_CYCLE;
float BUS_CYCLE;
float tREFI;
float tRFC;
float tRC;
float tRAS;
uint t_CMD;
uint t_RCD;
uint t_RRD;
uint t_RAS;
uint t_CAS;
uint t_RTRS;
uint t_OST;
uint t_WR;
uint t_WTR;
uint t_RP;
uint t_CCD;
uint t_AL;
uint t_CWD;
uint t_RC;
uint t_RTP;
uint t_RFC;

void 
init_dram_timing_parameters( void)
{
    switch ( dram_config_string )
    {      
        case DDR3_1333_9:        
            {
                //DDR3 1333 -9-9-9 Gbps memory system.  
                //Composed of 2 Gbit chips.  2 ranks, each rank has 8 2Gbit(x8) chips.
                //This is a 64 bit wide interface.
                //Total is 4096 MB
                //Bus Bandwidth is  10.667 GB/s
                //9-9-9 means 9*2 DDR half cycles for t_CAS and (9 / 667) = 13.5ns for t_RCD, t_RP
                DDR_BUS_WIDTH = 8;
                BUS_SPEED = 1333;
                MEM_SPEED = 667;
                MEM_CYCLE = (CORE_SPEED*1.0 / MEM_SPEED);
                BUS_CYCLE = (CORE_SPEED*1.0 / BUS_SPEED);

                tREFI = 7.8;
                tRFC = 160;
                tRC = 49.5;
                tRAS = 36;

                t_CMD = ceil (1.0 * BUS_CYCLE);
                t_RCD = ceil (13.5 * CYCLE_2_NS);
                t_RRD = ceil (7.5 * CYCLE_2_NS);
                t_RAS = ceil (36 * CYCLE_2_NS);
                t_CAS = ceil (9.0 * MEM_CYCLE);
                t_RTRS = ceil (1.0 * MEM_CYCLE);
                t_OST = ceil (1.0 * MEM_CYCLE);
                t_WR = ceil (15 * CYCLE_2_NS);
                t_WTR = ceil (7.5 * CYCLE_2_NS);
                t_RP = ceil (13.5 * CYCLE_2_NS);
                t_CCD = ceil (DDR_BUS_WIDTH/2 * MEM_CYCLE); 
                t_AL = 0;
                t_CWD = (ullint)(t_CAS-t_CMD);
                t_RC = ceil (49.5 * CYCLE_2_NS);
                t_RTP = ceil (7.5 * CYCLE_2_NS);
                t_RFC = ceil (160 * CYCLE_2_NS);
            }
            break;
        case DDR3_1600_10:        
            {
                //  DDR3 1600 -10-10-10 Gbps memory system.  
                //  Composed of 2 Gbit chips.  2 ranks, each rank has 8 2Gbit(x8) chips.
                //  This is a 64 bit wide interface.
                //  Total is 4096 MB
                //  Bus Bandwidth is  14.4 GB/s
                //  10-10-10 means 10*2 DDR half cycles for t_CAS and (10 / 800) = 12.5ns for t_RCD, t_RP
                DDR_BUS_WIDTH = 8;		// 64 bit = 8 bytes
                BUS_SPEED = 1600;		
                MEM_SPEED = 800;
                MEM_CYCLE = (CORE_SPEED*1.0 / MEM_SPEED);	
                BUS_CYCLE = (CORE_SPEED*1.0 / BUS_SPEED);

                tREFI = 7.8;
                tRFC = 160;
                tRC = 47.5;
                tRAS = 35;

                t_CMD 	= (ullint)ceil (1.0 * BUS_CYCLE);
                t_RCD 	= (ullint)ceil (12.5 * CYCLE_2_NS);	        // Taken from datasheet
                t_RRD 	= (ullint)ceil (6.0 * CYCLE_2_NS);		// Taken from datasheet
                t_RAS 	= (ullint)ceil (35 * CYCLE_2_NS);		// Taken from datasheet
                t_CAS 	= (ullint)ceil (10.0 * MEM_CYCLE);		// 10-10-10 So 10 Mem Cycles
                t_RTRS 	= (ullint)ceil (1.0 * MEM_CYCLE);
                t_OST 	= (ullint)ceil (1.0 * MEM_CYCLE);
                t_WR 	= (ullint)ceil (15 * CYCLE_2_NS);		// Taken from datasheet
                t_WTR 	= (ullint)ceil (7.5 * CYCLE_2_NS);		// Taken from datasheet
                t_RP 	= (ullint)ceil (12.5 * CYCLE_2_NS);	        // Taken from datasheet
                t_CCD 	= (ullint)ceil (DDR_BUS_WIDTH/2 * MEM_CYCLE);    // Taken from datasheet
                t_AL 	= 0;				                // No posted CAS will have t_AL = 0
                t_CWD 	= t_CAS-t_CMD;
                t_RC 	= (ullint)ceil (47.5 * CYCLE_2_NS);	        // Taken from datasheet
                t_RTP 	= (ullint)ceil (7.5 * CYCLE_2_NS);		// Taken from datasheet
                t_RFC 	= (ullint)ceil (160 * CYCLE_2_NS);		// Taken from datasheet
            }
            break;

        case DDR3_1333_6:       
            {
                //  DDR3 1333 -6-6-6 Gbps memory system.  
                //  Composed of 2 Gbit chips.  2 ranks, each rank has 8 2Gbit(x8) chips.
                //  This is a 64 bit wide interface.
                //  Total is 4096 MB
                //  Bus Bandwidth is  10.667 GB/s
                //  6-6-6 means 6*2 DDR half cycles for t_CAS and (6 / 667) = 9ns for t_RCD, t_RP
                DDR_BUS_WIDTH = 8;		// 64 bit = 8 bytes
                BUS_SPEED = 1333;		
                MEM_SPEED = 667;
                MEM_CYCLE = (CORE_SPEED*1.0 / MEM_SPEED);	
                BUS_CYCLE = (CORE_SPEED*1.0 / BUS_SPEED);

                t_CMD 	= ceil (1.0 * BUS_CYCLE);
                t_RCD 	= ceil (9 * CYCLE_2_NS);		// Taken from DRAMSim spd
                t_RRD 	= ceil (6 * CYCLE_2_NS);		// TODO FIXME In most cases doesn't matter
                t_RAS 	= ceil (30 * CYCLE_2_NS);		// Taken from DRAMSim spd
                t_CAS 	= ceil (6.0 * MEM_CYCLE);		// 6-6-6 So 6 Mem Cycles
                t_RTRS 	= ceil (1.0 * MEM_CYCLE);
                t_OST 	= ceil (1.0 * MEM_CYCLE);
                t_WR 	= ceil (10 * CYCLE_2_NS);		// Taken from DRAMSim spd
                t_WTR 	= ceil (t_WR / 2);			// TODO t_WTR not defined and I took average of 0 and t_WR
                t_RP 	= ceil (9 * CYCLE_2_NS);		// Taken from DRAMSim spd
                t_CCD 	= ceil (DDR_BUS_WIDTH/2 * MEM_CYCLE);   // TODO Bus Cycle = Half Mem Cycle. burst of 8 beats
                t_AL 	= 0;			                // No posted CAS will have t_AL = 0
                t_CWD 	= t_CAS-t_CMD;
                t_RC 	= ceil (40.5 * CYCLE_2_NS);	        // Taken from DRAMSim spd
                t_RTP 	= ceil (3.0 * MEM_CYCLE);	        // TODO don't know yet. default in DRAMSim for DDR2 & DDR3 
                t_RFC 	= ceil (210 * CYCLE_2_NS);		// FIXME Taken from DRAMSim spd
            }
            break;
        case DDR2_533_4:       
            {
                //  DDR2 533 4-4-4 Mbps memory system. 
                //  Composed of 72 512 Mbit chips. 2 ranks   
                //  Total is 4 GB
                //  Bus Bandwidth is 4.3 GB/s
                //  4-4-4 means 4*2 DDR half cycles for t_CAS and (4 / 266); = 15ns for t_RCD, t_RP

                //  Corresponds to MT36HTJ51272Y-53E 4 GB part from Micron, aka PC2-4200 DRAM
                // non-registered mode (doesn't add a cycle);
                DDR_BUS_WIDTH = 8;		// 64 bit = 8 bytes
                BUS_SPEED = 533;		
                MEM_SPEED = 266;		//  artifact of not knowing correct timing values
                MEM_CYCLE = (CORE_SPEED*1.0 / MEM_SPEED);	// artifact of not knowing correct timing values
                BUS_CYCLE = (CORE_SPEED*1.0 / BUS_SPEED);

                t_CMD 	= ceil (1.0 * BUS_CYCLE);
                t_RCD 	= ceil (15 * CYCLE_2_NS);	// Taken from datasheet
                t_RRD 	= ceil (7.5 * CYCLE_2_NS);	// TODO Generic for many DDR2. Taken from Data Sheet of some other part
                t_RAS 	= ceil (40 * CYCLE_2_NS);	// t_RAS = t_RC - t_RCD 
                t_CAS 	= ceil (4.0 * MEM_CYCLE);	// 4-4-4 So 4 Mem Cycles
                t_RTRS 	= ceil (1.0 * MEM_CYCLE);
                t_OST 	= ceil (1.0 * MEM_CYCLE);
                t_WR 	= ceil (15 * CYCLE_2_NS);	// Taken from component data sheet
                t_WTR 	= ceil (10 * CYCLE_2_NS);	// TODO Generic for many DDR2. Taken from Data Sheet of some other part
                t_RP 	= ceil (15 * CYCLE_2_NS);	// Taken from datasheet
                t_CCD 	= ceil (2.0 * MEM_CYCLE);	// TODO Generic for many DDR2. Taken from Data Sheet of some other part
                t_AL 	= 0;			        // No posted CAS will have t_AL = 0
                t_CWD 	= t_CAS-t_CMD;
                t_RC 	= ceil (55 * CYCLE_2_NS);	// Taken from datasheet
                t_RTP 	= ceil (7.5 * CYCLE_2_NS);	// TODO Generic for many DDR2. Taken from Data Sheet of some other part 
                t_RFC 	= ceil (105 * CYCLE_2_NS);	// Taken from datasheet
            }
            break;

        case DDR2_667_4:       
            {
                //  DDR2 667 4-4-4 Mbps memory system.  
                //  Composed of 1 Gbit chips.  1 rank, each rank has 5 1 Gbit (x8) chips.
                //  The 5th chip is for ECC, otherwise this is a 32 bit wide interface.
                //  Total is 512 MB
                //  Bus Bandwidth is 2.67 GB/s
                //  4-4-4 means 4*2 DDR half cycles for t_CAS and (4 / 333M) = 12ns for t_RCD, t_RP

                DDR_BUS_WIDTH = 4;	                      	// 32 bit = 4 bytes
                BUS_SPEED = 667;		
                MEM_SPEED = 333;                     		//  artifact of not knowing correct timing values
                MEM_CYCLE = (CORE_SPEED*1.0 / MEM_SPEED);	        // artifact of not knowing correct timing values
                BUS_CYCLE = (CORE_SPEED*1.0 / BUS_SPEED);

                t_CMD 	= ceil (1.0 * BUS_CYCLE);
                t_RCD 	= ceil (12 * CYCLE_2_NS);		// Taken from DRAMSim spd
                t_RRD 	= ceil (6 * CYCLE_2_NS);		// TODO FIXME In most cases doesn't matter
                t_RAS 	= ceil (45 * CYCLE_2_NS);		// Taken from DRAMSim spd
                t_CAS 	= ceil (4.0 * MEM_CYCLE);	        // 4-4-4 So 4 Mem Cycles
                t_RTRS 	= ceil (1.0 * MEM_CYCLE);
                t_OST 	= ceil (1.0 * MEM_CYCLE);
                t_WR 	= ceil (15 * CYCLE_2_NS);		// Taken from DRAMSim spd
                t_WTR 	= ceil (t_WR / 2);			// TODO FIXME  t_WTR not defined and I took average of 0 and t_WR
                t_RP 	= ceil (12 * CYCLE_2_NS);		// Taken from DRAMSim spd
                t_CCD 	= ceil (DDR_BUS_WIDTH/2 * MEM_CYCLE);   // TODO Bus Cycle = Half Mem Cycle. burst of 4 beats
                t_AL 	= 0;            			// No posted CAS will have t_AL = 0
                t_CWD 	= t_CAS-t_CMD;
                t_RC 	= ceil (57 * CYCLE_2_NS);		// Taken from DRAMSim spd
                t_RTP 	= ceil (3.0 * MEM_CYCLE);		// TODO don't know yet. default in DRAMSim for DDR2 & DDR3
                t_RFC 	= ceil (127.5 * CYCLE_2_NS);	        // FIXME Taken from DRAMSim spd
            }
            break;
    }
}
#endif   /* ----- #ifndef mc_constants_cc_INC  ----- */

