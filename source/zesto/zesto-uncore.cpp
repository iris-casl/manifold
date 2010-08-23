/* zesto-uncore.cpp - Zesto uncore wrapper class
 * 
 *	Modified by: Dhruv Choudhary
 *	Date: 27th July 2010
 *	Organizations: Georgia Institute of Technology
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

#include <limits.h>
#include "thread.h"
#include "stats.h"
#include "options.h"
#include "zesto-core.h"
#include "zesto-opts.h"
#include "zesto-cache.h"
#include "zesto-prefetch.h"
#include "zesto-uncore.h"
#include "sim.h"

#define USE_PIN_TRACES
#define GEN_APP_TRACES
/* The uncore class is just a wrapper around the last-level cache,
   front-side-bus and memory controller, plus relevant parameters.
 */

static char * LLC_opt_str = (char *)"LLC:4096:16:64:16:64:9:L:W:B:8:1:8:C";
static int LLC_bus_ratio = 1;
static int LLC_access_rate = 1;
static char * LLC_MSHR_cmd = (char *)"RPWB";

/* LLC prefetcher options */
static char * LLC_PF_opt_str[MAX_PREFETCHERS];
static int LLC_num_PF = 0;
static int LLC_PFFsize = 8;
static int LLC_PFthresh = 2;
static int LLC_PFmax = 1;
static int LLC_PF_buffer_size = 0;
static int LLC_PF_filter_size = 0;
static int LLC_PF_filter_reset = 0;
static int LLC_WMinterval = 10000;
static bool LLC_PF_on_miss = false;
static double LLC_low_watermark = 0.1;
static double LLC_high_watermark = 0.3;

/* constructor */
uncore_t::uncore_t(
    unsigned int arg_id,
    const double arg_cpu_speed,
    const unsigned int arg_vcs,
    unsigned int arg_tile_count)
