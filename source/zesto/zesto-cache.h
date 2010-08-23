#ifndef ZESTO_CACHE_INCLUDED
#define ZESTO_CACHE_INCLUDED

/* zesto-cache.h - Zesto cache structures
 * 
 *	Modified by: Dhruv Choudhary
 *	Date: 27th July 2010
 *	Organizations: Georgia Institute of Technology
 * Copyright � 2009 by Gabriel H. Loh and the Georgia Tech Research Corporation
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

/* used when passing an MSHR index into the cache functions, but for
   whatever reason there's no corresponding MSHR entry */
#define NO_MSHR (-1)

/* This is used for latency prediction when a load completely misses in cache
   and has to go off to main memory */
#define BIG_LATENCY 99999

#define CACHE_STAT(x) {if(!cp->frozen) {x}}

#define GET_BANK(x) (((x)>>cp->bank_shift) & cp->bank_mask)
#define GET_MSHR_BANK(x) (((x)>>cp->bank_shift) & cp->MSHR_mask)

/* CACHE_WRITEBACK is the same as CACHE_WRITE, except that CACHE_WRITEBACK does
   not cause the cache's replacement information/state to get updated. */
enum cache_command { CACHE_NOP, CACHE_READ, CACHE_WRITE, CACHE_WRITEBACK, CACHE_PREFETCH, REFRESH, INVALIDATE, FWD_DIRTY, READ_RESPONSE, WRITE_RESPONSE, ACK };
/*
*  INVALIDATE= invalidate your entries from top to bottom
*  FWD_DIRTY= Forward a line which should be dirty in your cache
*  ACK= Acknowledgement of invalidate
*/
enum repl_policy_t { REPLACE_LRU, REPLACE_MRU, REPLACE_RANDOM, REPLACE_NMRU, REPLACE_PLRU, REPLACE_CLOCK };
enum alloc_policy_t { WRITE_ALLOC, NO_WRITE_ALLOC };
enum write_policy_t { WRITE_THROUGH, WRITE_BACK };
enum read_only_t { CACHE_READWRITE, CACHE_READONLY };
enum coherence_state {MOD, EXC, SHR, INV};

enum PF_state_t { PF_REFRAIN, PF_OK };

struct cache_line_t {
  md_paddr_t tag;
  struct core_t * core; /* originating core */
  int way; /* which physical column/way am I in? */
  qword_t meta; /* additional field for replacment policy meta data */
  bool valid;
  enum coherence_state c_state;
  bool dirty;
  bool victim;
  bool prefetched;
  bool prefetch_used;
  struct cache_line_t * next;
};

struct cache_action_t {
  struct core_t * core; /* originating core */
  void * op;
  seq_t action_id;
  md_addr_t PC;
  md_paddr_t paddr;
  int MSHR_bank;
  int MSHR_index;
  bool miss_cb_invoked;
  enum cache_command cmd;
  struct cache_t * prev_cp;
  /* final call back on service-complete */
  void (*cb)(void * op); /* NULL = invalid/empty entry */
  /* callback on a miss, e.g., for speculative scheduling snatchback/replay */
  void (*miss_cb)(void * op, int expected_latency);
  bool (*translated_cb)(void * op, seq_t); /* returns true if TLB translation has completed */
  /* extract action_id; varies depending on whether we're dealing with a uop or IFQ entry */
  seq_t (*get_action_id)(void *);
  tick_t when_enqueued; /* primarily for MSHR's; used to provide FIFO scheduling */
  tick_t when_started;  /* primarily for MSHR's; set to TICK_T_MAX if in need of service */
  tick_t when_returned; /* primarily for MSHR's; set when value returned from higher level */
  tick_t pipe_exit_time; /* for tracking access latency */
  struct cache_action_t * MSHR_link; /* used for coalescing multiple MSHR requests to same addr */
  bool MSHR_linked; /* true if linked to earlier MSHR request */
};

struct cache_fill_t {
  int valid;
  md_paddr_t paddr;
  enum cache_command cmd;
  tick_t pipe_exit_time;
  struct core_t * core;
};

struct prefetch_buffer_t {
  md_paddr_t addr;
  struct prefetch_buffer_t * next;
};

struct prefetch_filter_t {
  char * table;
  int num_entries;
  int mask;
  int reset_interval;
  tick_t last_reset;
};

struct bus_t {
  char * name;
  int width; /* in bytes tranferrable per cycle */
  int ratio; /* number of cpu cycles per bus cycle */
  tick_t when_available;

  struct {
    counter_t accesses;
    counter_t utilization; /* cumulative cycles in use */
    counter_t prefetch_utilization; /* cumulative prefetch cycles in use */
  } stat;
};

struct cache_t {

  struct core_t * core; /* to which core does this belong? NULL = shared */
  struct uncore_t *uncore; /* to which uncore this belong? only if shared NULL = Private*/

  char * name;
  enum read_only_t read_only;
  bool frozen;

  int sets;
  int assoc;
  int log2_assoc; /* rounded up */

  int linesize;
  int addr_shift; /* to mask out the block offset */

