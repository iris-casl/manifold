#ifndef ZESTO_UNCORE_INCLUDED
#define ZESTO_UNCORE_INCLUDED

/* zesto-uncore.h - Zesto uncore wrapper class
 * 
 * Copyright © 2009 by Gabriel H. Loh and the Georgia Tech Research Corporation
 * Atlanta, GA  30332-0415
 * All Rights Reserved.
 * 
 * THIS IS A LEGAL DOCUMENT BY DOWNLOADING ZESTO, YOU ARE AGREEING TO THESE
 * TERMS AND CONDITIONS.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * NOTE: Portions of this release are directly derived from the SimpleScalar
 * Toolset (property of SimpleScalar LLC), and as such, those portions are
 * bound by the corresponding legal terms and conditions.  All source files
 * derived directly or in part from the SimpleScalar Toolset bear the original
 * user agreement.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the Georgia Tech Research Corporation nor the names of
 * its contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * 4. Zesto is distributed freely for commercial and non-commercial use.  Note,
 * however, that the portions derived from the SimpleScalar Toolset are bound
 * by the terms and agreements set forth by SimpleScalar, LLC.  In particular:
 * 
 *   "Nonprofit and noncommercial use is encouraged. SimpleScalar may be
 *   downloaded, compiled, executed, copied, and modified solely for nonprofit,
 *   educational, noncommercial research, and noncommercial scholarship
 *   purposes provided that this notice in its entirety accompanies all copies.
 *   Copies of the modified software can be delivered to persons who use it
 *   solely for nonprofit, educational, noncommercial research, and
 *   noncommercial scholarship purposes provided that this notice in its
 *   entirety accompanies all copies."
 * 
 * User is responsible for reading and adhering to the terms set forth by
 * SimpleScalar, LLC where appropriate.
 * 
 * 5. No nonprofit user may place any restrictions on the use of this software,
 * including as modified by the user, by any other authorized user.
 * 
 * 6. Noncommercial and nonprofit users may distribute copies of Zesto in
 * compiled or executable form as set forth in Section 2, provided that either:
 * (A) it is accompanied by the corresponding machine-readable source code, or
 * (B) it is accompanied by a written offer, with no time limit, to give anyone
 * a machine-readable copy of the corresponding source code in return for
 * reimbursement of the cost of distribution. This written offer must permit
 * verbatim duplication by anyone, or (C) it is distributed by someone who
 * received only the executable form, and is accompanied by a copy of the
 * written offer of source code.
 * 
 * 7. Zesto was developed by Gabriel H. Loh, Ph.D.  US Mail: 266 Ferst Drive,
 * Georgia Institute of Technology, Atlanta, GA 30332-0765
 */

#include "zesto-cache.h"

#include        "../simIris/components/impl/genericInterfaceVcs.h"
#include        "../simIris/components/impl/genericEvents.h"
#include        "../simIris/components/interfaces/processor.h"
#include        "../simIris/data_types/impl/highLevelPacket.h"
#include	"../memctrl/request.h"
#include        "../util/genericData.h"
#include	"../util/mc_constants.h"
#include        <math.h>
#include	<stdint.h>
#include        <fstream>
#include        <deque>

/* container class for the miscellaneous non-core objects, not
   including the L2, but up to and including the front-side bus */
class uncore_t : public Processor
{
  protected:
  unsigned int last_vc;

  /*Converts a request into a bitstream*/
  void convertToBitStream(Request* req, HighLevelPacket* hlp);
  /*Converts a bitstream into a request*/
  void convertFromBitStream(Request* req, HighLevelPacket *hlp);
  /*packet recieved from network sent to LLC*/
  void handle_new_packet_event(IrisEvent* e);
  /*Ready event sent by network interface*/
  void handle_ready_event(IrisEvent* e);
  /*Event handler for the uncore*/
  void process_event(IrisEvent* e);

  public:

  uint32_t virtual_channels;	//maximum number of packets that can be sent in parallel from the uncore
  uint32_t tile_count;		//The number of tiles in the CMP
  unsigned int num_cores;

  double cpu_speed; /* CPU speed in MHz */
  int cpu_ratio; /*ratio between CPU speed and memory bus speed*/

  /*Pointer to the cores that are part of this uncore tile We set maximum cores per tile as 4 though this can be increased*/
  struct core_t *core[4];

  /*Id of the uncore generally it is 1 to number of uncores*/
  /*NOTE: This is not the same as the id of the uncore in the mesh as the mesh has */
  unsigned int id;
  
  /* shared last-level cache */
  struct bus_t * LLC_bus;
  struct cache_t * LLC;
  tick_t LLC_cycle_mask;

  /* coherence request buffers */
  int creq_size;
  int creq_num;
  int creq_head;
  int creq_tail;
  struct coherence_req *creq;

  /*Uncore statistics*/
  struct uncore_stat_t {
  counter_t packets_out;
  counter_t packets_in;
  counter_t min_pkt_latency;
  counter_t last_packet_out_cycle;
  counter_t fwd_path_delay;
  counter_t round_trip_lat;
  counter_t TotalBLP;
  counter_t round_trip_network_latency;
  counter_t round_trip_hop_count;
  counter_t round_trip_memory_latency;
  counter_t waiting_in_ni;
  }stat;

  /*Functions called by the LLC to send a request */
  bool enqueuable(const md_paddr_t addr,const enum cache_command cmd,tick_t when_enqueued);
  void enqueue(struct cache_t * const prev_cp,const enum cache_command cmd,const md_paddr_t addr,const int linesize,const seq_t action_id,
			const int MSHR_bank,const int MSHR_index,void * const op,void (*const cb)(void *),seq_t (*const get_action_id)(void *));

  vector<bool> ready;
  bool sending;
  bool compare();
  uint sending_vc;
  ullint lastSentTime;
  vector <unsigned int> mc_node_ip;

  void uncore_reg_stats(struct uncore_t * uncore, struct stat_sdb_t * const sdb);

  FILE *app_mem_trace;  /*Memory trace of the application at the backside of cache*/
  bool log_trace;	/*Start logging trace flag set by the core*/
  
  void setup(uint no_nodes, uint vcs, uint max_sim_time);
  string toString () const;
  string print_stats() const;
  void set_output_path( string outpath_name );  
  /* constructor */
  uncore_t(unsigned int arg_id, const double arg_cpu_speed, unsigned int arg_virtual_channels,unsigned int arg_tile_count );
  virtual ~uncore_t();

};

void uncore_reg_options(struct opt_odb_t * const odb);
unsigned long long int GlobalAddrMap(unsigned long long int addr, unsigned int  threadId);
unsigned long long int GlobalAddrRip(unsigned long long int addr, unsigned int  threadId);
short int get_mcNo(unsigned long long int addr);

#endif /* ZESTO_UNCORE_INCLUDED */