: id(arg_id),
  cpu_speed(arg_cpu_speed),
  virtual_channels(arg_vcs),
  tile_count(arg_tile_count)
{
  /* temp variables for option-string parsing */
  char name[256];
  int sets, assoc, linesize, latency, banks, bank_width, MSHR_banks, MSHR_entries, WBB_entries;
  char rp, ap, wp, wc;

  /*The number of cores per node is a variabled declared in the simMc2mesh.cc file*/
  num_cores= cores_per_node;

  /*Initialising the uncore stats*/    
  stat.packets_out = 0;
  stat.packets_in = 0;
  stat.min_pkt_latency = 999999999;
  stat.last_packet_out_cycle = 0;
  stat.fwd_path_delay = 0;
  stat.round_trip_lat = 0;
  stat.round_trip_hop_count = 0;
  stat.round_trip_network_latency = 0;
  stat.round_trip_memory_latency = 0;
  stat.waiting_in_ni = 0;

  lastSentTime = 0;
  /*Setting the ready events for all VC's to default state*/  
  ready.resize( virtual_channels );
//  ready.insert( ready.begin(), ready.size(), false );
  for(unsigned int i = 0; i < ready.size(); i++)
      ready[i] = true;

  /* Shared LLC */
  if(sscanf(LLC_opt_str,"%[^:]:%d:%d:%d:%d:%d:%d:%c:%c:%c:%d:%d:%d:%c",
      name,&sets,&assoc,&linesize,&banks,&bank_width,&latency,&rp,&ap,&wp, &MSHR_entries, &MSHR_banks, &WBB_entries, &wc) != 14)
    fatal("invalid LLC options: <name:sets:assoc:linesize:banks:bank-width:latency:repl-policy:alloc-policy:write-policy:num-MSHR:MSHR-banks:WB-buffers:write-combining>\n\t(%s)",LLC_opt_str);

  LLC = cache_create_llc(NULL,name,CACHE_READWRITE,sets,assoc,linesize,rp,ap,wp,wc,banks,bank_width,latency,WBB_entries,MSHR_entries,MSHR_banks,0,0,NULL,NULL);
  if(!LLC)
	fatal("failed to calloc LLC");  


  LLC->uncore= this;
  if(!LLC_MSHR_cmd || !strcasecmp(LLC_MSHR_cmd,"fcfs"))
    LLC->MSHR_cmd_order = NULL;
  else
  {
    if(strlen(LLC_MSHR_cmd) != 4)
      fatal("-LLC:mshr_cmd must either be \"fcfs\" or contain all four of [RWBP]");
    bool R_seen = false;
    bool W_seen = false;
    bool B_seen = false;
    bool P_seen = false;

    LLC->MSHR_cmd_order = (enum cache_command*)calloc(4,sizeof(enum cache_command));
    if(!LLC->MSHR_cmd_order)
      fatal("failed to calloc MSHR_cmd_order array for LLC");

    for(int c=0;c<4;c++)
    {
      switch(mytoupper(LLC_MSHR_cmd[c]))
      {
        case 'R': LLC->MSHR_cmd_order[c] = CACHE_READ; R_seen = true; break;
        case 'W': LLC->MSHR_cmd_order[c] = CACHE_WRITE; W_seen = true; break;
        case 'B': LLC->MSHR_cmd_order[c] = CACHE_WRITEBACK; B_seen = true; break;
        case 'P': LLC->MSHR_cmd_order[c] = CACHE_PREFETCH; P_seen = true; break;
        default: fatal("unknown cache operation '%c' for -LLC:mshr_cmd; must be one of [RWBP]");
      }
    }
    if(!R_seen || !W_seen || !B_seen || !P_seen)
      fatal("-LLC:mshr_cmd must contain *each* of [RWBP]");
  }

  if(LLC_access_rate & (LLC_access_rate-1))
    fatal("-LLC:rate must be power of two");
  LLC_cycle_mask = LLC_access_rate-1;

  LLC->PFF_size = LLC_PFFsize;
  LLC->PFF = (cache_t::PFF_t*) calloc(LLC_PFFsize,sizeof(*LLC->PFF));
  if(!LLC->PFF)
    fatal("failed to calloc %s's prefetch FIFO",LLC->name);
  prefetch_buffer_create(LLC,LLC_PF_buffer_size);
  prefetch_filter_create(LLC,LLC_PF_filter_size,LLC_PF_filter_reset);
  LLC->prefetch_threshold = LLC_PFthresh;
  LLC->prefetch_max = LLC_PFmax;
  LLC->PF_low_watermark = LLC_low_watermark;
  LLC->PF_high_watermark = LLC_high_watermark;
  LLC->PF_sample_interval = LLC_WMinterval;

  LLC->prefetcher = (struct prefetch_t**) calloc(LLC_num_PF?LLC_num_PF:1/* avoid 0-size alloc */,sizeof(*LLC->prefetcher));
  LLC->num_prefetchers = LLC_num_PF;
  if(!LLC->prefetcher)
    fatal("couldn't calloc %s's prefetcher array",LLC->name);
  for(int i=0;i<LLC_num_PF;i++)
    LLC->prefetcher[i] = prefetch_create(LLC_PF_opt_str[i],LLC);
  if(LLC->prefetcher[0] == NULL)
    LLC->num_prefetchers = LLC_num_PF = 0;

  LLC_bus = bus_create("LLC_bus",LLC->linesize,LLC_bus_ratio);

  //creq_size=1000;
  //creq_head=0;
  //creq_tail=0;
  //creq_num=0;
  //creq = (struct coherence_req*) calloc(creq_size,sizeof(*creq));

}

/* destructor */
uncore_t::~uncore_t()
{
}


void uncore_t::setup(uint no_nodes, uint vcs, uint max_sim_time)
{
    address = myId();
    node_ip = address/3;

#ifdef GEN_APP_TRACES
  char ch[50];
  sprintf (ch, "app-trace%d.out",node_ip);
  printf ("[%s] is a application trace file\n",ch);
  app_mem_trace = fopen(ch,"w");
  if(!app_mem_trace)
     fatal("could not open app trace file");
  log_trace=false;
#endif
/*
	for(unsigned int i = 0; i < ready.size(); i++)
		ready[i] = true;
*/
}


string uncore_t::print_stats() const
{
    stringstream str;
    return str.str();
}

void uncore_t::set_output_path( string outpath_name)
{

}

