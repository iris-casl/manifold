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
#ifndef _CONSTANTS_H
#define _CONSTANTS_H
#include <math.h>
#include <stdint.h>

//#define DEBUG
//#define DEEP_DEBUG

typedef unsigned long long int Time;
typedef unsigned long long int Addr_t;
typedef unsigned int uint;
typedef unsigned int UInt;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////      	Following schemes are developed under the following assumptions
//////// 	1) Page Size = Row Size
////////	2) Coulumn size cannot be larger than cache block size
////////	3) Tag bilastFinishTimets t should be less than k+l+r
////////	4) Cache block size * blocks per row = cols per row * column size = row * size
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
#ifndef USE_ZESTO
enum cache_command { CACHE_NOP, CACHE_READ, CACHE_WRITE, CACHE_WRITEBACK, CACHE_PREFETCH, REFRESH, INVALIDATE, FWD_DIRTY, READ_RESPONSE, WRITE_RESPONSE, ACK };
#else
#include "../zesto/zesto-cache.h"
#endif
   enum DRAM_PAGE_POLICY { OPEN_PAGE_POLICY, CLOSE_PAGE_POLICY};
   enum MC_SCHEDULLING_ALGO { PAR_BS, FR_FCFS, FC_FS, NFQ};
   enum ADDR_MAP_SCHEME { PAGE_INTERLEAVING, PERMUTATION, CACHELINE_INTERLEAVING, SWAPPING, GENERIC, NO_SCHEME, LOCAL_ADDR_MAP};
   enum DRAM_CONFIG { DDR3_1333_9, 
    DDR3_1600_10,
    DDR3_1333_6,
    DDR2_533_4,
    DDR2_667_4
};
extern DRAM_PAGE_POLICY dram_page_policy; 
extern MC_SCHEDULLING_ALGO mc_scheduling_algorithm;
extern ADDR_MAP_SCHEME addr_map_scheme;

extern uint NO_OF_THREADS; 
extern uint NO_OF_CHANNELS ;		//  (int)log2() = k bits. 
extern uint NO_OF_RANKS ;       		//  (int)log2() = l bits.
extern uint NO_OF_BANKS ;         		//  (int)log2() = b bits. 
#define NO_OF_BUFFERS NO_OF_BANKS
extern uint NO_OF_ROWS; 		//4096 //  (int)log2() = r bits. 
extern uint NO_OF_COLUMNS;           //  (int)log2() = c bits. 
extern uint COLUMN_SIZE ;        	//  (int)log2() = v bits.  Column Size = 2bytes
#define BLOCKS_PER_ROW 128          	//  (int)log2() = n bits.  Cache line Per Row
#define CACHE_BLOCK_SIZE 64         	//  (int)log2() = z bits.  L2 Cache Block Size
#define ROW_SIZE NO_OF_COLUMNS*COLUMN_SIZE //(Also equal to BLOCKS_PER_ROW*CACHE_BLOCK_SIZE)
#define DRAM_SIZE NO_OF_CHANNELS*NO_OF_RANKS*NO_OF_BANKS*NO_OF_ROWS*ROW_SIZE
#define TAG_BITS 8              	// t bits

#define USE_MSHR 1 
extern uint MSHR_SIZE ; 

//#define CORE_UNCORE_RATIO 1
extern uint MAX_BUFFER_SIZE ;
extern uint MAX_CMD_BUFFER_SIZE ;
extern uint RESPONSE_BUFFER_SIZE ; //(NO_OF_CHANNELS * (NO_OF_BANKS*NO_OF_RANKS*MAX_BUFFER_SIZE + MAX_CMD_BUFFER_SIZE)) / 4  // Size = Upper Limit / 2

#define BATCH_FORM_TIME 2000;
#define MAX_BATCH_SIZE 5
#define MAX_READ_OV_WRITE 8

