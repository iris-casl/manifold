/*****************************************************************************
 *                                McPAT
 *                      SOFTWARE LICENSE AGREEMENT
 *            Copyright 2009 Hewlett-Packard Development Company, L.P.
 *                          All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Any User of the software ("User"), by accessing and using it, agrees to the
 * terms and conditions set forth herein, and hereby grants back to Hewlett-
 * Packard Development Company, L.P. and its affiliated companies ("HP") a
 * non-exclusive, unrestricted, royalty-free right and license to copy,
 * modify, distribute copies, create derivate works and publicly display and
 * use, any changes, modifications, enhancements or extensions made to the
 * software by User, including but not limited to those affording
 * compatibility with other hardware or software, but excluding pre-existing
 * software applications that may incorporate the software.  User further
 * agrees to use its best efforts to inform HP of any such changes,
 * modifications, enhancements or extensions.
 *
 * Correspondence should be provided to HP at:
 *
 * Director of Intellectual Property Licensing
 * Office of Strategy and Technology
 * Hewlett-Packard Company
 * 1501 Page Mill Road
 * Palo Alto, California  94304
 *
 * The software may be further distributed by User (but not offered for
 * sale or transferred for compensation) to third parties, under the
 * condition that such third parties agree to abide by the terms and
 * conditions of this license.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" WITH ANY AND ALL ERRORS AND DEFECTS
 * AND USER ACKNOWLEDGES THAT THE SOFTWARE MAY CONTAIN ERRORS AND DEFECTS.
 * HP DISCLAIMS ALL WARRANTIES WITH REGARD TO THE SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL
 * HP BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ***************************************************************************/

#ifndef XML_PARSE_H_
#define XML_PARSE_H_


//#ifdef WIN32
//#define _CRT_SECURE_NO_DEPRECATE
//#endif

#include <stdio.h>
#include "xmlParser.h"
#include <string.h>
#include <iostream>
using namespace std;

/*
void myfree(char *t); // {free(t);}
ToXMLStringTool tx,tx2;
*/
//all subnodes at the level of system.core(0-n)
//cache_policy is added into cache property arrays;//0 no write or write-though with non-write allocate;1 write-back with write-allocate