void  uncore_reg_options(struct opt_odb_t * const odb)
{
  opt_reg_string(odb, (char *)"-LLC",(char *)"last-level cache configuration string [DS]",
      &LLC_opt_str, /*default*/ (char *)"LLC:2048:16:64:16:64:12:L:W:B:8:1:8:C", /*print*/true,/*format*/NULL);
  opt_reg_string(odb, (char *)"-LLC:mshr_cmd",(char *)"last-level cache MSHR scheduling policy [DS]",
      &LLC_MSHR_cmd, /*default*/ LLC_MSHR_cmd, /*print*/true,/*format*/NULL);
  opt_reg_int(odb, (char *)"-LLC:bus",(char *)"CPU clock cycles per LLC-bus cycle [DS]",
      &LLC_bus_ratio, /*default*/ 1, /*print*/true,/*format*/NULL);
  opt_reg_int(odb, (char *)"-LLC:rate",(char *)"access LLC once per this many cpu clock cycles [DS]",
      &LLC_access_rate, /*default*/ 1, /*print*/true,/*format*/NULL);

  /* LLC prefetch control options */
  opt_reg_string_list(odb, (char *)"-LLC:pf", (char *)"last-level cache prefetcher configuration string(s) [DS]",
      LLC_PF_opt_str, MAX_PREFETCHERS, &LLC_num_PF, LLC_PF_opt_str, /* print */true, /* format */NULL, /* !accrue */false);
  opt_reg_int(odb, (char *)"-LLC:pf:fifosize",(char *)"LLC prefetch FIFO size [DS]",
      &LLC_PFFsize, /*default*/ 16, /*print*/true,/*format*/NULL);
  opt_reg_int(odb, (char *)"-LLC:pf:buffer",(char *)"LLC prefetch buffer size [DS]",
      &LLC_PF_buffer_size, /*default*/ 0, /*print*/true,/*format*/NULL);
  opt_reg_int(odb, (char *)"-LLC:pf:filter",(char *)"LLC prefetch filter size [DS]",
      &LLC_PF_filter_size, /*default*/ 0, /*print*/true,/*format*/NULL);
  opt_reg_int(odb, (char *)"-LLC:pf:filterreset",(char *)"LLC prefetch filter reset interval (cycles) [DS]",
      &LLC_PF_filter_reset, /*default*/ 65536, /*print*/true,/*format*/NULL);
  opt_reg_int(odb, (char *)"-LLC:pf:thresh",(char *)"LLC prefetch threshold (only prefetch if MSHR occupancy < thresh) [DS]",
      &LLC_PFthresh, /*default*/ 4, /*print*/true,/*format*/NULL);
  opt_reg_int(odb, (char *)"-LLC:pf:max",(char *)"maximum LLC prefetch requests in MSHRs at a time [DS]",
      &LLC_PFmax, /*default*/ 2, /*print*/true,/*format*/NULL);
  opt_reg_double(odb, (char *)"-LLC:pf:lowWM",(char *)"LLC low watermark for prefetch control [DS]",
      &LLC_low_watermark, /*default*/ 0.1, /*print*/true,/*format*/NULL);
  opt_reg_double(odb, (char *)"-LLC:pf:highWM",(char *)"LLC high watermark for prefetch control [DS]",
      &LLC_high_watermark, /*default*/ 0.5, /*print*/true,/*format*/NULL);
  opt_reg_int(odb, (char *)"-LLC:pf:WMinterval",(char *)"LLC sampling interval (in cycles) for prefetch control (0 = no PF controller) [DS]",
      &LLC_WMinterval, /*default*/ 100, /*print*/true,/*format*/NULL);
  opt_reg_flag(odb, (char *)"-LLC:pf:miss",(char *)"generate LLC prefetches only from miss traffic [DS]",
      &LLC_PF_on_miss, /*default*/ false, /*print*/true,/*format*/NULL);
  opt_reg_double(odb, (char *)"-cpu:speed", (char *)"CPU speed in MHz [DS]",
        &cpu_speed, /*default*/4000.0,/*print*/true,/*format*/NULL); 
}