  struct cache_line_t ** blocks;

  enum repl_policy_t replacement_policy;
  enum alloc_policy_t allocate_policy;
  enum write_policy_t write_policy;

  int banks;
  int bank_width; /* number of bytes read from a bank */
  int bank_shift;
  md_paddr_t bank_mask;

  const int begin_index;/* In case a shared cache what chunk lies with this cache*/
  const int end_index;


  int latency;
  int heap_size; /* for access event-Q heap */
  bool write_combining;
  struct cache_action_t ** pipe;    /* access pipeline for regular reads/writes */
  int * pipe_num; /* number of requests present in each bank */

  struct cache_fill_t ** fill_pipe; /* pipeline used to fill the cache from higher levels; one per bank */
  int * fill_num; /* number of fill requests present in each bank */

  /* miss status handling registers */
  int MSHR_banks; /* num MSHR banks */
  int MSHR_mask;
  int MSHR_size;
  enum cache_command * MSHR_cmd_order;
  int * MSHR_num; /* num MSHR entries occupied */
  int * MSHR_num_pf; /* num MSHR entries occupied by prefetch requests (from this level) */
  int * MSHR_fill_num; /* num MSHR entries pending to fill current level */
  int * MSHR_unprocessed_num; /* outstanding requests still waiting to go to next level */
  struct cache_action_t ** MSHR;
  int start_point;

  /* write-back buffers (only for write-back caches) - these are banked if MSHR is banked */
  int WBB_size;
  int * WBB_num;
  int * WBB_head;
  int * WBB_tail;
  struct cache_line_t ** WBB;

  /* prefetch FIFO */
  int PFF_size; /* PFB = PreFetch FIFO */
  int PFF_num;
  int PFF_head;
  int PFF_tail;
  struct PFF_t {
    md_addr_t PC;
    md_paddr_t addr;
    struct core_t * core;
  } * PFF;

  struct prefetch_t ** prefetcher;
  int num_prefetchers;
  /* prefetch control */
  int prefetch_threshold; /* only perform prefetch if MSHR occupancy is less than this threshold */
  int prefetch_max; /* max number of prefetch requests in MSHRs (from this level) */
  bool prefetch_on_miss; /* if true, prefetches only generated from miss traffic; else from all accesses */

  enum PF_state_t PF_state;
  int PF_last_sample; /* cumulative uncore utilization at last sample point */
  int PF_sample_interval;
  double PF_low_watermark;  /* {0.0 ... 1.0} */
  double PF_high_watermark; /* {0.0 ... 1.0} */

  struct prefetch_buffer_t * PF_buffer;
  int PF_buffer_size; /* num entries */
  struct prefetch_filter_t * PF_filter;

  /* to next level (toward main memory), e.g., DL1's next-level points to the LLC */
  struct cache_t * next_level;
  struct cache_t * prev_level;
  struct bus_t * next_bus;

  /* flags to indicate when there's nothing for the cache to do; helps
     speed up simulation */
  bool check_for_work;
  bool check_for_MSHR_fill_work;
  bool check_for_fill_work;
  bool check_for_pipe_work;
  bool check_for_MSHR_work;
  bool check_for_WBB_work;

  struct {
    counter_t load_lookups;
    counter_t load_misses;
    counter_t store_lookups;
    counter_t store_misses;
    counter_t writeback_lookups;
    counter_t writeback_misses;
    counter_t prefetch_lookups;
    counter_t prefetch_misses;
    counter_t prefetch_insertions;
    counter_t prefetch_useful_insertions;
    counter_t MSHR_occupancy; /* total occupancy */
    counter_t MSHR_full_cycles; /* number of cycles when full */
    counter_t WBB_insertions; /* total writebacks */
    counter_t WBB_victim_insertions; /* total non-dirty insertions */
    counter_t WBB_combines; /* number eliminated due to write combining */
    counter_t WBB_occupancy; /* total occupancy */
    counter_t WBB_full_cycles; /* number of cycles when full */
    counter_t WBB_hits;
    counter_t WBB_victim_hits;
    counter_t *core_lookups;
    counter_t *core_misses;
    counter_t MSHR_combos;
    counter_t uncore_utilization;/*This is only valid for the last level cache It is a measure of the traffic that is enqueued into the controller*/
  } stat;
};

struct cache_t * cache_create(
    struct core_t * const core,
    const char * const name,
    const bool read_only,
    const int sets,
    const int assoc,
    const int linesize,
    const char rp,
    const char ap,
    const char wp,
    const char wc,
    const int banks,
    const int bank_width,
    const int latency,
    const int WBB_size,
    const int MSHR_size,
    const int MSHR_banked,
    struct cache_t * const next_level_cache,
    struct bus_t * const bus_next);


struct cache_t * cache_create_llc(
    struct core_t * const core,
    const char * const name,
    const bool read_only,
    const int sets,
    const int assoc,
    const int linesize,
    const char rp,
    const char ap,
    const char wp,
    const char wc,
    const int banks,
    const int bank_width,
    const int latency,
    const int WBB_size,
    const int MSHR_size,
    const int MSHR_banked,
    const int begin_index,
    const int end_index,
    struct cache_t * const next_level_cache,
    struct bus_t * const bus_next);