extern uint NETWORK_ADDRESS_BITS ;
extern uint NETWORK_THREADID_BITS ;
extern uint NETWORK_COMMAND_BITS ; 

#define READ_SIZE CACHE_BLOCK_SIZE
#define WRITE_SIZE CACHE_BLOCK_SIZE
#define PREFETCH_SIZE CACHE_BLOCK_SIZE
#define WRITEBACK_SIZE CACHE_BLOCK_SIZE

#define REFRESH_PERIOD CORE_SPEED*64000			// 64ms
#define REFRESH_INC (ullint)floor(REFRESH_PERIOD/(8192)) - BUS_CYCLE	// -1 BUS_CYCLE to be on the safe side
/* 
extern uint NO_OF_THREADS;
extern uint NO_OF_CHANNELS;		//  (int)log2() = k bits. 
extern uint NO_OF_RANKS;       		//  (int)log2() = l bits.
extern uint NO_OF_BANKS;     		//  (int)log2() = b bits. 
extern uint NO_OF_BUFFERS;
extern uint NO_OF_ROWS; 		//4096 //  (int)log2() = r bits. 
extern uint NO_OF_COLUMNS;         	//  (int)log2() = c bits. 
extern uint COLUMN_SIZE;        		//  (int)log2() = v bits.  Column Size = 2bytes
extern uint BLOCKS_PER_ROW;         	//  (int)log2() = n bits.  Cache line Per Row
extern uint CACHE_BLOCK_SIZE;         	//  (int)log2() = z bits.  L2 Cache Block Size
extern uint ROW_SIZE;
extern uint DRAM_SIZE;
#define TAG_BITS 8              	// t bits

#define USE_MSHR 1 
extern uint MSHR_SIZE;

extern uint MAX_BUFFER_SIZE;
extern uint MAX_CMD_BUFFER_SIZE;
extern uint RESPONSE_BUFFER_SIZE;  //(NO_OF_CHANNELS * (NO_OF_BANKS*NO_OF_RANKS*MAX_BUFFER_SIZE + MAX_CMD_BUFFER_SIZE)) / 4  // Size = Upper Limit / 2

#define BATCH_FORM_TIME 2000;
//#define MAX_BATCH_TIME 2000;
#define MAX_BATCH_SIZE 5
#define MAX_READ_OV_WRITE 8

extern uint NETWORK_ADDRESS_BITS;
extern uint NETWORK_THREADID_BITS;
extern uint NETWORK_COMMAND_BITS; 

#define READ_SIZE CACHE_BLOCK_SIZE
#define WRITE_SIZE CACHE_BLOCK_SIZE
#define PREFETCH_SIZE CACHE_BLOCK_SIZE
#define WRITEBACK_SIZE CACHE_BLOCK_SIZE


#define REFRESH_PERIOD CORE_SPEED*64000			// 64ms
#define REFRESH_INC (ullint)floor(REFRESH_PERIOD/(8192)) - BUS_CYCLE	// -1 BUS_CYCLE to be on the safe side
 * */

extern float CORE_SPEED;
extern float CYCLE_2_NS;

extern unsigned int DDR_BUS_WIDTH;
extern float BUS_SPEED;
extern float MEM_SPEED;
extern float MEM_CYCLE;
extern float BUS_CYCLE;
extern float CYCLE_2_NS;
extern float tREFI;
extern float tRFC;
extern float tRC;
extern float tRAS;
extern unsigned int t_CMD;
extern unsigned int t_RCD;
extern unsigned int t_RRD;
extern unsigned int t_RAS;
extern unsigned int t_CAS;
extern unsigned int t_RTRS;
extern unsigned int t_OST;
extern unsigned int t_WR;
extern unsigned int t_WTR;
extern unsigned int t_RP;
extern unsigned int t_CCD;
extern unsigned int t_AL;
extern unsigned int t_CWD;
extern unsigned int t_RC;
extern unsigned int t_RTP;
extern unsigned int t_RFC;

#endif