/* register all of the stats */
void uncore_t::uncore_reg_stats(struct uncore_t * uncore,struct stat_sdb_t * const sdb)
{
  char buf[1024];
  char buf2[256];
  char buf3[256];
  //char buf2[1024];

  stat_reg_note(sdb,"\n#### LAST-LEVEL CACHE STATS ####");
  LLC_reg_stats(sdb, uncore->LLC);
  bus_reg_stats(sdb, NULL, uncore->LLC_bus);


  stat_reg_note(sdb,"\n#### UNCORE STATS ####");

  sprintf(buf,"u%d.packets_sent_out",uncore->node_ip);
  stat_reg_counter(sdb, true, buf, "total number of packets sent out", &uncore->stat.packets_out, uncore->stat.packets_out, NULL);
  sprintf(buf,"u%d.packets_in",uncore->node_ip);
  stat_reg_counter(sdb, true, buf, "total number of packets received", &uncore->stat.packets_in, uncore->stat.packets_in, NULL);
  sprintf(buf,"u%d.min_pkt_latency",uncore->node_ip);
  stat_reg_counter(sdb, true, buf, "minimum packet latency", &uncore->stat.min_pkt_latency, uncore->stat.min_pkt_latency, NULL);
  sprintf(buf,"u%d.last_packet_out_cycle",uncore->node_ip);
  stat_reg_counter(sdb, true, buf, "last packet out cycle", &uncore->stat.last_packet_out_cycle, uncore->stat.last_packet_out_cycle, NULL);

  sprintf(buf,"u%d.round_trip_lat",uncore->node_ip);
  stat_reg_counter(sdb, true, buf, "round trip latency", &uncore->stat.round_trip_lat, uncore->stat.round_trip_lat, NULL);
  sprintf(buf,"u%d.avg_round_trip_latency",uncore->node_ip);
  sprintf(buf2,"average round trip latency");
  sprintf(buf3,"u%d.round_trip_lat/u%d.packets_in",uncore->node_ip,uncore->node_ip);
  stat_reg_formula(sdb, true, buf, buf2, buf3, "%12.4f");

  sprintf(buf,"u%d.fwd_path_delay",uncore->node_ip);
  stat_reg_counter(sdb, true, buf, "forward path delay", &uncore->stat.fwd_path_delay, uncore->stat.fwd_path_delay, NULL);
  sprintf(buf,"u%d.avg_fwd_path_delay",uncore->node_ip);
  sprintf(buf2,"average fwd path delay");
  sprintf(buf3,"u%d.fwd_path_delay/u%d.packets_sent_out",uncore->node_ip,uncore->node_ip);
  stat_reg_formula(sdb, true, buf, buf2, buf3, "%12.4f");

  sprintf(buf,"u%d.round_trip_network_latency",uncore->node_ip);
  stat_reg_counter(sdb, true, buf, "round trip network latency", &uncore->stat.round_trip_network_latency, uncore->stat.round_trip_network_latency, NULL);
  sprintf(buf,"u%d.avg_round_trip_network_latency",uncore->node_ip);
  sprintf(buf2,"avg round trip network latency");
  sprintf(buf3,"u%d.round_trip_network_latency/u%d.packets_in",uncore->node_ip,uncore->node_ip);
  stat_reg_formula(sdb, true, buf, buf2, buf3, "%12.4f");

  sprintf(buf,"u%d.round_trip_hop_count",uncore->node_ip);
  stat_reg_counter(sdb, true, buf, "round trip hop count", &uncore->stat.round_trip_hop_count, uncore->stat.round_trip_hop_count, NULL);
  sprintf(buf,"u%d.avg_round_trip_hop_count",uncore->node_ip);
  sprintf(buf2,"avg round trip hop count");
  sprintf(buf3,"u%d.round_trip_hop_count/u%d.packets_in",uncore->node_ip,uncore->node_ip);
  stat_reg_formula(sdb, true, buf, buf2, buf3, "%12.4f");

  sprintf(buf,"u%d.round_trip_memory_latency",uncore->node_ip);
  stat_reg_counter(sdb, true, buf, "round trip memory latency", &uncore->stat.round_trip_memory_latency, uncore->stat.round_trip_memory_latency, NULL);
  sprintf(buf,"u%d.avg_round_trip_memory_latency",uncore->node_ip);
  sprintf(buf2,"avg round trip memory latency");
  sprintf(buf3,"u%d.round_trip_memory_latency/u%d.packets_in",uncore->node_ip,uncore->node_ip);
  stat_reg_formula(sdb, true, buf, buf2, buf3, "%12.4f");

  sprintf(buf,"u%d.waiting_in_ni",uncore->node_ip);
  stat_reg_counter(sdb, true, buf, "waiting in NI", &uncore->stat.waiting_in_ni, uncore->stat.waiting_in_ni, NULL);
  sprintf(buf,"u%d.avg_waiting_in_ni",uncore->node_ip);
  sprintf(buf2,"avg waiting in ni");
  sprintf(buf3,"u%d.waiting_in_ni/u%d.packets_in",uncore->node_ip,uncore->node_ip);
  stat_reg_formula(sdb, true, buf, buf2, buf3, "%12.4f");

}