typedef struct{
	int prediction_width;
	char prediction_scheme[20];
	int predictor_size;
	int predictor_entries;
	int local_predictor_size[20];
	int local_predictor_entries;
	int global_predictor_entries;
	int global_predictor_bits;
	int chooser_predictor_entries;
	int chooser_predictor_bits;
	double predictor_accesses;
} predictor_systemcore;
typedef struct{
	int number_entries;
	int cache_policy;//0 no write or write-though with non-write allocate;1 write-back with write-allocate
	double total_hits;
	double total_accesses;
	double total_misses;
	double conflicts;
} itlb_systemcore;
typedef struct{
	//params
	double icache_config[20];
	int buffer_sizes[20];
	int cache_policy;//0 no write or write-though with non-write allocate;1 write-back with write-allocate
	//stats
	double total_accesses;
	double read_accesses;
	double read_misses;
	double replacements;
	double read_hits;
	double total_hits;
	double total_misses;
	double miss_buffer_access;
	double fill_buffer_accesses;
	double prefetch_buffer_accesses;
	double prefetch_buffer_writes;
	double prefetch_buffer_reads;
	double prefetch_buffer_hits;
	double conflicts;
} icache_systemcore;
typedef struct{
	//params
	int number_entries;
	int cache_policy;//0 no write or write-though with non-write allocate;1 write-back with write-allocate
	//stats
	double total_accesses;
	double read_accesses;
	double write_accesses;
	double write_hits;
	double read_hits;
	double read_misses;
	double write_misses;
	double total_hits;
	double total_misses;
	double conflicts;
} dtlb_systemcore;
typedef struct{
	//params
	double dcache_config[20];
	int buffer_sizes[20];
	int cache_policy;//0 no write or write-though with non-write allocate;1 write-back with write-allocate
	//stats
	double total_accesses;
	double read_accesses;
	double write_accesses;
	double total_hits;
	double total_misses;
	double read_hits;
	double write_hits;
	double read_misses;
	double write_misses;
	double replacements;
	double write_backs;
	double miss_buffer_access;
	double fill_buffer_accesses;
	double prefetch_buffer_accesses;
	double prefetch_buffer_writes;
	double prefetch_buffer_reads;
	double prefetch_buffer_hits;
	double wbb_writes;
	double wbb_reads;
	double conflicts;
} dcache_systemcore;
typedef struct{
	//params
	int BTB_config[20];
	//stats
	double total_accesses;
	double read_accesses;
	double write_accesses;
	double total_hits;
	double total_misses;
	double read_hits;
	double write_hits;
	double read_misses;
	double write_misses;
	double replacements;
} BTB_systemcore;
typedef struct{
	//all params at the level of system.core(0-n)
	int clock_rate;
	bool opt_local;
	bool x86;
	int machine_bits;
	int virtual_address_width;
	int physical_address_width;
	int opcode_width;
	int micro_opcode_width;
	int instruction_length;
	int machine_type;
	int internal_datapath_width;
	int number_hardware_threads;
	int fetch_width;
	int number_instruction_fetch_ports;
	int decode_width;
	int issue_width;
	int peak_issue_width;
	int commit_width;
	int pipelines_per_core[20];
	int pipeline_depth[20];
	char FPU[20];
	char divider_multiplier[20];
	int ALU_per_core;
	double FPU_per_core;
	int MUL_per_core;
	int instruction_buffer_size;
	int decoded_stream_buffer_size;
	int instruction_window_scheme;
	int instruction_window_size;
	int fp_instruction_window_size;
	int ROB_size;
	int archi_Regs_IRF_size;
	int archi_Regs_FRF_size;
	int phy_Regs_IRF_size;
	int phy_Regs_FRF_size;
	int rename_scheme;
	int register_windows_size;
	char LSU_order[20];
	int store_buffer_size;
	int load_buffer_size;
	int memory_ports;
	char Dcache_dual_pump[20];
	int RAS_size;
	int fp_issue_width;
	int prediction_width;
	int number_of_BTB;
	int number_of_BPT;

	//all stats at the level of system.core(0-n)
	double total_instructions;
	double int_instructions;
	double fp_instructions;
	double branch_instructions;
	double branch_mispredictions;
	double committed_instructions;
	double committed_int_instructions;
	double committed_fp_instructions;
	double load_instructions;
	double store_instructions;
	double total_cycles;
	double idle_cycles;
	double busy_cycles;
	double instruction_buffer_reads;
	double instruction_buffer_write;
	double ROB_reads;
	double ROB_writes;
	double rename_accesses;
	double fp_rename_accesses;
	double rename_reads;
	double rename_writes;
	double fp_rename_reads;
	double fp_rename_writes;
	double inst_window_reads;
	double inst_window_writes;
	double inst_window_wakeup_accesses;
	double inst_window_selections;
	double fp_inst_window_reads;
	double fp_inst_window_writes;
	double fp_inst_window_wakeup_accesses;
	double fp_inst_window_selections;
	double archi_int_regfile_reads;
	double archi_float_regfile_reads;
	double phy_int_regfile_reads;
	double phy_float_regfile_reads;
	double phy_int_regfile_writes;
	double phy_float_regfile_writes;
	double archi_int_regfile_writes;
	double archi_float_regfile_writes;
	double int_regfile_reads;
	double float_regfile_reads;
	double int_regfile_writes;
	double float_regfile_writes;
	double windowed_reg_accesses;
	double windowed_reg_transports;
	double function_calls;
	double context_switches;
	double ialu_accesses;
	double fpu_accesses;
	double mul_accesses;
	double cdb_alu_accesses;
	double cdb_mul_accesses;
	double cdb_fpu_accesses;
	double load_buffer_reads;
	double load_buffer_writes;
	double load_buffer_cams;
	double store_buffer_reads;
	double store_buffer_writes;
	double store_buffer_cams;
	double store_buffer_forwards;
	double main_memory_access;
	double main_memory_read;
	double main_memory_write;
	double pipeline_duty_cycle;

	double IFU_duty_cycle ;
	double LSU_duty_cycle ;
	double MemManU_I_duty_cycle;
	double MemManU_D_duty_cycle ;
	double ALU_duty_cycle ;
	double MUL_duty_cycle ;
	double FPU_duty_cycle ;
	double ALU_cdb_duty_cycle ;
	double MUL_cdb_duty_cycle ;
	double FPU_cdb_duty_cycle ;

	//all subnodes at the level of system.core(0-n)
	predictor_systemcore predictor;
	itlb_systemcore itlb;
	icache_systemcore icache;
	dtlb_systemcore dtlb;
	dcache_systemcore dcache;
	BTB_systemcore BTB;

} system_core;
typedef struct{
	//params
	int Directory_type;
	double Dir_config[20];
	int buffer_sizes[20];
	int clockrate;
	int ports[20];
	int device_type;
	int cache_policy;//0 no write or write-though with non-write allocate;1 write-back with write-allocate
	char threeD_stack[20];
	//stats
	double total_accesses;
	double read_accesses;
	double write_accesses;
	double read_misses;
	double write_misses;
	double conflicts;
	double duty_cycle;
} system_L1Directory;
typedef struct{
	//params
	int Directory_type;
	double Dir_config[20];
	int buffer_sizes[20];
	int clockrate;
	int ports[20];
	int device_type;
	int cache_policy;//0 no write or write-though with non-write allocate;1 write-back with write-allocate
	char threeD_stack[20];
	//stats
	double total_accesses;
	double read_accesses;
	double write_accesses;
	double read_misses;
	double write_misses;
	double conflicts;
	double duty_cycle;
} system_L2Directory;
typedef struct{
	//params
	double L2_config[20];
	int clockrate;
	int ports[20];
	int device_type;
	int cache_policy;//0 no write or write-though with non-write allocate;1 write-back with write-allocate
	char threeD_stack[20];
	int buffer_sizes[20];
	//stats
	double total_accesses;
	double read_accesses;
	double write_accesses;
	double total_hits;
	double total_misses;
	double read_hits;
	double write_hits;
	double read_misses;
	double write_misses;
	double replacements;
	double write_backs;
	double miss_buffer_accesses;
	double fill_buffer_accesses;
	double prefetch_buffer_accesses;
	double prefetch_buffer_writes;
	double prefetch_buffer_reads;
	double prefetch_buffer_hits;
	double wbb_writes;
	double wbb_reads;
	double conflicts;
	double duty_cycle;
} system_L2;
typedef struct{
	//params
	double L3_config[20];
	int clockrate;
	int ports[20];
	int device_type;
	int cache_policy;//0 no write or write-though with non-write allocate;1 write-back with write-allocate
	char threeD_stack[20];
	int buffer_sizes[20];
	//stats
	double total_accesses;
	double read_accesses;
	double write_accesses;
	double total_hits;
	double total_misses;
	double read_hits;
	double write_hits;
	double read_misses;
	double write_misses;
	double replacements;
	double write_backs;
	double miss_buffer_accesses;
	double fill_buffer_accesses;
	double prefetch_buffer_accesses;
	double prefetch_buffer_writes;
	double prefetch_buffer_reads;
	double prefetch_buffer_hits;
	double wbb_writes;
	double wbb_reads;
	double conflicts;
	double duty_cycle;
} system_L3;
typedef struct{
	//params
	int number_of_inputs_of_crossbars;
	int number_of_outputs_of_crossbars;
	int flit_bits;
	int input_buffer_entries_per_port;
	int ports_of_input_buffer[20];
	//stats
	double crossbar_accesses;
} xbar0_systemNoC;
typedef struct{
	//params
	int clockrate;
	bool type;
	bool has_global_link;
	char topology[20];
	int horizontal_nodes;
	int vertical_nodes;
	int link_throughput;
	int link_latency;
	int input_ports;
	int output_ports;
	int virtual_channel_per_port;
	int flit_bits;
	int input_buffer_entries_per_vc;
	int ports_of_input_buffer[20];
	int dual_pump;
	int number_of_crossbars;
	char crossbar_type[20];
	char crosspoint_type[20];
	xbar0_systemNoC xbar0;
	int arbiter_type;
	double chip_coverage;
	//stats
	double total_accesses;
	double duty_cycle;
	double route_over_perc;
} system_NoC;
typedef struct{
	//params
	int mem_tech_node;
	int device_clock;
	int peak_transfer_rate;
	int internal_prefetch_of_DRAM_chip;
	int capacity_per_channel;
	int number_ranks;
	int num_banks_of_DRAM_chip;
	int Block_width_of_DRAM_chip;
	int output_width_of_DRAM_chip;
	int page_size_of_DRAM_chip;
	int burstlength_of_DRAM_chip;
	//stats
	double memory_accesses;
	double memory_reads;
	double memory_writes;
} system_mem;
typedef struct{
	//params
    int mc_clock;
	int peak_transfer_rate;
    int llc_line_length;
	int number_mcs;
	int memory_channels_per_mc;
	int number_ranks;
	int req_window_size_per_channel;
	int IO_buffer_size_per_channel;
	int databus_width;
	int addressbus_width;
	//stats
	double memory_accesses;
	double memory_reads;
	double memory_writes;
} system_mc;

