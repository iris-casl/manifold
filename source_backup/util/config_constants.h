/*
 * =====================================================================================
 *
 *       Filename:  constants.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/25/2010 11:18:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  SYED MINHAJ HASSAN
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef _2CONSTANTS_H
#define _2CONSTANTS_H
#include <math.h>
#include <stdint.h>
#include "simIrisComponentHeader.h"

typedef unsigned long long int Time;
typedef unsigned long long int Addr_t;
typedef unsigned int UInt;

enum DRAM_PAGE_POLICY { OPEN_PAGE_POLICY, CLOSE_PAGE_POLICY};
enum MC_SCHEDULLING_ALGO { PAR_BS, FR_FCFS, FC_FS, NFQ};
enum ADDR_MAP_SCHEME { PAGE_INTERLEAVING, PERMUTATION, CACHELINE_INTERLEAVING, SWAPPING, GENERIC, NO_SCHEME, LOCAL_ADDR_MAP};

DRAM_PAGE_POLICY dram_page_policy = OPEN_PAGE_POLICY; 
MC_SCHEDULLING_ALGO mc_scheduling_algorithm = FR_FCFS;
ADDR_MAP_SCHEME addr_map_scheme = PAGE_INTERLEAVING;

unsigned int THREAD_BITS_POSITION = 25;
unsigned int MC_ADDR_BITS = 12;
unsigned int BANK_BITS = 13;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////      	Following schemes are developed under the following assumptions
//////// 	1) Page Size = Row Size
////////	2) Coulumn size cannot be larger than cache block size
////////	3) Tag bilastFinishTimets t should be less than k+l+r
////////	4) Cache block size * blocks per row = cols per row * column size = row * size
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 

//#define CLUBBED 1 // Uncomment this for dhruvs classy clubbing optimizations
#define STALL_TIME 50	

uint NO_OF_THREADS=2;
uint NO_OF_CHANNELS=1;		//  (int)log2() = k bits. 
uint NO_OF_RANKS=32;       		//  (int)log2() = l bits.
uint NO_OF_BANKS=8;         		//  (int)log2() = b bits. 
uint NO_OF_BUFFERS = NO_OF_BANKS;
uint NO_OF_ROWS = 8192; 		//4096 //  (int)log2() = r bits. 
uint NO_OF_COLUMNS = 128;         	//  (int)log2() = c bits. 
uint COLUMN_SIZE = 64;        		//  (int)log2() = v bits.  Column Size = 2bytes
uint BLOCKS_PER_ROW = 128;          	//  (int)log2() = n bits.  Cache line Per Row
uint CACHE_BLOCK_SIZE = 64;         	//  (int)log2() = z bits.  L2 Cache Block Size
uint ROW_SIZE = NO_OF_COLUMNS*COLUMN_SIZE; //(Also equal to BLOCKS_PER_ROW*CACHE_BLOCK_SIZE)
uint DRAM_SIZE = NO_OF_CHANNELS*NO_OF_RANKS*NO_OF_BANKS*NO_OF_ROWS*ROW_SIZE;
uint TAG_BITS=8;              	// t bits

uint USE_MSHR = 1;
uint MSHR_SIZE = 8;

uint GLOBAL_XOR = 0;

//#define CORE_UNCORE_RATIO 1
uint MAX_BUFFER_SIZE = 8;
uint MAX_CMD_BUFFER_SIZE = 16;
uint RESPONSE_BUFFER_SIZE = 56*8; 

uint BATCH_FORM_TIME = 2000;
//#define MAX_BATCH_TIME 2000;
uint MAX_BATCH_SIZE = 5;
uint MAX_READ_OV_WRITE = 8;

uint NETWORK_ADDRESS_BITS = 48;
uint NETWORK_THREADID_BITS = 6;
uint NETWORK_COMMAND_BITS = 3;

const uint READ_SIZE = CACHE_BLOCK_SIZE;
const uint WRITE_SIZE = CACHE_BLOCK_SIZE;
const uint PREFETCH_SIZE = CACHE_BLOCK_SIZE;
const uint WRITEBACK_SIZE = CACHE_BLOCK_SIZE;

/* DRAM Timing parameters. Init for these variables is done in mc_constants.cc
 * depending on clock speeds and dram sizes. */
uint DDR_BUS_WIDTH;
uint BUS_SPEED;
uint CORE_SPEED;
uint MEM_SPEED;
uint MEM_CYCLE;
uint BUS_CYCLE;
uint CYCLE;
uint tREFI;
uint tRFC;
uint tRC;
uint tRAS;
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


const unsigned long long int REFRESH_PERIOD = CORE_SPEED*64000;			// 64ms
const unsigned long long int REFRESH_INC = (ullint)floor(REFRESH_PERIOD/(8192)) - BUS_CYCLE;	// -1 BUS_CYCLE to be on the safe side

enum DRAM_CONFIG { DDR3_1333_9_9_9, 
    DDR3_1600_10_10_10,
    DDR3_1333_6_6_6,
    DDR2_533_4_4_4,
    DDR2_667_4_4_4
};

DRAM_CONFIG dram_config_string = DDR3_1333_9_9_9;

#endif