/*informs the uncore that the network is ready to ceive a packet for a given virtual channel*/
void
uncore_t::handle_ready_event(IrisEvent* e)
{

    ready[e->vc] = true;	/*Set the ready event for a given VC*/
    delete e;
    return ;
}


void uncore_t::process_event(IrisEvent* e)
{
    switch(e->type)
    {
        case NEW_PACKET_EVENT:
            handle_new_packet_event(e);
            break;
        case READY_EVENT:
            handle_ready_event(e);
            break;
        default:
            break;
    }
    return;
}


void uncore_t::handle_new_packet_event( IrisEvent* e)
{
    ni_recv = false;
    HighLevelPacket* hlp = static_cast<HighLevelPacket* >( e->event_data.at(0));
    double lat = Simulator::Now() - hlp->sent_time;

    stat.last_packet_out_cycle = Simulator::Now();

    if( stat.min_pkt_latency > lat)
	stat.min_pkt_latency = lat;

    stat.packets_in++;

    stat.round_trip_hop_count += hlp->hop_count;
    stat.round_trip_network_latency += (ullint)hlp->avg_network_latency;
    stat.round_trip_memory_latency += hlp->stat_memory_serviced_time;
    stat.waiting_in_ni += hlp->waiting_in_ni;

    Request * req = new Request();

    convertFromBitStream(req,hlp);

    req->startTime = hlp->req_start_time;


#ifdef USE_PIN_TRACES
   req->address = GlobalAddrRip(req->address,node_ip);
#endif

    //cout << "[" << dec << sim_cycle << "] Packet returned to Uncore " << dec << node_ip << " Address 0x" << hex << req->address << endl;

    stat.round_trip_lat += ((ullint)Simulator::Now() - req->startTime);

    fill_arrived_llc(LLC,req->address);
    delete hlp;

    // send back a ready event
    IrisEvent* event2 = new IrisEvent();
    event2->type = READY_EVENT;
    event2->vc = hlp->virtual_channel;
    Simulator::Schedule( Simulator::Now()+1, &NetworkComponent::process_event, interface_connections[0], event2);

    delete req;
    delete e;
    return ;
} 

bool uncore_t::enqueuable(const md_paddr_t addr,const enum cache_command cmd,tick_t when_enqueued)
{

    sending =false;
    bool found = false;
    sending_vc = -1;
    //uint old_last_vc = last_vc;
    for( uint i=last_vc+1; i<vcs; i++)
    {
        if(ready[i])
        {
            found = true;
            sending_vc = i;
            last_vc = i;
            break;
        }
    }
    if ( !found)
    {
        for ( uint i=0; i<=last_vc; i++)
        {
            if(ready[i])
            {
                found = true;
                sending_vc = i;
                last_vc = i;
                break;
            }
        }
    }


	if(found)
		return true;
	else
		return false;
	
}

void uncore_t::enqueue(struct cache_t * const prev_cp,const enum cache_command cmd,const md_paddr_t addr,const int linesize,const seq_t action_id,
			const int MSHR_bank,const int MSHR_index,void * const op,void (*const cb)(void *),seq_t (*const get_action_id)(void *))
{
	
#ifdef GEN_APP_TRACES
if(log_trace)
	fprintf(app_mem_trace,"0x%llx %lld %d\n",addr,sim_cycle,cmd);
#endif

	Request *req = new Request();
	req->cmdType = cmd;
#ifdef USE_PIN_TRACES
	//cout << "Uncore" << node_ip  << "Changed address from 0x" << hex << addr;
	req->address = GlobalAddrMap(addr,node_ip);
	//cout << " to 0x" << req->address ;
#else
	req->address = addr;
#endif
	req->arrivalTime = sim_cycle;
	req->threadId = node_ip;
	//req->address = GlobalAddrMap(addr,node_ip);	

	req->mcNo = get_mcNo(req->address);
        req->mcNo = mc_node_ip[req->mcNo];
	stat.packets_out++;
        stat.fwd_path_delay += (ullint)(Simulator::Now() - req->arrivalTime);
        HighLevelPacket* hlp = new HighLevelPacket();


	//cout << endl << "[" << dec << sim_cycle <<  "][Uncore" << dec << node_ip << "]" << " Sending packet Address 0x" << hex << addr << " to 0x" << req->address << "  CommandType " << dec<< req->cmdType << endl;
	/*Setting HighLevelPacket values*/
        hlp->virtual_channel = sending_vc;
        hlp->source = address;
	hlp->destination = req->mcNo;
        hlp->addr = req->address;
        hlp->transaction_id = 1000;
	convertToBitStream(req, hlp);
        if(hlp->sent_time < Simulator::Now())
            hlp->sent_time = (ullint)Simulator::Now()+1;
        hlp->req_start_time = sim_cycle;

	/*Sending HLP to the interface*/
        ready[sending_vc] = false;
        IrisEvent* event = new IrisEvent();
        event->type = NEW_PACKET_EVENT;
        event->event_data.push_back(hlp);
        event->vc = sending_vc;
        Simulator::Schedule( hlp->sent_time, &NetworkComponent::process_event, interface_connections[0], event );

	delete req;
}