typedef struct{
	//All number_of_* at the level of 'system' Ying 03/21/2009
	int number_of_cores;
	int number_of_L1Directories;
	int number_of_L2Directories;
	int number_of_L2s;
	bool Private_L2;
	int number_of_L3s;
	int number_of_NoCs;
	int number_of_dir_levels;
    int domain_size;
    int first_level_dir;
	// All params at the level of 'system'
	int homogeneous_cores;
	int homogeneous_L1Directories;
	int homogeneous_L2Directories;
	double core_tech_node;
	int target_core_clockrate;
	int target_chip_area;
	int temperature;
	int number_cache_levels;
	int L1_property;
	int L2_property;
	int homogeneous_L2s;
	int L3_property;
	int homogeneous_L3s;
	int homogeneous_NoCs;
	int homogeneous_ccs;
	int Max_area_deviation;
	int Max_power_deviation;
	int device_type;
	bool longer_channel_device;
	int opt_dynamic_power;
	int opt_lakage_power;
	int opt_clockrate;
	int opt_area;
	int interconnect_projection_type;
	int machine_bits;
	int virtual_address_width;
	int physical_address_width;
	int virtual_memory_page_size;
    double total_cycles;
	//system.core(0-n):3rd level
	system_core core[64];
	system_L1Directory L1Directory[64];
	system_L2Directory L2Directory[64];
	system_L2 L2[64];
	system_L3 L3[64];
    system_NoC NoC[64];
    system_mem mem;
	system_mc mc;
} root_system;

class ParseXML
{
public:
	void parse(char* filepath);
    void initialize();
public:
	root_system sys;
};


#endif /* XML_PARSE_H_ */