void cache_reg_stats(
    struct stat_sdb_t * const sdb,
    const struct core_t * const core,
    struct cache_t * const cp);

void LLC_reg_stats(
    struct stat_sdb_t * const sdb,
    struct cache_t * const cp);

void cache_reset_stats(struct cache_t * const cp);

void prefetch_buffer_create(
    struct cache_t * const cp,
    const int num_entries);

void prefetch_filter_create(
    struct cache_t * const cp,
    const int num_entries,
    const int reset_interval);

void cache_process(struct cache_t * const cp);

void cache_process_llc(struct cache_t * const cp);

struct cache_line_t * cache_is_hit(
    struct cache_t * const cp,
    const enum cache_command cmd,
    const md_paddr_t addr,
    struct core_t * const core);

struct cache_line_t * cache_is_hit_llc(
    struct cache_t * const cp,
    const enum cache_command cmd,
    const md_paddr_t addr,
    struct core_t * const core);

void cache_insert_block(
    struct cache_t * const cp,
    const enum cache_command cmd,
    const md_paddr_t addr,
    struct core_t * const core);

struct cache_line_t * cache_invalidate(
    struct cache_t * const cp,
    const md_paddr_t addr,
    core_t * const core);

void cache_insert_block_llc(
    struct cache_t * const cp,
    const enum cache_command cmd,
    const md_paddr_t addr,
    struct core_t * const core);

struct cache_line_t * cache_get_evictee(
    struct cache_t * const cp,
    const md_paddr_t addr,
    struct core_t * const core);

struct cache_line_t * cache_get_evictee_llc(
    struct cache_t * const cp,
    const md_paddr_t addr,
    struct core_t * const core);

int cache_enqueuable(
    const struct cache_t * const cp,
    const int thread_id,
    const md_paddr_t addr);

int cache_enqueuable_llc(
    const struct cache_t * const cp,
    const int thread_id,
    const md_paddr_t addr);

void cache_enqueue(
    struct core_t * const core,
    struct cache_t * const cp,
    struct cache_t * const prev_cp,
    const enum cache_command cmd,
    const int thread_id,
    const md_addr_t PC,
    const md_paddr_t addr,
    const seq_t action_id,
    const int MSHR_bank,
    const int MSHR_index,
    void * const op,
    void (*const cb)(void *),
    void (*const miss_cb)(void *, int),
    bool (*const translated_cb)(void*,seq_t),
    seq_t (*const get_action_id)(void *));


void cache_enqueue_llc(
    struct core_t * const core,
    struct cache_t * const cp,
    struct cache_t * const prev_cp,
    const enum cache_command cmd,
    const int thread_id,
    const md_addr_t PC,
    const md_paddr_t addr,
    const seq_t action_id,
    const int MSHR_bank,
    const int MSHR_index,
    void * const op,
    void (*const cb)(void *),
    void (*const miss_cb)(void *, int),
    bool (*const translated_cb)(void*,seq_t),
    seq_t (*const get_action_id)(void *));

void fill_arrived(
    struct cache_t * const cp,
    const int MSHR_bank,
    const int MSHR_index);

void fill_arrived_llc(
    struct cache_t * const cp,
    unsigned long long int address);

void step_core_PF_controllers(struct core_t * const core);
void prefetch_core_caches(struct core_t * const core);
void step_LLC_PF_controller(struct uncore_t * const uncore);
void prefetch_LLC(struct uncore_t * const uncore);

void cache_freeze_stats(struct core_t * const core);

struct bus_t * bus_create(
    const char * const name,
    const int width,
    const int ratio);

void bus_reg_stats(
    struct stat_sdb_t * const sdb,
    struct core_t * const core,
    struct bus_t * const bus);

int bus_free(const struct bus_t * const bus);

void bus_use(
    struct bus_t * const bus,
    const int transfer_size,
    const int prefetch);

/* since these cache functions cannot directly set the core->oracle.hosed
   bit, they just return and depend on the rest of the core state getting
   hosed to force a reset. */
#ifndef cache_fatal
#ifdef DEBUG
#define cache_fatal(msg, retval) fatal(msg)
#else
#define cache_fatal(msg, retval) { \
  fprintf(stderr,"fatal (%s,%d:%s): ",__FILE__,__LINE__,cp->name); \
  fprintf(stderr,"%s\n",msg); \
  return (retval); \
}
#endif
#endif

#ifndef cache_assert
#ifdef DEBUG
#define cache_assert(cond, retval) assert(cond)
#else
#define cache_assert(cond, retval) { \
  if(!(cond)) { \
    fprintf(stderr,"assertion failed (%s,%d:%s): ",__FILE__,__LINE__,cp->name); \
    fprintf(stderr,"%s\n",#cond); \
    return (retval); \
  } \
}
#endif
#endif


#endif /* ZESTO_COMMIT_INCLUDED */