extern unsigned int THREAD_BITS_POSITION;
md_paddr_t GlobalAddrMap(md_paddr_t addr, unsigned int  threadId)
{
    md_paddr_t newAddr;
    unsigned long long int threadBits = (unsigned long long int)ceil(log2(NO_OF_THREADS));
    unsigned long int threadBitsPos = THREAD_BITS_POSITION;
    md_paddr_t lowerMask = (md_paddr_t)pow(2.0,threadBitsPos*1.0)-1;
    md_paddr_t upperMask = 0xFFFFFFFFFFFFLLU << threadBitsPos;
    md_paddr_t lowerAddr = addr & lowerMask;
    md_paddr_t upperAddr = addr & upperMask;
#ifdef USE_PIN_TRACES

#endif
    md_paddr_t threadAddr = (threadId & ((md_paddr_t)pow(2.0,threadBits*1.0)-1));
    newAddr = lowerAddr | (threadAddr << threadBitsPos)| (upperAddr << threadBits);	
    return newAddr;
}

md_paddr_t GlobalAddrRip(md_paddr_t addr, unsigned int  threadId)
{
    md_paddr_t newAddr;
    unsigned long long int threadBits = (unsigned long long int)ceil(log2(NO_OF_THREADS));
    unsigned long int threadBitsPos = THREAD_BITS_POSITION;
    md_paddr_t lowerMask = (md_paddr_t)pow(2.0,threadBitsPos*1.0)-1;
    md_paddr_t upperMask = 0xFFFFFFFFFFFFLLU << threadBitsPos;
    md_paddr_t lowerAddr = addr & lowerMask;
    md_paddr_t upperAddr = addr >> threadBitsPos;
    md_paddr_t threadAddr = upperAddr & (NO_OF_THREADS-1) ;
    upperAddr = upperAddr >> (unsigned long long int)ceil(log2(NO_OF_THREADS)) ;
    upperAddr = upperAddr << threadBitsPos ;
    
#ifdef USE_PIN_TRACES
    //cout << "Uncore[" << dec<< threadId << "] Original address 0x" << hex << lowerAddr << " Node_ip taken 0x" << dec << upperAddr << endl;
#endif
    if(threadAddr!=threadId)
    {
	cout << "Uncore[" << dec<< threadId << "] Original address 0x" << hex << lowerAddr << " Node_ip taken " << dec << upperAddr << endl;
	fprintf(stdout,"\n[%lld]UpperAddr= 0x%lld Returned addr= 0x%llx",sim_cycle,upperAddr, addr);
	//fprintf(stdout,"\nIntsructions completed %lld",uncores[threadId]->core[0]->stat.oracle_total_insn);
	assert(threadAddr!=threadId); 
    }
    
    newAddr = upperAddr| lowerAddr;	

    return newAddr;
}




short int get_mcNo(md_paddr_t req_addr)
{

    md_paddr_t *addr = &req_addr;
    unsigned int temp = MC_ADDR_BITS;   
    unsigned int temp2 = temp-(int)log2(no_mcs);   
    unsigned int lower_mask = (uint)pow(2.0,temp2*1.0)-1;
#ifdef _64BIT
    md_paddr_t upper_mask = (ullint)((0xFFFFFFFFFFFF)-(pow(2.0,temp*1.0)-1));
#else
    md_paddr_t upper_mask = (ullint)((0xFFFFFFFF)-(pow(2.0,temp*1.0)-1));
#endif
    unsigned int lower_addr = (*addr) & lower_mask;
    md_paddr_t upper_addr = ((*addr) & upper_mask) >> (int)log2(no_mcs);

#ifdef GLOBAL_XOR
    short int tempBits = (int)log2(NO_OF_THREADS) - (int)log2(no_mcs);
    short int tempFactor = ((id >> tempBits) & (no_mcs-1));
    short int mc_addr = (((*addr) >> temp2) & (no_mcs-1)) ^ tempFactor;
#else    
    short int mc_addr = ((*addr) >> temp2) & (no_mcs-1);
#endif

    *addr = upper_addr | lower_addr;
    return mc_addr;
}


void uncore_t::convertToBitStream(Request* req, HighLevelPacket* hlp)
{
    for ( uint i=0 ; i < NETWORK_ADDRESS_BITS ; i++ )
    {
	unsigned long long int bit_addr = (req->address >> i);
	bool bit = (bool)(bit_addr & 0x1);
        hlp->data.push_back(bit);
    }
    for ( uint i=0 ; i < NETWORK_COMMAND_BITS ; i++ )
    {
	bool bit = (bool)((req->cmdType >> i) & 0x1);
        hlp->data.push_back(bit);
    }
    for ( uint i=0 ; i < NETWORK_THREADID_BITS ; i++ )
    {
	bool bit = (bool)((req->threadId >> i) & 0x1);
        hlp->data.push_back(bit);
    }
    if (req->cmdType == CACHE_WRITEBACK)
    {	for ( uint i = hlp->data.size() ; i < (8*CACHE_BLOCK_SIZE - max_phy_link_bits); i++ )
	{
            hlp->data.push_back(true);
        }
        hlp->msg_class = RESPONSE_PKT;
    }
    else
        hlp->msg_class = ONE_FLIT_REQ;

    hlp->data_payload_length = uint(ceil(hlp->data.size() *1.0 / max_phy_link_bits));	
    hlp->data_payload_length = hlp->data_payload_length * max_phy_link_bits;
    hlp->sent_time = req->arrivalTime;
    for ( uint i=hlp->data.size() ; i < hlp->data_payload_length; i++ )
        hlp->data.push_back(false);

}



void uncore_t::convertFromBitStream(Request* req, HighLevelPacket *hlp)
{
    req->address = 0x0;	
    req->mcNo=hlp->source/3;
    //cout << endl;
    for (unsigned int i=0; i < NETWORK_ADDRESS_BITS; i++)
    {
	//cout << dec << hlp->data[i]<<endl;
	unsigned long long int bit = hlp->data[i] ;
	req->address = req->address | bit << i;
	//cout<< hex << req->address <<endl;
    }
    //cout << "Returned address" << hex << req->address << endl;
#ifdef _64BIT
    req->address = req->address & (0xFFFFFFFFFFFF) ;
#else
    req->address = req->address & (0xFFFFFFFF) ;
#endif

    //cout << "\n BitstreamReqAddress " <<  hex << req->address ; 
/*    unsigned int temp = 0;
    for (unsigned int i = 0; i < NETWORK_COMMAND_BITS; i++)
    {
	temp = temp | (hlp->data[i+NETWORK_ADDRESS_BITS] << i);
    }
    req->cmdType = (cache_command)temp;

    req->threadId = 0;
    for (unsigned int i=0; i < NETWORK_THREADID_BITS; i++)
    {
	req->threadId = req->threadId | (hlp->data[i+NETWORK_ADDRESS_BITS+NETWORK_COMMAND_BITS] << i);
    }
*/

    req->data.value = 0;
    if (req->cmdType == CACHE_READ || req->cmdType == CACHE_WRITE || req->cmdType == CACHE_PREFETCH)
    	for ( uint i = NETWORK_ADDRESS_BITS; i < (8*CACHE_BLOCK_SIZE - max_phy_link_bits); i++ )
	{
	    req->data.value = req->data.value | (hlp->data[i+NETWORK_ADDRESS_BITS] << i);
	    req->data.size = CACHE_BLOCK_SIZE;
        }
}

string
uncore_t::toString () const
{
    stringstream str;
    str << "\nUncore: "
        << "\t vcs: " << ready.size()
        << "\t address: " <<address
        << "\t node_ip: " << node_ip
        << "\t int address: " << interface_connections[0]->address
        ;
    return str.str();
}



