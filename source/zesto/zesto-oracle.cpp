/* zesto-oracle.cpp - Zesto oracle functional simulator class
 *
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
 *
 * NOTE: This file (zesto-oracle.cpp) contains code directly and
 * indirectly derived from previous SimpleScalar source files.
 * These sections are demarkated with "<SIMPLESCALAR>" and
 * "</SIMPLESCALAR>" to specify the start and end, respectively, of
 * such source code.  Such code is bound by the combination of terms
 * and agreements from both Zesto and SimpleScalar.  In case of any
 * conflicting terms (for example, but not limited to, use by
 * commercial entities), the more restrictive terms shall take
 * precedence (e.g., commercial and for-profit entities may not
 * make use of the code without a license from SimpleScalar, LLC).
 * The SimpleScalar terms and agreements are replicated below as per
 * their original requirements.
 *
 * SimpleScalar � Tool Suite
 * � 1994-2003 Todd M. Austin, Ph.D. and SimpleScalar, LLC
 * All Rights Reserved.
 * 
 * THIS IS A LEGAL DOCUMENT BY DOWNLOADING SIMPLESCALAR, YOU ARE AGREEING TO
 * THESE TERMS AND CONDITIONS.
 * 
 * No portion of this work may be used by any commercial entity, or for any
 * commercial purpose, without the prior, written permission of SimpleScalar,
 * LLC (info@simplescalar.com). Nonprofit and noncommercial use is permitted as
 * described below.
 * 
 * 1. SimpleScalar is provided AS IS, with no warranty of any kind, express or
 * implied. The user of the program accepts full responsibility for the
 * application of the program and the use of any results.
 * 
 * 2. Nonprofit and noncommercial use is encouraged.  SimpleScalar may be
 * downloaded, compiled, executed, copied, and modified solely for nonprofit,
 * educational, noncommercial research, and noncommercial scholarship purposes
 * provided that this notice in its entirety accompanies all copies. Copies of
 * the modified software can be delivered to persons who use it solely for
 * nonprofit, educational, noncommercial research, and noncommercial
 * scholarship purposes provided that this notice in its entirety accompanies
 * all copies.
 * 
 * 3. ALL COMMERCIAL USE, AND ALL USE BY FOR PROFIT ENTITIES, IS EXPRESSLY
 * PROHIBITED WITHOUT A LICENSE FROM SIMPLESCALAR, LLC (info@simplescalar.com).
 * 
 * 4. No nonprofit user may place any restrictions on the use of this software,
 * including as modified by the user, by any other authorized user.
 * 
 * 5. Noncommercial and nonprofit users may distribute copies of SimpleScalar
 * in compiled or executable form as set forth in Section 2, provided that
 * either: (A) it is accompanied by the corresponding machine-readable source
 * code, or (B) it is accompanied by a written offer, with no time limit, to
 * give anyone a machine-readable copy of the corresponding source code in
 * return for reimbursement of the cost of distribution. This written offer
 * must permit verbatim duplication by anyone, or (C) it is distributed by
 * someone who received only the executable form, and is accompanied by a copy
 * of the written offer of source code.
 * 
 * 6. SimpleScalar was developed by Todd M. Austin, Ph.D. The tool suite is
 * currently maintained by SimpleScalar LLC (info@simplescalar.com). US Mail:
 * 2395 Timbercrest Court, Ann Arbor, MI 48105.
 * 
 * Copyright � 1994-2003 by Todd M. Austin, Ph.D. and SimpleScalar, LLC.
 */
#define ZESTO_ORACLE_C

#include <stddef.h>
#include <stdio.h>
#include <sys/io.h>
#include "zesto-qsim.h"
#include "misc.h"
#include "thread.h"
#include "syscall.h"
#include "sim.h"

#include "zesto-core.h"
#include "zesto-opts.h"
#include "zesto-oracle.h"
#include "zesto-fetch.h"
#include "zesto-bpred.h"
#include "zesto-decode.h"
#include "zesto-alloc.h"
#include "zesto-exec.h"
#include "zesto-commit.h"
#include "zesto-cache.h"
#include "zesto-uncore.h"


#ifdef USE_PIN_TRACES
md_addr_t store_nextPC[16];
bool use_stored_nextPC[16];
#else
//extern class Vm *v_ptr;
//extern class VmQ *vq_ptr;
const unsigned long TIME_QUANTUM = 0x00ff;
CDomain *cd=NULL;
#endif


bool core_oracle_t::static_members_initialized = false;
 /* for decode.dep_map */
struct core_oracle_t::map_node_t * core_oracle_t::map_free_pool = NULL;
int core_oracle_t::map_free_pool_debt = 0;
/* for oracle spec-memory map */
struct spec_byte_t * core_oracle_t::spec_mem_free_pool = NULL;
int core_oracle_t::spec_mem_pool_debt = 0;

/* CONSTRUCTOR */
core_oracle_t::core_oracle_t(struct core_t * const arg_core):
  spec_mode(false), hosed(false), MopQ(NULL), MopQ_head(0), MopQ_tail(0),
  MopQ_num(0), current_Mop(NULL), mem_req_free_pool(NULL),
  syscall_mem_req_head(NULL), syscall_mem_req_tail(NULL), syscall_mem_reqs(0),
  syscall_remaining_delay(0)
{
  /* MopQ should be large enough to support all in-flight
     instructions.  We assume one entry per "slot" in the machine
     (even though in uop-based structures, multiple slots may
     correspond to a single Mop), and then add a few and round up
     just in case. */
  core = arg_core;
  memset(&dep_map,0,sizeof(dep_map));
  memset(&spec_mem_map,0,sizeof(spec_mem_map));


  struct core_knobs_t * knobs = core->knobs;
  int temp_MopQ_size = knobs->commit.ROB_size +
                  knobs->alloc.depth * knobs->alloc.width +
                  knobs->decode.uopQ_size + knobs->decode.depth * knobs->decode.width +
                  knobs->fetch.IQ_size + knobs->fetch.depth * knobs->fetch.width +
                  knobs->fetch.byteQ_size + 64;
  MopQ_size = 1 << ((int)ceil(log(temp_MopQ_size)/log(2.0)));

  //MopQ = (struct Mop_t *)calloc(MopQ_size,sizeof(*MopQ));
  posix_memalign((void**)&MopQ,16,MopQ_size*sizeof(*MopQ));
  if(!MopQ)
    fatal("failed to calloc MopQ");
  memset(MopQ,0,MopQ_size*sizeof(*MopQ));
  assert(sizeof(*MopQ) % 16 == 0); // size of Mop is mult of 16
  assert(sizeof(struct uop_t) % 16 == 0); // size of uop is mult of 16
  /* head, tail and num will have been calloc'd to zero */

  /* This shouldn't be necessary, but I threw it in because valgrind
     (--tool=memcheck) was reporting that Mop->uop was being used
     uninitialized. */
  int i;
  for(i=0;i<MopQ_size;i++)
    MopQ[i].uop = NULL;
}

// DEBUG
static counter_t syscall_mem_accesses = 0;

/* register oracle-related stats in the stat-database (sdb) */
void
core_oracle_t::reg_stats(struct stat_sdb_t * const sdb)
{
  char buf[1024];
  char buf2[1024];
  struct thread_t * arch = core->current_thread;

  stat_reg_note(sdb,"\n#### ORACLE STATS ####");
  sprintf(buf,"c%d.oracle_num_insn",arch->id);
  sprintf(buf2,"c%d.oracle_total_insn - c%d.oracle_insn_undo",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "number of instructions executed by oracle", buf2, "%12.0f");
  sprintf(buf,"c%d.oracle_total_insn",arch->id);
  stat_reg_counter(sdb, true, buf, "total number of instructions executed by oracle, including misspec", &core->stat.oracle_total_insn, core->stat.oracle_total_insn, NULL);
  sprintf(buf,"c%d.oracle_insn_undo",arch->id);
  stat_reg_counter(sdb, false, buf, "total number of instructions undone by oracle (misspeculated insts)", &core->stat.oracle_inst_undo, core->stat.oracle_inst_undo, NULL);
  sprintf(buf,"c%d.oracle_num_uops",arch->id);
  sprintf(buf2,"c%d.oracle_total_uops - c%d.oracle_uop_undo",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "number of uops executed by oracle", buf2, "%12.0f");
  sprintf(buf,"c%d.oracle_total_uops",arch->id);
  stat_reg_counter(sdb, true, buf, "total number of uops executed by oracle, including misspec", &core->stat.oracle_total_uops, core->stat.oracle_total_uops, NULL);
  sprintf(buf,"c%d.oracle_uop_undo",arch->id);
  stat_reg_counter(sdb, false, buf, "total number of uops undone by oracle", &core->stat.oracle_uop_undo, core->stat.oracle_uop_undo, NULL);
  sprintf(buf,"c%d.oracle_num_eff_uops",arch->id);
  sprintf(buf2,"c%d.oracle_total_eff_uops - c%d.oracle_eff_uop_undo",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "number of effective uops executed by oracle", buf2, "%12.0f");
  sprintf(buf,"c%d.oracle_total_eff_uops",arch->id);
  stat_reg_counter(sdb, true, buf, "total number of effective uops executed by oracle, including misspec", &core->stat.oracle_total_eff_uops, core->stat.oracle_total_eff_uops, NULL);
  sprintf(buf,"c%d.oracle_eff_uop_undo",arch->id);
  stat_reg_counter(sdb, false, buf, "total number of effective uops undone by oracle", &core->stat.oracle_eff_uop_undo, core->stat.oracle_eff_uop_undo, NULL);

  sprintf(buf,"c%d.oracle_IPC",arch->id);
  sprintf(buf2,"c%d.oracle_num_insn / c%d.sim_cycle",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "IPC at oracle", buf2, NULL);
  sprintf(buf,"c%d.oracle_uPC",arch->id);
  sprintf(buf2,"c%d.oracle_num_uops / c%d.sim_cycle",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "uPC at oracle", buf2, NULL);
  sprintf(buf,"c%d.oracle_euPC",arch->id);
  sprintf(buf2,"c%d.oracle_num_eff_uops / c%d.sim_cycle",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "euPC at oracle", buf2, NULL);
  sprintf(buf,"c%d.oracle_total_IPC",arch->id);
  sprintf(buf2,"c%d.oracle_total_insn / c%d.sim_cycle",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "IPC at oracle, including wrong-path", buf2, NULL);
  sprintf(buf,"c%d.oracle_total_uPC",arch->id);
  sprintf(buf2,"c%d.oracle_total_uops / c%d.sim_cycle",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "uPC at oracle, including wrong-path", buf2, NULL);
  sprintf(buf,"c%d.oracle_total_euPC",arch->id);
  sprintf(buf2,"c%d.oracle_total_eff_uops / c%d.sim_cycle",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "euPC at oracle, including wrong-path", buf2, NULL);
  sprintf(buf,"c%d.avg_oracle_flowlen",arch->id);
  sprintf(buf2,"c%d.oracle_num_uops / c%d.oracle_num_insn",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "uops per instruction at oracle", buf2, NULL);
  sprintf(buf,"c%d.avg_oracle_eff_flowlen",arch->id);
  sprintf(buf2,"c%d.oracle_num_eff_uops / c%d.oracle_num_insn",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "effective uops per instruction at oracle", buf2, NULL);
  sprintf(buf,"c%d.avg_oracle_total_flowlen",arch->id);
  sprintf(buf2,"c%d.oracle_total_uops / c%d.oracle_total_insn",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "uops per instruction at oracle, including wrong-path", buf2, NULL);
  sprintf(buf,"c%d.avg_oracle_total_eff_flowlen",arch->id);
  sprintf(buf2,"c%d.oracle_total_eff_uops / c%d.oracle_total_insn",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "effective uops per instruction at oracle, including wrong-path", buf2, NULL);

  sprintf(buf,"c%d.oracle_num_refs",arch->id);
  stat_reg_counter(sdb, true, buf, "total number of loads and stores executed by oracle", &core->stat.oracle_num_refs, core->stat.oracle_num_refs, NULL);
  sprintf(buf,"c%d.oracle_num_loads",arch->id);
  stat_reg_counter(sdb, true, buf, "total number of loads executed by oracle", &core->stat.oracle_num_loads, core->stat.oracle_num_loads, NULL);
  sprintf(buf2,"c%d.oracle_num_refs - c%d.oracle_num_loads",arch->id,arch->id);
  sprintf(buf,"c%d.oracle_num_stores",arch->id);
  stat_reg_formula(sdb, true, buf, "total number of stores executed by oracle", buf2, "%12.0f");
  sprintf(buf,"c%d.oracle_num_branches",arch->id);
  stat_reg_counter(sdb, true, buf, "total number of branches executed by oracle", &core->stat.oracle_num_branches, core->stat.oracle_num_branches, NULL);


  sprintf(buf,"c%d.oracle_total_refs",arch->id);
  stat_reg_counter(sdb, true, buf, "total number of loads and stores executed by oracle, including wrong-path", &core->stat.oracle_total_refs, core->stat.oracle_total_refs, NULL);
  sprintf(buf,"c%d.oracle_total_loads",arch->id);
  stat_reg_counter(sdb, true, buf, "total number of loads executed by oracle, including wrong-path", &core->stat.oracle_total_loads, core->stat.oracle_total_loads, NULL);
  sprintf(buf2,"c%d.oracle_total_refs - c%d.oracle_total_loads",arch->id,arch->id);
  sprintf(buf,"c%d.oracle_total_stores",arch->id);
  stat_reg_formula(sdb, true, buf, "total number of stores executed by oracle, including wrong-path", buf2, "%12.0f");
  sprintf(buf,"c%d.oracle_total_branches",arch->id);
  stat_reg_counter(sdb, true, buf, "total number of branches executed by oracle, including wrong-path", &core->stat.oracle_total_branches, core->stat.oracle_total_branches, NULL);
  sprintf(buf,"c%d.MopQ_occupancy",arch->id);
  stat_reg_counter(sdb, true, buf, "total oracle MopQ occupancy", &core->stat.MopQ_occupancy, core->stat.MopQ_occupancy, NULL);
  sprintf(buf,"c%d.MopQ_avg",arch->id);
  sprintf(buf2,"c%d.MopQ_occupancy/c%d.sim_cycle",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "average oracle MopQ occupancy", buf2, NULL);
  sprintf(buf,"c%d.MopQ_full",arch->id);
  stat_reg_counter(sdb, true, buf, "total cycles oracle MopQ was full", &core->stat.MopQ_full_cycles, core->stat.MopQ_full_cycles, NULL);
  sprintf(buf,"c%d.MopQ_frac_full",arch->id);
  sprintf(buf2,"c%d.MopQ_full/c%d.sim_cycle",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "fraction of cycles oracle MopQ was full", buf2, NULL);
  sprintf(buf,"c%d.oracle_bogus_cycles",arch->id);
  stat_reg_counter(sdb, true, buf, "total cycles oracle stalled on invalid wrong-path insts", &core->stat.oracle_bogus_cycles, core->stat.oracle_bogus_cycles, NULL);
  sprintf(buf,"c%d.oracle_frac_bogus",arch->id);
  sprintf(buf2,"c%d.oracle_bogus_cycles/c%d.sim_cycle",arch->id,arch->id);
  stat_reg_formula(sdb, true, buf, "fraction of cycles oracle stalled on invalid wrong-path insts", buf2, NULL);
  sprintf(buf,"c%d.oracle_emergency_recoveries",arch->id);
  stat_reg_counter(sdb, true, buf, "number of times this thread underwent an emergency recovery", &core->num_emergency_recoveries, core->num_emergency_recoveries, NULL);

  // DEBUG
  sprintf(buf,"c%d.syscall_mem_accesses",arch->id);
  stat_reg_counter(sdb, true, buf, "memory accesses made by system calls", &syscall_mem_accesses, syscall_mem_accesses, NULL);
}

void core_oracle_t::update_occupancy(void)
{
    /* MopQ */
  core->stat.MopQ_occupancy += MopQ_num;
  if(MopQ_num >= MopQ_size)
    core->stat.MopQ_full_cycles++;
}

struct Mop_t * core_oracle_t::get_Mop(const int index)
{
  return &MopQ[index];
}

int core_oracle_t::get_index(const struct Mop_t * const Mop)
{
  return Mop - MopQ;
}

int core_oracle_t::next_index(const int index)
{
  return modinc(index,MopQ_size); //return (index+1)%MopQ_size;
}

/* create a new entry in the spec_mem table, insert the entry,
   and return a pointer to the entry. */
struct spec_byte_t * core_oracle_t::spec_write_byte(
    const md_addr_t addr,
    const byte_t val)
{
  int index = addr & MEM_HASH_MASK;
  struct spec_byte_t * p = get_spec_mem_node();
  if(spec_mem_map.hash[index].tail)
    spec_mem_map.hash[index].tail->next = p;
  p->prev = spec_mem_map.hash[index].tail;
  spec_mem_map.hash[index].tail = p;
  if(spec_mem_map.hash[index].head == NULL)
    spec_mem_map.hash[index].head = p;

  p->val = val;
  p->addr = addr;
  return p;
}

/* return true/false if byte is in table (read from most recent).
   If present, store value in valp. */
bool core_oracle_t::spec_read_byte(
    const md_addr_t addr,
    byte_t * const valp)
{
  int index = addr & MEM_HASH_MASK;
  struct spec_byte_t * p = spec_mem_map.hash[index].tail;
  while(p)
  {
    if(p->addr == addr)
    {
      *valp = p->val;
      return true;
    }
    p = p->prev;
  }

  return false;
}

/* wrapper so that system call's memory accesses get filtered
   through the cache hierarchy. */

enum md_fault_type
core_oracle_t::syscall_mem_access(
    int thread_id,
    struct mem_t *mem,		/* memory space to access */
    enum mem_cmd cmd,		/* Read (from sim mem) or Write */
    md_addr_t addr,		/* target address to access */
    void *vp,			/* host memory address to access */
    int nbytes)			/* number of bytes to access */
{
  struct core_t * core = cores[threads[thread_id]->current_core];
  if(core->knobs->memory.syscall_memory_latency)
  {
    struct core_oracle_t * O = core->oracle;
    enum cache_command c_cmd = (cmd==Read)?CACHE_READ:CACHE_WRITE;

    /* The system call accesses perform the reads/writes
       one byte at a time.  As such, check to see if we
       have back-to-back reads (or writes) to the same
       eight-byte chunk; if so, don't record the subsequent
       access(es) as separate events. */
    bool same_access = false;
    struct syscall_mem_req_t * p;

    if(O->syscall_mem_req_tail)
    {
      p = O->syscall_mem_req_tail;
      if((p->thread_id == thread_id) &&
         (p->cmd == c_cmd) && ((p->addr&~0x7) == (addr&~0x7)))
         same_access = true;
    }

    if(!same_access)
    {
      syscall_mem_accesses++;
      if(O->mem_req_free_pool)
      {
        p = O->mem_req_free_pool;
        O->mem_req_free_pool = O->mem_req_free_pool->next;
      }
      else
      {
        p = (struct syscall_mem_req_t*) calloc(1,sizeof(*p));
        if(!p)
          fatal("cannot calloc syscall memory request struct");
      }
      p->next = NULL;
      p->addr = addr;
      p->thread_id = thread_id;
      p->cmd = c_cmd;

      if(O->syscall_mem_req_tail) /* append to list if any */
      {
        assert(O->syscall_mem_req_head);
        O->syscall_mem_req_tail->next = p;
        O->syscall_mem_req_tail = p;
      }
      else /* else start new list */
      {
        O->syscall_mem_req_head = p;
        O->syscall_mem_req_tail = p;
      }

      O->syscall_mem_reqs++;
    }
  }

  return mem_access(thread_id,mem,cmd,addr,vp,nbytes);
}

/* dummy callbacks for system call memory requests */
void core_oracle_t::syscall_callback(void * const op)
{
  /* nada */
}

bool core_oracle_t::syscall_translated_callback(void * const op, const seq_t action_id)
{
  return true;
}

seq_t core_oracle_t::syscall_get_action_id(void * const op)
{
  return DUMMY_SYSCALL_ACTION_ID;
}

  /*----------------*/
 /* <SIMPLESCALAR> */
/*----------------*/

/**********************************************************/
/* CODE FOR ACTUAL ORACLE EXECUTION, RECOVERY, AND COMMIT */
/**********************************************************/

/*
 * oracle's register accessors
 */

/* general purpose registers */

#if defined(TARGET_X86)
/* current program counter */
#define CPC            (thread->regs.regs_PC)

/* next program counter */
#define NPC            (thread->regs.regs_NPC)
#define SET_NPC(EXPR)        (thread->regs.regs_NPC )// = (EXPR))
#define SET_NPC_D(EXPR)         SET_NPC(EXPR)
#define SET_NPC_V(EXPR)                            \
  ((Mop->fetch.inst.mode & MODE_OPER32) ? SET_NPC((EXPR)) : SET_NPC((EXPR) & 0xffff))

                                                 /* general purpose registers */
#define _HI(N)            ((N) & 0x04)
#define _ID(N)            ((N) & 0x03)
#define _ARCH(N)        ((N) < MD_REG_TMP0)

                                                 /* segment registers ; UCSD*/
#define SEG_W(N)        (thread->regs.regs_S.w[N])
#define SET_SEG_W(N,EXPR)    (thread->regs.regs_S.w[N] = (EXPR))

#define GPR_B(N)        (_ARCH(N)                \
    ? (_HI(N)                \
      ? thread->regs.regs_R.b[_ID(N)].hi        \
      : thread->regs.regs_R.b[_ID(N)].lo)      \
    : thread->regs.regs_R.b[N].lo)
#define SET_GPR_B(N,EXPR)    (_ARCH(N)                   \
    ? (_HI(N)                   \
      ? (thread->regs.regs_R.b[_ID(N)].hi = (EXPR))  \
      : (thread->regs.regs_R.b[_ID(N)].lo = (EXPR))) \
    : (thread->regs.regs_R.b[N].lo = (EXPR)))

#define GPR_W(N)        (thread->regs.regs_R.w[N].lo)
#define SET_GPR_W(N,EXPR)    (thread->regs.regs_R.w[N].lo = (EXPR))

#define GPR_D(N)        (thread->regs.regs_R.dw[N])
#define SET_GPR_D(N,EXPR)    (thread->regs.regs_R.dw[N] = (EXPR))

                                                 /* FIXME: move these to the DEF file? */
#define GPR_V(N)        ((Mop->fetch.inst.mode & MODE_OPER32)        \
    ? GPR_D(N)                \
    : (dword_t)GPR_W(N))
#define SET_GPR_V(N,EXPR)    ((Mop->fetch.inst.mode & MODE_OPER32)        \
    ? SET_GPR_D(N, EXPR)            \
    : SET_GPR_W(N, EXPR))

#define GPR_A(N)        ((Mop->fetch.inst.mode & MODE_ADDR32)        \
    ? GPR_D(N)                \
    : (dword_t)GPR_W(N))
#define SET_GPR_A(N,EXPR)    ((Mop->fetch.inst.mode & MODE_ADDR32)        \
    ? SET_GPR_D(N, EXPR)            \
    : SET_GPR_W(N, EXPR))

#define GPR_S(N)        ((Mop->fetch.inst.mode & MODE_STACK32)        \
    ? GPR_D(N)                \
    : (dword_t)GPR_W(N))
#define SET_GPR_S(N,EXPR)    ((Mop->fetch.inst.mode & MODE_STACK32)        \
    ? SET_GPR_D(N, EXPR)            \
    : SET_GPR_W(N, EXPR))

/* these are to read/write a 32-bit register corresponding
   to an 8-bit register name (AL/AH/BL/...) */
#define GPR_DB(N)		(_ARCH(N)				\
    ? thread->regs.regs_R.dw[_ID(N)]		\
    : thread->regs.regs_R.dw[N])
#define SET_GPR_DB(N,EXPR)	(_ARCH(N)				   \
    ? (thread->regs.regs_R.dw[_ID(N)] = (EXPR))  \
    : (thread->regs.regs_R.dw[N] = (EXPR)))

#define GPR(N)                  GPR_D(N)
#define SET_GPR(N,EXPR)         SET_GPR_D(N,EXPR)

#define AFLAGS(MSK)        (thread->regs.regs_C.aflags & (MSK))
#define SET_AFLAGS(EXPR,MSK)    (assert(((EXPR) & ~(MSK)) == 0),    \
    thread->regs.regs_C.aflags =            \
    ((thread->regs.regs_C.aflags & ~(MSK))    \
     | ((EXPR) & (MSK))))
#define DAFLAGS(MSK) (MSK)

#define FSW(MSK)        (thread->regs.regs_C.fsw & (MSK))
#define SET_FSW(EXPR,MSK)    (assert(((EXPR) & ~(MSK)) == 0),    \
    thread->regs.regs_C.fsw =            \
    ((thread->regs.regs_C.fsw & ~(MSK))        \
     | ((EXPR) & (MSK))))

                                                 /* added by cristiano */
#define CWD(MSK)                (thread->regs.regs_C.cwd & (MSK))
#define SET_CWD(EXPR,MSK)       (assert(((EXPR) & ~(MSK)) == 0),        \
    thread->regs.regs_C.cwd =                      \
    ((thread->regs.regs_C.cwd & ~(MSK))            \
     | ((EXPR) & (MSK))))

                                                 /* floating point registers, L->word, F->single-prec, D->double-prec */
                                                 /* FIXME: bad overlap, also need to support stack indexing... */
#define _FPARCH(N)      ((N) < MD_REG_FTMP0)
#define F2P(N)          (_FPARCH(N) ? ((FSW_TOP(thread->regs.regs_C.fsw) + (N)) & 0x07) : (N))
#define FPR(N)            (thread->regs.regs_F.e[F2P(N)])
#define SET_FPR(N,EXPR)        (thread->regs.regs_F.e[F2P(N)] = (EXPR))

#define FPSTACK_POP()   SET_FSW_TOP(thread->regs.regs_C.fsw, (FSW_TOP(thread->regs.regs_C.fsw) + 1) & 0x07)
#define FPSTACK_PUSH()  SET_FSW_TOP(thread->regs.regs_C.fsw, (FSW_TOP(thread->regs.regs_C.fsw) + 7) & 0x07)

#define FPSTACK_IMPL(OP)                            \
{                                    \
  /* if ((OP) == fpstk_nop) - already checked before call of macro */                        \
  /* ; */                            \
  /* else */ if ((OP) == fpstk_pop)                        \
    SET_FSW_TOP(thread->regs.regs_C.fsw, (FSW_TOP(thread->regs.regs_C.fsw)+1)&0x07);\
  else if ((OP) == fpstk_poppop)                    \
    SET_FSW_TOP(thread->regs.regs_C.fsw, (FSW_TOP(thread->regs.regs_C.fsw)+2)&0x07);\
  else if ((OP) == fpstk_push)                    \
    SET_FSW_TOP(thread->regs.regs_C.fsw, (FSW_TOP(thread->regs.regs_C.fsw)+7)&0x07);\
  else                                \
  panic("bogus FP stack operation");                \
}

#else
#error No ISA target defined (only x86 supported) ...
#endif



/* speculative memory state accessor macros */
#ifdef TARGET_X86

#define READ_BYTE(SRC, FAULT)     ((FAULT) = md_fault_none, uop->oracle.virt_addr = (SRC), uop->decode.mem_size = 1, XMEM_READ_BYTE(thread->mem, (SRC)))
#define READ_WORD(SRC, FAULT)     ((FAULT) = md_fault_none, uop->oracle.virt_addr = (SRC), uop->decode.mem_size = 2, XMEM_READ_WORD(thread->mem, (SRC)))
#define READ_DWORD(SRC, FAULT)     ((FAULT) = md_fault_none, uop->oracle.virt_addr = (SRC), uop->decode.mem_size = 4, XMEM_READ_DWORD(thread->mem, (SRC)))
#define READ_QWORD(SRC, FAULT)    ((FAULT) = md_fault_none, uop->oracle.virt_addr = (SRC), uop->decode.mem_size = 8, XMEM_READ_QWORD(thread->mem, (SRC)))

#define WRITE_BYTE(SRC, DST, FAULT)   ((FAULT) = md_fault_none, uop->oracle.virt_addr = (DST), uop->decode.mem_size = 1, XMEM_WRITE_BYTE(thread->mem, (DST), (SRC)))
#define WRITE_WORD(SRC, DST, FAULT)   ((FAULT) = md_fault_none, uop->oracle.virt_addr = (DST), uop->decode.mem_size = 2, XMEM_WRITE_WORD(thread->mem, (DST), (SRC)))
#define WRITE_DWORD(SRC, DST, FAULT)   ((FAULT) = md_fault_none, uop->oracle.virt_addr = (DST), uop->decode.mem_size = 4, XMEM_WRITE_DWORD(thread->mem, (DST), (SRC)))
#define WRITE_QWORD(SRC, DST, FAULT)  ((FAULT) = md_fault_none, uop->oracle.virt_addr = (DST), uop->decode.mem_size = 8, XMEM_WRITE_QWORD(thread->mem, (DST), (SRC)))

/* this is for FSTE */
#define WRITE_DWORD2(SRC, DST, FAULT)   (uop->decode.mem_size = 12, XMEM_WRITE_DWORD2(thread->mem, (DST), (SRC)))

#else /* !TARGET_X86 */
#error No ISA target defined (only x86 supported) ...
#endif

                                                 /* system call handler macro */
#define SYSCALL(INST)     unsigned int tni=0

  /*-----------------*/
 /* </SIMPLESCALAR> */
/*-----------------*/


/* Inst/uop execution functions: doing this allows you to actually
   compile this file (zesto-oracle.c) with optimizations turned on
   (e.g. gcc -O3), since without it, the giant switch was making gcc
   run out of memory. */
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,I1,I2,I3,OFLAGS,IFLAGS)\
static inline void SYMCAT(OP,_IMPL_FUNC)(struct thread_t * const thread, struct core_t * const core, struct Mop_t * const Mop, struct uop_t * const uop, bool * const bogus)               \
     SYMCAT(OP,_IMPL)
#define DEFUOP(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,I1,I2,I3,OFLAGS,IFLAGS)\
static inline void SYMCAT(OP,_IMPL_FUNC)(struct thread_t * const thread, struct core_t * const core, struct Mop_t * const Mop, struct uop_t * const uop, bool * const bogus)               \
     SYMCAT(OP,_IMPL)
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)
#define CONNECT(OP)
#define DECLARE_FAULT(FAULT)                        \
      { uop->oracle.fault = (FAULT); return; }
#include "machine.def"
#undef DEFINST
#undef DEFUOP
#undef DEFLINK
#undef DECLARE_FAULT


/* The following code is derived from the original execution
   code in the SimpleScalar/x86 pre-release, but it has been
   extensively modified for the way we're doing things in Zesto.
   Major changes include the handling of REP instructions and
   handling of execution down wrong control paths. */
struct Mop_t *
core_oracle_t::exec(const md_addr_t requested_PC)
{
  struct thread_t * thread = core->current_thread;
  struct core_knobs_t * knobs = core->knobs;
  int flow_index = 0;        /* index into flowtab */
  uop_inst_t flowtab[MD_MAX_FLOWLEN+2];    /* table of uops, +2 for REP control uops */
  struct Mop_t * Mop = NULL;
  bool * bogus = &core->fetch->bogus;

  int wrote_spec_mem = false;

  //////fprintf(stdout,"\n\n[%lld]Oracle.start()  Thread->PC= 0x%llx Thread->NPC= 0x%llx Requested PC 0x%llx spec_mode %d MopQ_num %d MopQ_size %d", sim_cycle, thread->regs.regs_PC, thread->regs.regs_NPC,requested_PC, spec_mode, MopQ_num, MopQ_size);

  
  if(*bogus) /* are we on a wrong path and fetched bogus insts? */
  {
    assert(spec_mode);
    ZESTO_STAT(core->stat.oracle_bogus_cycles++;)
    return NULL;
  }

  //if(core->stat.oracle_total_insn > 20000000)
	core->uncore->log_trace=true;

//  /* process the memory requests generated by the system call*/
//  if(syscall_mem_reqs > 0)
//  {
//    if(MopQ_num <= 1)
//    {
//      struct syscall_mem_req_t * p = syscall_mem_req_head;
//      assert(p);
//
//      /* wait N cycles between launching successive memory requests */
//      if(knobs->memory.syscall_memory_latency && syscall_remaining_delay > 0)
//      {
//        syscall_remaining_delay --;
//        return NULL;
//      }
//
//      if(!cache_enqueuable(core->memory.DL1,core->current_thread->id,p->addr))
//        return NULL;
//
//      cache_enqueue(core,core->memory.DL1,NULL,p->cmd,core->current_thread->id,0,p->addr,DUMMY_SYSCALL_ACTION_ID,0,NO_MSHR,(void*)DUMMY_SYSCALL_OP,syscall_callback,NULL,syscall_translated_callback,syscall_get_action_id);
//      core->exec->update_last_completed(sim_cycle);
//      syscall_remaining_delay = knobs->memory.syscall_memory_latency - 1; /* credit this cycle */
//
//    /* successfully enqueued the request, so remove the request from
//         the list */
//    syscall_mem_req_head = p->next;
//    syscall_mem_reqs--;
//    if(syscall_mem_req_head == NULL)
//      {
//        syscall_mem_req_tail = NULL;
//        assert(syscall_mem_reqs == 0);
//      }
//
//      /* put back into free pool */
//      p->next = mem_req_free_pool;
//      mem_req_free_pool = p;
//
//      assert(syscall_mem_reqs >= 0);
//
//      return NULL;
//    }
//    else
//      return NULL;
//  }
//  assert(syscall_mem_reqs == 0);

  //if(sim_cycle==2599461)
  	//exit(0);  

  if(current_Mop) /* we already have a Mop */
  {    
    if(current_Mop->decode.is_trap) /* this isn't necessary, but it saves us redecoding the trap over and over again */
    {
      //////fprintf(stdout,"\n[%lld]Oracle TRAP-macro-op unconsumed Come on",sim_cycle);
      /* make sure pipeline has drained */
      if(MopQ_num > 1) /* 1 since the trap itself is in the MopQ */
      {
	  //////fprintf(stdout,"\n[%lld]Oracle.end()  Trap instruction waiting for pipeline to drain Thread->PC= 0x%llx Thread->NPC= 0x%llx \n", sim_cycle, thread->regs.regs_PC, thread->regs.regs_NPC);
	  return NULL;
      }
    }
    else
    {
	    //fprintf(stdout,"\nI am here Crossed current_Mop is not a TRAP\n ");
	    assert(current_Mop->uop->timing.when_ready == TICK_T_MAX);
	    assert(current_Mop->uop->timing.when_issued == TICK_T_MAX);
	    assert(current_Mop->uop->timing.when_exec == TICK_T_MAX);
	    assert(current_Mop->uop->timing.when_completed == TICK_T_MAX);
    }
    //////fprintf(stdout,"\n[%lld]Oracle CurrentMop available unconsumed \n\n", sim_cycle);
    //////fprintf(stdout,"\n[%lld]Oracle.end()  Thread->PC= 0x%llx Thread->NPC= 0x%llx ", sim_cycle, thread->regs.regs_PC, thread->regs.regs_NPC);
    return current_Mop;
  }
  else
  {
    Mop = &MopQ[MopQ_tail];
  }

  
  if(MopQ_num >= MopQ_size)
  {
    /* warnonce("MopQ full: consider increasing MopQ size"); */
    return NULL;
  }

  /* reset Mop state; we return the uop array here (rather than when the Mop is
     committed) just to leave information around to help debug insts that have
     already committed. */
  if(Mop->uop)
    core->return_uop_array(Mop->uop);
  core->zero_Mop(Mop);
  Mop->timing.when_fetch_started = TICK_T_MAX;
  Mop->timing.when_fetched = TICK_T_MAX;
  Mop->timing.when_MS_started = TICK_T_MAX;
  Mop->timing.when_decode_started = TICK_T_MAX;
  Mop->timing.when_decode_finished = TICK_T_MAX;
  Mop->timing.when_commit_started = TICK_T_MAX;
  Mop->timing.when_commit_finished = TICK_T_MAX;
  Mop->valid = true;
  Mop->core = core;

  /* go to next instruction */
  if(requested_PC != thread->regs.regs_NPC)
  {
    //////if(spec_mode==false)
	//////fprintf(stdout,"\n[%lld]Oracle: Going down the wrong path",sim_cycle);
	
    spec_mode = true;
    //////fprintf(stdout,"\n[%lld]Oracle: Issuing no-ops",sim_cycle);
  }
  Mop->oracle.spec_mode = spec_mode;

  thread->regs.regs_PC = requested_PC;

  if(spec_mode)
  {
	Mop->fetch.inst.code[0]=MD_NOP_INST.code[0];
	MD_SET_OPCODE_DURING_FETCH(Mop->decode.op, Mop->fetch.inst);
	if(Mop->decode.op == OP_NA)
		Mop->decode.op = NOP;
	Mop->decode.opflags = MD_OP_FLAGS(Mop->decode.op);
	thread->regs.regs_NPC=thread->regs.regs_PC;
  }
  else
  {

refetch_nextPC:
	/* get the next instruction to execute */
	/* read raw bytes from virtual memory */
	MD_FETCH_INST(Mop->fetch.inst, thread->mem, thread->regs.regs_PC, core->id);
	/* then decode the instruction */
	MD_SET_OPCODE_DURING_FETCH(Mop->decode.op, Mop->fetch.inst);

	Mop->fetch.inst.len=Mop->fetch.inst.qemu_len;
	//////fprintf(stdout,"\n[%lld]Oracle.start DequeuedPC: 0x%llx   ",sim_cycle,Mop->fetch.inst.vaddr);
	/////for (int i = 0; i < Mop->fetch.inst.qemu_len; i++) 
		//////fprintf(stdout,"%x ", Mop->fetch.inst.code[i]);

	/*Copying the Physical and virtual address of fetch.inst into the Macro-op struct addr
	This is not necessary but is more cleaner to add an addr struct to handle pagetables*/
        //Mop->addr.vaddr=Mop->fetch.inst.vaddr;
	//Mop->addr.paddr=Mop->fetch.inst.paddr;
   

	md_addr_t nextPC,PC;
	MD_FETCH_NEXT_PC(nextPC,core->id);
	if(nextPC==0)
	{
		thread->regs.regs_NPC = thread->regs.regs_PC + MD_INST_SIZE(Mop->fetch.inst);
		//fprintf(stdout,"\n[%lld]Oracle nextPC is zero!! Adding inst.len %llx Instr Length: %d",sim_cycle,thread->regs.regs_NPC,Mop->fetch.inst.len);
	}
	else
	{
		thread->regs.regs_NPC=nextPC;
		//////fprintf(stdout,"\n[%lld]Oracle nextPC is %llx Instr Length: %d",sim_cycle,nextPC,Mop->fetch.inst.len);
	}
	/* convert XCHG X,X to NOPs (XCHG EAX,EAX already identified-as/synonomous-with NOP) */
	if( ((Mop->decode.op == XCHG_RMvRv) || (Mop->decode.op == XCHG_RMbRb)) && (R==RM))
		Mop->decode.op = NOP;
	if(Mop->decode.op == OP_NA)
		Mop->decode.op = NOP;

	Mop->decode.rep_seq = thread->rep_sequence;

	Mop->decode.opflags = MD_OP_FLAGS(Mop->decode.op);
	if(Mop->fetch.inst.rep)
		Mop->decode.opflags |= F_COND|F_CTRL;
	Mop->decode.is_ctrl = !!(Mop->decode.opflags & F_CTRL);
	if(!Mop->decode.is_ctrl && !Mop->fetch.inst.rep)	//TODO it broke after two days check if any control instructions can come consecutively like the '88 7' case
	{
		if(thread->regs.regs_PC==thread->regs.regs_NPC)
			goto refetch_nextPC;
	}
  }
  Mop->decode.is_trap = !!(Mop->decode.opflags & F_TRAP);
  Mop->decode.is_ctrl = !!(Mop->decode.opflags & F_CTRL);

  if(Mop->decode.opflags & F_FENCE)
  {
	//fprintf(stdout,"\n[%lld] Fence Instruction",sim_cycle);
  }
  

  //set up initial default next PC, THis is removed because now I know the next PC from the QEMU trace
  //thread->regs.regs_NPC = thread->regs.regs_PC + MD_INST_SIZE(Mop->fetch.inst);

  //if((!Mop->decode.is_ctrl) || (!Mop->fetch.inst.rep))
  	//thread->regs.regs_NPC = thread->regs.regs_PC + MD_INST_SIZE(Mop->fetch.inst);


  /* grab values for Mop */
  Mop->fetch.PC = thread->regs.regs_PC;
  Mop->fetch.pred_NPC = thread->regs.regs_PC + MD_INST_SIZE(Mop->fetch.inst);


  if(Mop->decode.is_ctrl)
  {
	if(thread->regs.regs_NPC!= (thread->regs.regs_PC + MD_INST_SIZE(Mop->fetch.inst) ) )
		Mop->decode.targetPC=thread->regs.regs_NPC;
	else// TODO Check if this is correct Is just adding the IMM offset enough, In case a branch is not taken QEMU does not tell me what the target is! Thus I need to make an intelligent guess as I dont keep values
		Mop->decode.targetPC=thread->regs.regs_PC + MD_INST_SIZE(Mop->fetch.inst)+ Mop->fetch.inst.imm;
  }




  /* set unique id */
  Mop->oracle.seq = core->global_seq++;

  /*********************************************************/
  /* REP implementation:
     1. Before any instances of the REP'd instruction, we
        insert a "micro-branch" uop that implements the check
        for whether there are zero iterations.  Note that this
        branch doesn't actually do anything architecturally
        (see notes in machine.def), but it can still be tagged
        as a recover_inst in case of a micro-branch misprediction.
     2. Each REP iteration is manifested in a separate macro-op
        instantiation, but as far as the stats go, it all still
        counts as a single x86 instruction.
     3. At the end of each REP iteration, we append two extra
        uops to implement the REP control flow.  One decrements
        ECX/CX, and the other implements a branch back to the
        same instruction if more REP's are needed.
                                                           */
  /*********************************************************/

  /* check for insts with repeat count of 0, this can never happen because QEMU sends only executed instructions
     Thus we are ignoring the microops inserted for zero repetition instructions */
  if(Mop->fetch.inst.rep)
    Mop->oracle.zero_rep = false;

  /* For the first in a sequence of REPs (including zero REPs), insert
     a microbranch into the flow-table; uop conversion happens later */
  if(Mop->fetch.inst.rep && (thread->rep_sequence == 0))
  {
    Mop->decode.first_rep = true;

    /* inject an initial micro-branch to test if there are any iterations at all */
    if(Mop->fetch.inst.mode & MODE_ADDR32)
      flowtab[0] = (!!(UP) << 30) | (md_uop_opc(REPFIRST_MICROBR_D) << 16);
    else
      flowtab[0] = (!!(UP) << 30) | (md_uop_opc(REPFIRST_MICROBR_W) << 16);

    Mop->decode.flow_length = 1;
  }
  else
    Mop->decode.first_rep = false;

  if(Mop->fetch.inst.rep && (Mop->decode.opflags & F_MEM) )
  {
	if(Mop->fetch.inst.mem_ops.memops )
		Mop->decode.targetPC= thread->regs.regs_PC ;
	else
		Mop->decode.targetPC= thread->regs.regs_PC + MD_INST_SIZE(Mop->fetch.inst);
  }

  /* If there are more than zero instances of the instruction, then fill in the uops */
  if(!Mop->oracle.zero_rep  ||    (thread->rep_sequence != 0)     ) /* a mispredicted zero rep will have rep_sequence != 0 */
  {
    assert(Mop->decode.opflags & F_UCODE); /* all instructions should have a flow mapping (even for 1-uop codes) */

    /* get instruction flow */
    Mop->decode.flow_length += md_get_flow(Mop, flowtab + Mop->decode.first_rep, bogus); /* have to adjust for the micro-branch we may have already injected */

    /* allocate a uop-array of appropriate length */
    Mop->uop = core->get_uop_array(Mop->decode.flow_length + (Mop->fetch.inst.rep?2:0));

    if(Mop->fetch.inst.rep && (thread->rep_sequence == 0)) /* insert ubranch for 1st iteration of REP */
    {
      struct uop_t * uop = &Mop->uop[0];
      uop->decode.raw_op = flowtab[0];
      MD_SET_UOPCODE(uop->decode.op,&uop->decode.raw_op);
      uop->decode.opflags = MD_OP_FLAGS(uop->decode.op);
      uop->decode.is_ctrl = true;
      uop->Mop = Mop;
    }

    //fprintf(stdout,"\nFlow length %d Bogus %d",Mop->decode.flow_length, *bogus);
    if((!*bogus) && Mop->decode.flow_length > Mop->decode.first_rep) /* if 1st rep, flow_length already was equal to 1 */
    {
      int imm_uops_left = 0;
      struct uop_t * fusion_head = NULL; /* if currently fused, points at head. */
      struct uop_t * prev_uop = NULL;

      for(int i=Mop->decode.first_rep;i<Mop->decode.flow_length;i++)
      {
        struct uop_t * uop = &Mop->uop[i];
        uop->decode.raw_op = flowtab[i];
        if(!imm_uops_left)
        {
          uop->decode.has_imm = UHASIMM;
          MD_SET_UOPCODE(uop->decode.op,&uop->decode.raw_op);
          uop->decode.opflags = MD_OP_FLAGS(uop->decode.op);
          uop->decode.is_load = !!(uop->decode.opflags & F_LOAD);
          if(Mop->decode.opflags & F_STORE)
          {
            uop->decode.is_sta = !!( (uop->decode.op == STAD) |
                                     (uop->decode.op == STAW) |
                                     (uop->decode.op == STA_BGENW) |
                                     (uop->decode.op == STA_BGENWI) |
                                     (uop->decode.op == STA_BGEND) |
                                     (uop->decode.op == STA_BGENDI)
                                   );
            uop->decode.is_std = !!(uop->decode.opflags & F_STORE);
          }
          uop->decode.is_ctrl = !!(uop->decode.opflags & F_CTRL);
          uop->decode.is_nop = uop->decode.op == NOP;

          if(knobs->decode.fusion_mode & FUSION_TYPE(uop))
          {
            assert(prev_uop);
            if(fusion_head == NULL)
            {
              assert(!prev_uop->decode.in_fusion);
              fusion_head = prev_uop;
              fusion_head->decode.in_fusion = true;
              fusion_head->decode.is_fusion_head = true;
              fusion_head->decode.fusion_size = 1;
              fusion_head->decode.fusion_head = fusion_head; /* point at yourself as well */
            }

            uop->decode.in_fusion = true;
            uop->decode.fusion_head = fusion_head;
            fusion_head->decode.fusion_size++;

            prev_uop->decode.fusion_next = uop;
          }
          else
            fusion_head = NULL;

          prev_uop = uop;
        }
        else
        {
          imm_uops_left--; /* don't try to decode the immediates! */
          uop->decode.is_imm = true;
        }

        uop->Mop = Mop; /* back-pointer to parent macro-op */
        if(uop->decode.has_imm)
          imm_uops_left = 2;
      }
    }
    else
    {
      /* If at any point we decode something strange, if we're on the wrong path, we'll just
         abort the instruction.  This will basically bring fetch to a halt until the machine
         gets back on the correct control-flow path. */
      if(spec_mode)
      {
        *bogus = true;
        if(wrote_spec_mem)
          warn("(%s:%d) leaking spec mem",__FILE__,__LINE__);
        return NULL;
      }
      else
        fatal("could not locate UCODE flow");
    }

    /* mark repeat iteration for repeating instructions;
       inject corresponding uops (one ECX update, one micro-branch */
    if(Mop->fetch.inst.rep)
    {
      /* ECX/CX update */
      int idx = Mop->decode.flow_length;
      if(Mop->fetch.inst.mode & MODE_ADDR32)
      {
        flowtab[idx] = ((!!(UP) << 30) | (md_uop_opc(SUBDI) << 16)
                                       | (md_uop_reg(XR_ECX, Mop, bogus) << 12)
                                       | (md_uop_reg(XR_ECX, Mop, bogus) << 8)
                                       | (md_uop_immb(XE_ONE, Mop, bogus)));
        Mop->uop[idx].decode.raw_op = flowtab[idx];
        MD_SET_UOPCODE(Mop->uop[idx].decode.op,&Mop->uop[idx].decode.raw_op);
        Mop->uop[idx].decode.opflags = MD_OP_FLAGS(Mop->uop[idx].decode.op);
        Mop->uop[idx].Mop = Mop;
      }
      else
      {
        flowtab[idx] = ((!!(UP) << 30) | (md_uop_opc(SUBWI) << 16)
                                       | (md_uop_reg(XR_CX, Mop, bogus) << 12)
                                       | (md_uop_reg(XR_CX, Mop, bogus) << 8)
                                       | (md_uop_immb(XE_ONE, Mop, bogus)));
        Mop->uop[idx].decode.raw_op = flowtab[idx];
        MD_SET_UOPCODE(Mop->uop[idx].decode.op,&Mop->uop[idx].decode.raw_op);
        Mop->uop[idx].decode.opflags = MD_OP_FLAGS(Mop->uop[idx].decode.op);
        Mop->uop[idx].Mop = Mop;
      }

      idx++;


      /* micro-jump to test for end of REP */
      if(Mop->fetch.inst.mode & MODE_ADDR32)
      {
        if(Mop->fetch.inst.rep == REP_REPNZ)
          flowtab[idx] = (!!(UP) << 30) | (md_uop_opc(REPNZ_MICROBR_D) << 16);
        else if(Mop->fetch.inst.rep == REP_REPZ)
          flowtab[idx] = (!!(UP) << 30) | (md_uop_opc(REPZ_MICROBR_D) << 16);
        else if(Mop->fetch.inst.rep == REP_REP)
          flowtab[idx] = (!!(UP) << 30) | (md_uop_opc(REP_MICROBR_D) << 16);
        else
        {
          if(spec_mode)
          {
            *bogus = true;
            //if(wrote_spec_mem)
            //  warn("(%s:%d) leaking spec mem",__FILE__,__LINE__);
            return NULL;
          }
          else
            panic("bogus repeat code");
        }
      }
      else/*16 bit address mode*/
      {
        if(Mop->fetch.inst.rep == REP_REPNZ)
          flowtab[idx] = (!!(UP) << 30) | (md_uop_opc(REPNZ_MICROBR_W) << 16);
        else if(Mop->fetch.inst.rep == REP_REPZ)
          flowtab[idx] = (!!(UP) << 30) | (md_uop_opc(REPZ_MICROBR_W) << 16);
        else if(Mop->fetch.inst.rep == REP_REP)
          flowtab[idx] = (!!(UP) << 30) | (md_uop_opc(REP_MICROBR_W) << 16);
        else
        {
          if(spec_mode)
          {
            *bogus = true;
            return NULL;
          }
          else
            panic("bogus repeat code");
        }
      }
      Mop->uop[idx].decode.raw_op = flowtab[idx];
      Mop->uop[idx].decode.is_ctrl = true;
      MD_SET_UOPCODE(Mop->uop[idx].decode.op,&Mop->uop[idx].decode.raw_op);
      Mop->uop[idx].Mop = Mop;

      Mop->decode.flow_length += 2;
    }

    for(int i=0;i<Mop->decode.flow_length;i++)
    {
      Mop->uop[i].flow_index = i;
      Mop->uop[i].decode.Mop_seq = Mop->oracle.seq;
      Mop->uop[i].decode.uop_seq = (Mop->oracle.seq << UOP_SEQ_SHIFT) + i;
    }

  }
  else /* zero-rep inst This branch will never be taken with QEMU */
  {
    Mop->uop = core->get_uop_array(1);
    struct uop_t * uop = &Mop->uop[0];
    uop->decode.raw_op = flowtab[0];
    MD_SET_UOPCODE(uop->decode.op,&uop->decode.raw_op);
    uop->decode.opflags = MD_OP_FLAGS(uop->decode.op);
    uop->decode.is_ctrl = true;
    uop->Mop = Mop;
  }

  if(!Mop->fetch.inst.rep || Mop->decode.first_rep)
    Mop->uop[0].decode.BOM = true;

  while(flow_index < Mop->decode.flow_length)
  {
    /* If we have a microcode op, get the op and inst, this
     * has already been done for non-microcode instructions */
    struct uop_t * uop = &Mop->uop[flow_index];
    uop->decode.FU_class = MD_OP_FUCLASS(uop->decode.op);

    /* get dependency names */
    switch (uop->decode.op)
    {
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,I1,I2,I3,OFLAGS,IFLAGS)\
      case OP: uop->decode.idep_name[0] = I1; uop->decode.odep_name = O1;  \
               uop->decode.idep_name[1] = I2;  \
               uop->decode.idep_name[2] = I3;  \
               uop->decode.iflags = (IFLAGS!=DNA)?IFLAGS:0; \
               uop->decode.oflags = (OFLAGS!=DNA)?OFLAGS:0; \
      break;
#define DEFUOP(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,I1,I2,I3,OFLAGS,IFLAGS)\
      case OP: uop->decode.idep_name[0] = I1; uop->decode.odep_name = O1;  \
               uop->decode.idep_name[1] = I2;  \
               uop->decode.idep_name[2] = I3;  \
               uop->decode.iflags = (IFLAGS!=DNA)?IFLAGS:0; \
               uop->decode.oflags = (OFLAGS!=DNA)?OFLAGS:0; \
      break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)          \
      case OP:                            \
                            panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#include "machine.def"
      default:
        if(spec_mode)
        {
          *bogus = true;
          //if(wrote_spec_mem)
          //  cleanup_aborted_mop(Mop);
          return NULL;
        }
        else
          panic("attempted to execute a bogus opcode");
    }

    /* check for completed flow at top of loop */
    int offset = MD_INC_FLOW;
    Mop->decode.last_uop_index = flow_index;
    flow_index += offset;
  }

  flow_index = 0;
  while(flow_index < Mop->decode.flow_length)
  {
    /* If we have a microcode op, get the op and inst, this
     * has already been done for non-microcode instructions */
    struct uop_t * uop = &Mop->uop[flow_index];

    assert((!(uop->decode.opflags & F_STORE)) || uop->decode.is_std);
    assert((!(uop->decode.opflags & F_LOAD)) || uop->decode.is_load);

    /* are all operands valid? */
    if( (uop->decode.idep_name[0] >= MD_TOTAL_REGS) ||
        (uop->decode.idep_name[1] >= MD_TOTAL_REGS) ||
        (uop->decode.idep_name[2] >= MD_TOTAL_REGS) ||
        (uop->decode.odep_name >= MD_TOTAL_REGS) )
    {
      if(spec_mode)
      {
        *bogus = true;
        //if(wrote_spec_mem)
        //  cleanup_aborted_mop(Mop);
        return NULL;
      }
      else
        panic("decoded an instruction with an invalid register specifier (%d=%d,%d,%d max should be %d)",uop->decode.odep_name, uop->decode.idep_name[0], uop->decode.idep_name[1], uop->decode.idep_name[2],MD_TOTAL_REGS);
    }

    /* break addr dependency to allow STA/STD to execute independently */
    if(uop->decode.is_std)
      uop->decode.idep_name[MD_STA_OP_INDEX] = DNA;



//    /* read input dep values WE CANNOT READ THESE VALUES BECAUSE ZESTO DOES NOT KEEP ANY STATE ANYMORE */
//    for(int i=0;i<MAX_IDEPS;i++)
//    {
//      if((uop->decode.idep_name[i] != DNA) && (uop->decode.idep_name[i] != MD_REG_ZERO))
//      {
//        if(REG_IS_GPR(uop->decode.idep_name[i]))
//          uop->oracle.ivalue[i].dw = thread->regs.regs_R.dw[_DGPR(uop->decode.idep_name[i])];
//        else if(REG_IS_FPR(uop->decode.idep_name[i]))
//          uop->oracle.ivalue[i].e = thread->regs.regs_F.e[_DFPR(uop->decode.idep_name[i])];
//        else if(REG_IS_SEG(uop->decode.idep_name[i]))
//          uop->oracle.ivalue[i].w = thread->regs.regs_S.w[_DSEG(uop->decode.idep_name[i])];
//      }
//    }
//    /* copy previous values of odep for possible recovery */
//    if((uop->decode.odep_name != DNA) && (uop->decode.odep_name != MD_REG_ZERO))
//    {
//      if(REG_IS_GPR(uop->decode.odep_name))
//        uop->oracle.prev_ovalue.dw = thread->regs.regs_R.dw[_DGPR(uop->decode.odep_name)];
//      else if(REG_IS_FPR(uop->decode.odep_name))
//        uop->oracle.prev_ovalue.e = thread->regs.regs_F.e[_DFPR(uop->decode.odep_name)];
//      else if(REG_IS_SEG(uop->decode.odep_name))
//        uop->oracle.prev_ovalue.w = thread->regs.regs_S.w[_DSEG(uop->decode.odep_name)];
//    }
//    uop->oracle.ictrl = thread->regs.regs_C;

    /* execute the instruction This is not needed anymore beacuse QEMU is executing the instructions 
       Code here is also responsible for writing the PC and NPC values as well as the virtual addresses of memory ops
       So we need to add functionality to insert these values
       NPC values get written in the beginning with the MD_FETCH_NEXT_PC() call
       We need to write the values of addresses for memory ops

    switch (uop->decode.op)
    {
#define DEFINST(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,I1,I2,I3,OFLAGS,IFLAGS)\
      case OP: SYMCAT(OP,_IMPL_FUNC)(thread,core,Mop,uop,bogus);               \
      break;
#define DEFUOP(OP,MSK,NAME,OPFORM,RES,FLAGS,O1,I1,I2,I3,OFLAGS,IFLAGS)\
      case OP: SYMCAT(OP,_IMPL_FUNC)(thread,core,Mop,uop,bogus);               \
      break;
#define DEFLINK(OP,MSK,NAME,MASK,SHIFT)          \
      case OP:                            \
                            panic("attempted to execute a linking opcode");
#define CONNECT(OP)
#include "machine.def"
#undef DEFINST
#undef DEFUOP
#undef CONNECT
      default:
        panic("attempted to execute a bogus opcode");
    }*/



//    if(uop->oracle.spec_mem[0])
//      wrote_spec_mem = true;
//
//    /* maintain $r0 semantics */
//    thread->regs.regs_R.dw[MD_REG_ZERO] = 0;
//
//    /* copy output value */
//    if((uop->decode.odep_name != DNA) && (uop->decode.odep_name != MD_REG_ZERO))
//    {
//      if(REG_IS_GPR(uop->decode.odep_name))
//        uop->oracle.ovalue.dw = thread->regs.regs_R.dw[_DGPR(uop->decode.odep_name)];
//      else if(REG_IS_FPR(uop->decode.odep_name))
//        uop->oracle.ovalue.e = thread->regs.regs_F.e[_DFPR(uop->decode.odep_name)];
//      else if(REG_IS_SEG(uop->decode.odep_name))
//        uop->oracle.ovalue.w = thread->regs.regs_S.w[_DSEG(uop->decode.odep_name)];
//    }
//    uop->oracle.octrl = thread->regs.regs_C;




    /*Updating the address of the load micro-op with the one given by QEMU*/
    if(uop->decode.is_load)
    {
	if(Mop->fetch.inst.mem_ops.ld_dequeued[0]==false)	//if this is the firts load mem-op of an instruction
	{
		uop->oracle.phys_addr= Mop->fetch.inst.mem_ops.mem_paddr_ld[0];
        	uop->oracle.virt_addr= Mop->fetch.inst.mem_ops.mem_vaddr_ld[0];
        	uop->decode.mem_size=Mop->fetch.inst.mem_ops.ld_size[0];
		Mop->fetch.inst.mem_ops.ld_dequeued[0]=true;
	}else	//if an instruction has two load micro-ops
	{
		uop->oracle.phys_addr= Mop->fetch.inst.mem_ops.mem_paddr_ld[1];
        	uop->oracle.virt_addr= Mop->fetch.inst.mem_ops.mem_vaddr_ld[1];
        	uop->decode.mem_size=Mop->fetch.inst.mem_ops.ld_size[1];
		Mop->fetch.inst.mem_ops.ld_dequeued[1]=true;
	}
        if( (uop->decode.op == LDPCWW) || (uop->decode.op == LDPCDW) || (uop->decode.op == LDPCWD) || (uop->decode.op == LDPCDD) )
        {
                ///////fprintf(stdout,"Detected a load PC instruction");	//I need the address of the stack segment
                uop->oracle.phys_addr=thread->regs.regs_NPC;
                uop->oracle.virt_addr=thread->regs.regs_NPC;
                uop->decode.mem_size=4;
        }


	//if(Mop->fetch.inst.rep && (!(uop->oracle.virt_addr)) )	//Case where the iteration is not executed so QEMU gives you the instruction but not the memory op
	if((!(uop->oracle.virt_addr)) )	//Case where the iteration is not executed so QEMU gives you the instruction but not the memory op
	{
			uop->oracle.phys_addr= 0xfffffff0;
        		uop->oracle.virt_addr= 0xfffffff0;
        		uop->decode.mem_size= 4;
	}	
        assert(uop->decode.mem_size!=0);
    }
    else if(uop->decode.is_std)
    {
	int prev_uop_index = flow_index-1;
        /* virt_addr and mem_size set by execution of STD uop, copy back to STA */
	assert(prev_uop_index >= 0);
	while(Mop->uop[prev_uop_index].decode.is_imm)
	{
		prev_uop_index --;
		assert(prev_uop_index >= 0);
	}
	assert(Mop->uop[prev_uop_index].decode.is_sta);

	if(Mop->fetch.inst.mem_ops.str_dequeued[0]==false)	//if this is the firts load mem-op of an instruction
	{
		//fprintf(stdout,"\n[%lld]Oracle This is the first store sent by QEMu ", sim_cycle);
		uop->oracle.phys_addr= Mop->fetch.inst.mem_ops.mem_paddr_str[0];
        	uop->oracle.virt_addr= Mop->fetch.inst.mem_ops.mem_vaddr_str[0];
        	uop->decode.mem_size=Mop->fetch.inst.mem_ops.str_size[0];
		Mop->fetch.inst.mem_ops.str_dequeued[0]=true;
	}else	//if an instruction has two store micro-ops
	{
		//fprintf(stdout,"\n[%lld]Oracle This is the second store sent by QEMu ", sim_cycle);
		uop->oracle.phys_addr= Mop->fetch.inst.mem_ops.mem_paddr_str[1];
        	uop->oracle.virt_addr= Mop->fetch.inst.mem_ops.mem_vaddr_str[1];
        	uop->decode.mem_size=Mop->fetch.inst.mem_ops.str_size[1];
		Mop->fetch.inst.mem_ops.str_dequeued[1]=true;
	}
	
	//if(Mop->fetch.inst.rep && (!(uop->oracle.virt_addr)) )	//Case where the iteration is not executed so QEMU gives you the instruction but not the memory op
	if((!(uop->oracle.virt_addr)) )	//Case where the iteration is not executed so QEMU gives you the instruction but not the memory op
	{	
			uop->oracle.phys_addr= 0xbeffeffe;
        		uop->oracle.virt_addr= 0xbeffeffe;
        		uop->decode.mem_size= 4;
	}	

	if( (uop->decode.op == STPCD) || (uop->decode.op == STPCW) )
	{
		//fprintf(stdout,"Detected a store PC instruction");	//I need the address of the stack segment
		uop->oracle.phys_addr=Mop->fetch.inst.paddr;
        	uop->oracle.virt_addr=Mop->fetch.inst.vaddr;
		uop->decode.mem_size=4;
	}
	Mop->uop[prev_uop_index].oracle.virt_addr = uop->oracle.virt_addr;
	Mop->uop[prev_uop_index].oracle.phys_addr = uop->oracle.phys_addr;
	Mop->uop[prev_uop_index].decode.mem_size = uop->decode.mem_size;
	if(!spec_mode)
	{
		assert(uop->oracle.virt_addr && uop->decode.mem_size);
		///////if(!uop->oracle.virt_addr)
			/////fprintf(stdout,"\n Did not get memop");
	}
    }

    if (uop->oracle.fault != md_fault_none)
    {
      if(spec_mode)
      {
        *bogus = true;
        //if(wrote_spec_mem)
        //  cleanup_aborted_mop(Mop);
        return NULL;
      }
      else
        fatal("fault (%d) detected @ 0x%08p", uop->oracle.fault, thread->regs.regs_PC);
    }

    /* Update stats */
    if((!uop->decode.in_fusion) || uop->decode.is_fusion_head)
    {
      Mop->stat.num_uops++;
      ZESTO_STAT(core->stat.oracle_total_uops++;)
    }
    Mop->stat.num_eff_uops++;
    ZESTO_STAT(core->stat.oracle_total_eff_uops++;)

    if (uop->decode.opflags & F_CTRL) {
      Mop->stat.num_branches++;
      ZESTO_STAT(core->stat.oracle_total_branches++;)
      if(!spec_mode)
        ZESTO_STAT(core->stat.oracle_num_branches++;)
    }
    if (uop->decode.opflags & F_MEM) 
    {
      Mop->stat.num_refs++;
      ZESTO_STAT(core->stat.oracle_total_refs++;)
      if(!spec_mode)
        ZESTO_STAT(core->stat.oracle_num_refs++;)
      if (uop->decode.opflags & F_LOAD) {
        Mop->stat.num_loads++;
        ZESTO_STAT(core->stat.oracle_total_loads++;)
        if(!spec_mode)
          ZESTO_STAT(core->stat.oracle_num_loads++;)
      }
    }

    flow_index += MD_INC_FLOW;
  }

  /* Do FP-stack adjustments if necessary */
  if(Mop->decode.fpstack_op != fpstk_nop)
    FPSTACK_IMPL(Mop->decode.fpstack_op);

  /* update register mappings, inter-uop dependencies */
  /* NOTE: this occurs in its own loop because the above loop
     may terminate prematurely if a bogus fetch condition is
     encountered. */
  flow_index = 0;
  while(flow_index < Mop->decode.flow_length)
  {
    struct uop_t * uop = &Mop->uop[flow_index];

    install_dependencies(uop); /* update back/fwd pointers between uop and parent */
    install_mapping(uop); /* add self to register mapping (oracle's rename table) */
    flow_index += MD_INC_FLOW;
  }

  /* if PC==NPC, means we're still REP'ing */
  if(thread->regs.regs_PC == thread->regs.regs_NPC)
  {
    ///////fprintf(stdout,"\n[%lld]Oracle.assert  Thread->PC= 0x%llx Thread->NPC= 0x%llx spec_mode %d Mop->fetch.inst.rep %d ", sim_cycle, thread->regs.regs_PC, thread->regs.regs_NPC, spec_mode, Mop->fetch.inst.rep);
    assert(Mop->oracle.spec_mode || Mop->fetch.inst.rep);    
    thread->rep_sequence ++;
  }
  else
  {
    Mop->uop[Mop->decode.last_uop_index].decode.EOM = true; /* Mark EOM if appropriate */
    thread->rep_sequence = 0;
  }

  if(Mop->uop[Mop->decode.last_uop_index].decode.EOM) /* count insts based on EOM markers */
  {
    if(!Mop->oracle.spec_mode)
      thread->stat.num_insn++;
    ZESTO_STAT(core->stat.oracle_total_insn++;)
  }

//  /* maintain $r0 semantics */
//  thread->regs.regs_R.dw[MD_REG_ZERO] = 0;

  Mop->oracle.NextPC = thread->regs.regs_NPC;

  /* commit this inst to the MopQ */
  MopQ_tail = modinc(MopQ_tail,MopQ_size); //(MopQ_tail + 1) % MopQ_size;
  MopQ_num ++;


  if(Mop->decode.is_trap)
  {    
    current_Mop = Mop;
    /* make sure pipeline has drained */
    if(MopQ_num > 0)
    {
      //////fprintf(stdout,"\n[%lld]Oracle.end()  Returning null so that pipeline drains TRAP Thread->PC= 0x%llx Thread->NPC= 0x%llx ", sim_cycle, thread->regs.regs_PC, thread->regs.regs_NPC);
      return NULL;
    }
    else
      assert(!spec_mode);
  }

  current_Mop = Mop;

  assert(Mop->decode.flow_length); /* there had better be at least one uop */

#ifdef ZTRACE
  ztrace_print(Mop);
#endif

  //////fprintf(stdout,"\n[%lld]Oracle.end()  Thread->PC= 0x%llx Thread->NPC= 0x%llx ", sim_cycle, thread->regs.regs_PC, thread->regs.regs_NPC);
  return Mop;
}

/* After calling oracle-exec, you need to first call this function
   to tell the oracle that you are in fact done with the previous
   Mop.  This may occur due to interruptions half-way through fetch
   processing (e.g., instruction spilt across cache lines). */
void core_oracle_t::consume(const struct Mop_t * const Mop)
{
  assert(Mop == current_Mop);
  //if(Mop->decode.is_trap)
	  //fprintf(stdout,"\n[%lld]Oracle : Consuming Macro-op",sim_cycle);
  current_Mop = NULL;
}

void core_oracle_t::commit_uop(struct uop_t * const uop)
{
  /* clean up idep/odep ptrs */
  struct odep_t * odep = uop->exec.odep_uop;
  while(odep)
  {
    struct odep_t * next = odep->next;
    zesto_assert(odep->uop,(void)0);
    odep->uop->exec.idep_uop[odep->op_num] = NULL;
    core->return_odep_link(odep);
    odep = next;
  }
  uop->exec.odep_uop = NULL;

  /* remove self from register mapping */
  commit_mapping(uop);

  /* clear oracle's back/fwd pointers between uop and children */
  commit_dependencies(uop);


  /* commit memory writes to real memory system NOT NEEDED ANYMORE with QEMU
  for(int j=0;j<12;j++)
  {
    if(!uop->oracle.spec_mem[j])
      break;
    MEM_WRITE_BYTE_NON_SPEC(core->current_thread->mem, uop->oracle.virt_addr+j, uop->oracle.spec_mem[j]->val);
    core->oracle->commit_write_byte(uop->oracle.spec_mem[j]);
    uop->oracle.spec_mem[j] = NULL;
  }*/
}

/* This is called by the backend to inform the oracle that the pipeline has
   completed processing (committed) the entire Mop. */
void
core_oracle_t::commit(const struct Mop_t * const commit_Mop)
{
  struct Mop_t * Mop = &MopQ[MopQ_head];

  if(MopQ_num <= 0) /* nothing to commit */
    fatal("attempt to commit when MopQ is empty");

  assert(Mop == commit_Mop);

  /* TODO: add checker support */

  assert(Mop->oracle.spec_mode == 0); /* can't commit wrong path insts! */

  Mop->valid = false;

  MopQ_head = modinc(MopQ_head,MopQ_size); //(MopQ_head + 1) % MopQ_size;
  //fprintf(stdout,"\n[%lld]Oracle.commit() Decrementing MopQ_num",sim_cycle);
  MopQ_num--;
  assert(MopQ_num >= 0);
}

/* Undo the effects of the single Mop.  This function only affects the ISA-level
   state.  Bookkeeping for the MopQ and other core-level structures has to be
   dealt with separately. */
void
core_oracle_t::undo(struct Mop_t * const Mop)
{
  struct thread_t * thread = core->current_thread;
  /* walk uop list backwards, undoing each operation's effects */
  for(int i=Mop->decode.flow_length-1;i>=0;i--)
  {
    struct uop_t * uop = &Mop->uop[i];
    uop->exec.action_id = core->new_action_id(); /* squashes any in-flight loads/stores */

    /* collect stats */
    if(uop->decode.EOM)
    {
      if(!Mop->oracle.spec_mode)
      {
        thread->stat.num_insn --; /* one less oracle instruction executed */
      }
      ZESTO_STAT(core->stat.oracle_inst_undo ++;)
    }

    if(uop->decode.is_imm)
      continue;
    else
    { 
      /* one less oracle uop executed */
      if(!uop->decode.in_fusion || uop->decode.is_fusion_head)
      {
        ZESTO_STAT(core->stat.oracle_uop_undo ++;)
      }
      ZESTO_STAT(core->stat.oracle_eff_uop_undo ++;)
    }

    /* undo register write NOT NEEDED WITH QEMU */
    //if((uop->decode.odep_name != DNA) && (uop->decode.odep_name != MD_REG_ZERO))
    //{
    //  if(REG_IS_GPR(uop->decode.odep_name))
    //    thread->regs.regs_R.dw[_DGPR(uop->decode.odep_name)] = uop->oracle.prev_ovalue.dw;
    //  else if(REG_IS_FPR(uop->decode.odep_name))
    //    thread->regs.regs_F.e[_DFPR(uop->decode.odep_name)] = uop->oracle.prev_ovalue.e;
    //  else if(REG_IS_SEG(uop->decode.odep_name))
    //    thread->regs.regs_S.w[_DSEG(uop->decode.odep_name)] = uop->oracle.prev_ovalue.w;
    //}

    /* undo ctrl register updates */
    //thread->regs.regs_C = uop->oracle.ictrl;

    /* undo memory changes NOT NEEDED ANYMORE with QEMU
    if(uop->decode.is_std)
    {
      for(int j=0;j<12;j++)
      {
        if(uop->oracle.spec_mem[j] == NULL)
          break;
        squash_write_byte(uop->oracle.spec_mem[j]);
        uop->oracle.spec_mem[j] = NULL;
      }
    }*/

    /* remove self from register mapping */
    undo_mapping(uop);

    /* clear back/fwd pointers between uop and parent */
    undo_dependencies(uop);
  }

  thread->rep_sequence = Mop->decode.rep_seq;

  Mop->fetch.jeclear_action_id = core->new_action_id();
}

/* recover the oracle's state right up to Mop (but don't undo Mop) */
void
core_oracle_t::recover(const struct Mop_t * const Mop)
{
  int idx = moddec(MopQ_tail,MopQ_size); //(MopQ_tail-1+MopQ_size) % MopQ_size;
  //fprintf(stdout,"\n\n[%lld]Oracle.recover_start()  Thread->PC= 0x%llx Thread->NPC= 0x%llx ", sim_cycle, core->current_thread->regs.regs_PC, core->current_thread->regs.regs_NPC);
  while(Mop != &MopQ[idx])
  {
    if(idx == MopQ_head)
      fatal("ran out of Mop's before finding requested MopQ recovery point");
    undo(&MopQ[idx]);
    MopQ[idx].valid = false;
    if(MopQ[idx].fetch.bpred_update)
    {
      core->fetch->bpred->flush(MopQ[idx].fetch.bpred_update);
      core->fetch->bpred->return_state_cache(MopQ[idx].fetch.bpred_update);
      MopQ[idx].fetch.bpred_update = NULL;
    }

    MopQ_num --;
    MopQ_tail = idx;
    idx = moddec(idx,MopQ_size); //(idx-1+MopQ_size) % MopQ_size;
  }
  /* reset PC */
  if(Mop->fetch.branch_mispred)	//If this is a recover due to branch misprediction
  {
	core->current_thread->regs.regs_PC = Mop->fetch.PC;
  	core->current_thread->regs.regs_NPC = Mop->oracle.NextPC;
  }
  else	//jeclear due to store misordering TODO: reexecute from previous Mop
  {
	//core->current_thread->regs.regs_PC = Mop->fetch.PC;
  	//core->current_thread->regs.regs_NPC = Mop->oracle.NextPC;
  }
  spec_mode = Mop->oracle.spec_mode;
  current_Mop = NULL;
  //fprintf(stdout,"\n[%lld]Oracle.recover_end()  Thread->PC= 0x%llx Thread->NPC= 0x%llx ", sim_cycle, core->current_thread->regs.regs_PC, core->current_thread->regs.regs_NPC);
}

/* flush everything after Mop */
void core_oracle_t::pipe_recover(struct Mop_t * const Mop, const md_addr_t New_PC)
{
  struct core_knobs_t * knobs = core->knobs;
  if(knobs->fetch.jeclear_delay)
    core->fetch->jeclear_enqueue(Mop,New_PC);
  else
  {
    if(Mop->fetch.bpred_update)
      core->fetch->bpred->recover(Mop->fetch.bpred_update,(New_PC != (Mop->fetch.PC + Mop->fetch.inst.len)));
    /*core->oracle->*/recover(Mop);
    core->commit->recover(Mop);
    core->exec->recover(Mop);
    core->alloc->recover(Mop);
    core->decode->recover(Mop);
    core->fetch->recover(New_PC);
  }
}

/* flush everything including the Mop; restart fetching with this
   Mop again. */
void core_oracle_t::pipe_flush(struct Mop_t * const Mop)
{
  const int prev_Mop_index = moddec(Mop-MopQ,MopQ_size); //((Mop - MopQ) - 1 + MopQ_size) % MopQ_size;
  zesto_assert(MopQ_num > 1 && (Mop != & MopQ[MopQ_head]),(void)0); /* I don't think there are any uop flows that can cause intra-Mop violations */
  struct Mop_t * const prev_Mop = &MopQ[prev_Mop_index];
  /* CALLs/RETNs where the PC is loaded from memory, and that load is involved
     in a store-load ordering violation, can cause a branch (target) mispredict
     recovery from being properly taken care of (partly due to the fact that
     our uop flow does not act on the recovery until the lasp uop of the flow,
     which is typically *not* the load).  So to get around that, we just patch
     up the predicted NPC. */
  prev_Mop->fetch.pred_NPC = prev_Mop->oracle.NextPC;

  //pipe_recover(prev_Mop, prev_Mop->fetch.pred_NPC);
  //fprintf(stdout,"\n[%lld]Oracle.pipe_flush()  Now start from = 0x%llx ", sim_cycle, Mop->core->current_thread->regs.regs_NPC);
  md_addr_t current_nextPC;
#ifdef USE_PIN_TRACES
  current_nextPC=store_nextPC[Mop->core->id];
#else
  md_fetch_next_pc(&current_nextPC,Mop->core->id);
#endif
  if(!spec_mode)
	  pipe_recover(Mop, Mop->core->current_thread->regs.regs_NPC);
  else //If the mode is speculative the thread PC and nextPC are the same so that no-ops do not run off into the address stream
	  pipe_recover(Mop, current_nextPC);
  //TODO: All Mops must be refetched and executed following a memory order violation, For now we just go ahead with the next instruction stream assuming this does not happen too often 

}

/* like oracle recover, but empties out the entire pipeline, wrong-path or not */
void
core_oracle_t::complete_flush(void)
{
  int idx = moddec(MopQ_tail,MopQ_size); //(MopQ_tail-1+MopQ_size) % MopQ_size;
  md_addr_t arch_PC = 0x00000000LL;
  while(MopQ_num)
  {
    arch_PC = MopQ[idx].fetch.PC;
    assert(MopQ[idx].valid);
    undo(&MopQ[idx]);
    MopQ[idx].valid = false;
    if(MopQ[idx].fetch.bpred_update)
    {
      core->fetch->bpred->flush(MopQ[idx].fetch.bpred_update);
      core->fetch->bpred->return_state_cache(MopQ[idx].fetch.bpred_update);
      MopQ[idx].fetch.bpred_update = NULL;
    }

    MopQ_num --;
    MopQ_tail = idx;
    idx = moddec(idx,MopQ_size); //(idx-1+MopQ_size) % MopQ_size;
  }
  /* reset PC */
  core->current_thread->regs.regs_PC = arch_PC;
  core->current_thread->regs.regs_NPC = arch_PC;
  assert(MopQ_head == MopQ_tail);

  spec_mode = false;
  current_Mop = NULL;
}

/* This *completely* resets the oracle to restart execution all over
   again.  This reloads the initial simulated program state from the
   first checkpoint in the EIO file. */
void
core_oracle_t::reset_execution(void)
{
  complete_flush();
  core->commit->recover();
  core->exec->recover();
  core->alloc->recover();
  core->decode->recover();
  core->fetch->recover(0);
  // NOT NEEDED WITH QEMU wipe_memory(core->current_thread->mem);

  // NOT NEEDED WITH QEMU ld_reload_prog(core->current_thread);
  core->fetch->PC = core->current_thread->regs.regs_PC;
  core->fetch->bogus = false;
  core->stat.oracle_resets++;
}

/* Called when a uop commits; removes uop from list of producers. */
void core_oracle_t::commit_mapping(const struct uop_t * const uop)
{
  struct map_node_t * p;

  /* regular register output */
  if((uop->decode.odep_name != DNA) && (uop->decode.odep_name != DGPR(MD_REG_ZERO)))
  {
    p = dep_map.head[uop->decode.odep_name];

    /* if you're committing this, it better be in the mapping */
    assert(p);
    /* if you're committing this, it better be the oldest one */
    assert(uop == p->uop);

    /* remove from head */
    dep_map.head[uop->decode.odep_name] = p->next;
    if(p->next)
      p->next->prev = NULL;

    /* if only mapping, remove from tail as well */
    if(dep_map.tail[uop->decode.odep_name] == p)
      dep_map.tail[uop->decode.odep_name] = NULL;

    return_map_node(p);
  }

  /* flags output */
  if(uop->decode.oflags && uop->decode.oflags != DNA)
  {
    p = dep_map.head[DCREG(MD_REG_AFLAGS)];

    /* if you're committing this, it better be in the mapping */
    assert(p);
    /* if you're committing this, it better be the oldest one */
    assert(uop == p->uop);

    /* remove from head */
    dep_map.head[DCREG(MD_REG_AFLAGS)] = p->next;
    if(p->next)
      p->next->prev = NULL;

    /* if only mapping, remove from tail as well */
    if(dep_map.tail[DCREG(MD_REG_AFLAGS)] == p)
      dep_map.tail[DCREG(MD_REG_AFLAGS)] = NULL;

    return_map_node(p);
  }
}

/* Cleans up dependency pointers between uop and CHILDREN. */
void core_oracle_t::commit_dependencies(struct uop_t * const uop)
{
  struct odep_t * odep = uop->oracle.odep_uop;

  while(odep)
  {
    struct odep_t * next = odep->next;

    /* remove from child's idep vector */
    assert(odep->uop->oracle.idep_uop[odep->op_num] == uop);
    odep->uop->oracle.idep_uop[odep->op_num] = NULL;

    odep->uop = NULL;
    odep->op_num = -1;
    core->return_odep_link(odep);

    odep = next;
  }
  uop->oracle.odep_uop = NULL;
}

/* remove the entry from the table */
void core_oracle_t::commit_write_byte(struct spec_byte_t * const p)
{
  const int index = p->addr & MEM_HASH_MASK;
  assert(spec_mem_map.hash[index].head == p);

  if(p->next)
    p->next->prev = NULL;
  spec_mem_map.hash[index].head = p->next;
  if(p->next == NULL)
  {
    spec_mem_map.hash[index].tail = NULL;
    assert(spec_mem_map.hash[index].head == NULL);
  }

  return_spec_mem_node(p);
}


/**************************************/
/* PROTECTED METHODS/MEMBER-FUNCTIONS */
/**************************************/


/* ORACLE DEPENDENCY MAP:
   The oracle keeps track of the equivalent of a register renaming
   table.  For each uop, the following functions add the uop to the
   output dependency list (odep list) of each of its input
   dependencies that the oracle still knows about (i.e., each parent
   uop that has not yet committed from the oracle), and the adds
   this uop into the mapping table as the most recent producer of
   the output register.  These pointers are used to implement the
   edges of the dataflow dependency graph which is used instead of
   explicit register renaming et al. to make the simulator a little
   bit faster. */

/* adds uop as most recent producer of its output(s) */
void core_oracle_t::install_mapping(struct uop_t * const uop)
{
  struct map_node_t * p = NULL;

  /* regular register output */
  if((uop->decode.odep_name != DNA) && (uop->decode.odep_name != DGPR(MD_REG_ZERO)))
  {
    p = get_map_node();
    p->uop = uop;

    /* install in tail (newest) position */
    
    if(dep_map.head[uop->decode.odep_name] == NULL) /* if no in-flight producers */
    {
      dep_map.head[uop->decode.odep_name] = p;
      assert(dep_map.tail[uop->decode.odep_name] == NULL);
      dep_map.tail[uop->decode.odep_name] = p;
      /* p's next/prev pointers are already cleared */
    }
    else /* at least one other producer still in-flight */
    {
      p->prev = dep_map.tail[uop->decode.odep_name];
      assert(p->prev);
      p->prev->next = p;
      dep_map.tail[uop->decode.odep_name] = p;
    }
  }

  /* flags output - same thing as above */
  if(uop->decode.oflags && uop->decode.oflags != DNA)
  {
    p = get_map_node();
    p->uop = uop;
    if(dep_map.head[DCREG(MD_REG_AFLAGS)] == NULL)
    {
      dep_map.head[DCREG(MD_REG_AFLAGS)] = p;
      assert(dep_map.tail[DCREG(MD_REG_AFLAGS)] == NULL);
      dep_map.tail[DCREG(MD_REG_AFLAGS)] = p;
      /* p's next/prev pointers are already cleared */
    }
    else
    {
      p->prev = dep_map.tail[DCREG(MD_REG_AFLAGS)];
      assert(p->prev);
      p->prev->next = p;
      dep_map.tail[DCREG(MD_REG_AFLAGS)] = p;
    }
  }
}

/* Called when a uop is undone (during pipe flush); removes uop from
   list of producers.  Difference is commit removes from the head,
   undo removes from the tail. */
void core_oracle_t::undo_mapping(const struct uop_t * const uop)
{
  struct map_node_t * p;
  
  /* regular register output */
  if(uop->decode.odep_name != DNA && uop->decode.odep_name != MD_REG_ZERO)
  {
    p = dep_map.tail[uop->decode.odep_name];

    /* XXX might not be in mapping if oldest inst? */
    if(!p)
      goto flags;

    /* if you're undoing this, it better be the youngest one */
    assert(uop == p->uop);

    /* remove from tail */
    dep_map.tail[uop->decode.odep_name] = p->prev;
    if(p->prev)
      p->prev->next = NULL;

    /* if only mapping, remove from head as well */
    if(dep_map.head[uop->decode.odep_name] == p)
      dep_map.head[uop->decode.odep_name] = NULL;

    return_map_node(p);
  }

flags:

  /* flags output */
  if(uop->decode.oflags && uop->decode.oflags != DNA)
  {
    p = dep_map.tail[DCREG(MD_REG_AFLAGS)];

    /* XXX might not be in mapping if oldest inst? */
    if(!p)
      return;
    /* if you're undoing this, it better be the oldest one */
    assert(uop == p->uop);

    /* remove from tail */
    dep_map.tail[DCREG(MD_REG_AFLAGS)] = p->prev;
    if(p->prev)
      p->prev->next = NULL;

    /* if only mapping, remove from head as well */
    if(dep_map.head[DCREG(MD_REG_AFLAGS)] == p)
      dep_map.head[DCREG(MD_REG_AFLAGS)] = NULL;

    return_map_node(p);
  }
}

/* Mapping table node alloc/dealloc */
struct core_oracle_t::map_node_t * core_oracle_t::get_map_node(void)
{
  struct map_node_t * p = NULL;
  if(map_free_pool)
  {
    p = map_free_pool;
    map_free_pool = p->next;
  }
  else
  {
    p = (struct map_node_t*) calloc(1,sizeof(*p));
    if(!p)
      fatal("couldn't calloc a dependency node");
  }
  assert(p);
  p->next = NULL;
  /* other fields were cleared when p was "return"ed (see below) */
  map_free_pool_debt++;
  return p;
}

void core_oracle_t::return_map_node(struct map_node_t * const p)
{
  p->next = map_free_pool;
  map_free_pool = p;
  p->uop = NULL;
  p->prev = NULL;
  map_free_pool_debt--;
  /* p->next used for free list, will be cleared on "get" */
}


/* Installs pointers back to the uop's parents, and installs odep
   pointers from the parents forward to this uop.  (Build uop's list
   of parents, add uop to each parent's list of children.) */
void core_oracle_t::install_dependencies(struct uop_t * const uop)
{
  for(int i=0;i<MAX_IDEPS;i++)
  {
    /* get pointers to parent uops */
    int reg_name = uop->decode.idep_name[i];
    if(reg_name != DNA && reg_name != MD_REG_ZERO)
    {
      /* my parent is the most recent producer (dep_map.tail) of my operand */
      if(dep_map.tail[reg_name])
      {
        uop->oracle.idep_uop[i] = dep_map.tail[reg_name]->uop;
        assert(uop->oracle.idep_uop[i]->Mop->oracle.seq <= uop->Mop->oracle.seq);
      }

      /* install pointers from parent to this uop (add to parent's odep list) */
      if(uop->oracle.idep_uop[i])
      {
        struct odep_t * odep = core->get_odep_link();
        odep->next = uop->oracle.idep_uop[i]->oracle.odep_uop;
        uop->oracle.idep_uop[i]->oracle.odep_uop = odep;
        odep->uop = uop;
        odep->aflags = false;
        odep->op_num = i;
      }
      /* else parent already committed */
    }
  }

  if(uop->decode.iflags && uop->decode.iflags != DNA)
  {
    /* get pointers to parent uop */
    int reg_name = DCREG(MD_REG_AFLAGS);
    int idx = MAX_IDEPS-1; /* always install AFLAGS idep in last entry */
    assert(uop->decode.idep_name[idx] == DNA);
    uop->decode.idep_name[idx] = DCREG(MD_REG_AFLAGS);

    /* my parent is the most recent producer (dep_map.tail) of my operand */
    if(dep_map.tail[reg_name])
      uop->oracle.idep_uop[idx] = dep_map.tail[reg_name]->uop;

    /* install pointers from parent to this uop (add to parent's odep list) */
    if(uop->oracle.idep_uop[idx])
    {
      struct odep_t * odep = core->get_odep_link();
      odep->next = uop->oracle.idep_uop[idx]->oracle.odep_uop;
      uop->oracle.idep_uop[idx]->oracle.odep_uop = odep;
      odep->aflags = true;
      odep->uop = uop;
      odep->op_num = idx;
    }
    /* else parent already committed */
  }
}

/* Cleans up dependency pointers between uop and PARENTS. */
void core_oracle_t::undo_dependencies(struct uop_t * const uop)
{
  for(int i=0;i<MAX_IDEPS;i++)
  {
    struct uop_t * parent = uop->oracle.idep_uop[i];
    if(parent)
    {
      struct odep_t * odep = NULL;
      struct odep_t * odep_prev = NULL;

      odep = parent->oracle.odep_uop;
      while(odep)
      {
        if((odep->uop == uop) && (odep->op_num == i))
        {
          if(odep_prev)
            odep_prev->next = odep->next;
          else
            parent->oracle.odep_uop = odep->next;
          core->return_odep_link(odep);
          break;
        }
        odep_prev = odep;
        odep = odep->next;
      }
      assert(odep); /* if I point back at a parent, the parent should point fwd to me. */
    }
    uop->oracle.idep_uop[i] = NULL; /* remove my back pointer */
  }

  /* don't need to special-case AFLAGS since those were added to the idep list */
}


/* Code for pre-commit memory handling */
/* Stores get inserted into the hash table, and loads check the
   hash table before retrieving a value from memory.  This is very
   similar to the speculative memory handling approach used in MASE.
   Stores do not update the simulated memory (mem_t) until they
   actually commit.  All speculative updates get stored in a
   separate table, and loads have to check this first before
   retrieving values from main memory.  Note that almost everything
   here is performed on a byte-by-byte basis due to the lack of
   alignment restrictions in x86. */

struct spec_byte_t * core_oracle_t::get_spec_mem_node(void)
{
  struct spec_byte_t * p;
  if(spec_mem_free_pool)
  {
    p = spec_mem_free_pool;
    spec_mem_free_pool = p->next;
    p->next = NULL;
    /* other fields were cleared when returned to free pool */
    return p;
  }
  else
  {
    p = (struct spec_byte_t*) calloc(1,sizeof(*p));
    if(!p)
      fatal("couldn't malloc spec_mem node");
    return p;
  }
}

void core_oracle_t::return_spec_mem_node(struct spec_byte_t * const p)
{
  assert(p);
  p->next = spec_mem_free_pool;
  spec_mem_free_pool = p;
  p->prev = NULL;
  p->addr = 0;
  p->val = 0;
}

/* similar to commit_write_byte, but remove the tail entry
   instead (used on a uop-flush/squash). */
void core_oracle_t::squash_write_byte(struct spec_byte_t * const p)
{
  const int index = p->addr & MEM_HASH_MASK;
  assert(spec_mem_map.hash[index].tail == p);

  if(p->prev)
    p->prev->next = NULL;
  spec_mem_map.hash[index].tail = p->prev;
  if(p->prev == NULL)
  {
    spec_mem_map.hash[index].head = NULL;
    assert(spec_mem_map.hash[index].tail == NULL);
  }

  return_spec_mem_node(p);
}

/* if a Mop is partially executed in oracle-exec and then
   aborts (likely due to it being on the wrong path and
   containing bogus ops), speculative writes from the
   partially executed uops need to be cleaned up. */
void core_oracle_t::cleanup_aborted_mop(struct Mop_t * const Mop)
{
  int i = Mop->decode.flow_length - 1;
  while(i>=0)
  {
    struct uop_t * uop = &Mop->uop[i];

    if(!uop->decode.is_imm)
    {
      for(int j=0;j<12;j++)
      {
        if(uop->oracle.spec_mem[j] == NULL)
          break;
        //squash_write_byte(uop->oracle.spec_mem[j]);
        uop->oracle.spec_mem[j] = NULL;
      }
    }
    i--;
  }
}





/* fetch an instruction */
void
md_fetch_inst(md_inst_t *inst, struct mem_t *mem, const md_addr_t pc, unsigned int core_id)
{
  unsigned  int i,len,memops;
#ifdef USE_PIN_TRACES

  fscanf(cores[core_id]->current_thread->pin_trace,"%d ",&len);
  fscanf(cores[core_id]->current_thread->pin_trace,"%d ",&memops);
  //fprintf(stdout,"Store Next PC %llx ", store_nextPC);

  inst->vaddr=pc;//store_nextPC[core_id];	
  inst->paddr=pc;//store_nextPC[core_id];
  inst->qemu_len=len;
  //fprintf(stdout,"\n[%lld][Core%d]Trace DequeuedPC: 0x%llx   ",sim_cycle,core_id,inst->vaddr);
  for (i = 0; i < len; i++) 
  {
	if(feof(cores[core_id]->current_thread->pin_trace))
		longjmp(sim_exit_buf, /* exitcode + fudge */0 + 1);
	fscanf(cores[core_id]->current_thread->pin_trace,"%x ",&(inst->code[i]));
	//fprintf(stdout," %x", inst->code[i]);
  }

  inst->mem_ops.mem_vaddr_ld[0]=0;
  inst->mem_ops.mem_vaddr_ld[1]=0;
  inst->mem_ops.mem_vaddr_str[0]=0;
  inst->mem_ops.mem_vaddr_str[1]=0;
  if(memops)
  {
	unsigned long long ch;
	inst->mem_ops.memops=memops;
	fscanf(cores[core_id]->current_thread->pin_trace," %llx ",&ch);
	//fprintf(stdout,"\nTrace Another memory op out of %d %llx ", memops,ch);

	if(ch!=0x00 || ch!=0x01)
		store_nextPC[core_id]=ch;
	while(true)
	{
		if(feof(cores[core_id]->current_thread->pin_trace))
			longjmp(sim_exit_buf, /* exitcode + fudge */0 + 1);
		if(ch==0)
		{
			if(inst->mem_ops.mem_vaddr_ld[0]==0)
			{
				fscanf(cores[core_id]->current_thread->pin_trace,"%llx %d",&(inst->mem_ops.mem_vaddr_ld[0]), &(inst->mem_ops.ld_size[0]) );
				//fprintf(stdout,"\nTrace Read First %llx Size %d ",inst->mem_ops.mem_vaddr_ld[0], inst->mem_ops.ld_size[0]);
				inst->mem_ops.mem_paddr_ld[0]= inst->mem_ops.mem_vaddr_ld[0];
				inst->mem_ops.ld_dequeued[0]=false;
				inst->mem_ops.memops++;
			}
			else
			{
				fscanf(cores[core_id]->current_thread->pin_trace,"%llx %d",&(inst->mem_ops.mem_vaddr_ld[1]), &(inst->mem_ops.ld_size[1]) );
				//fprintf(stdout,"\nTrace Read Second %llx Size %d ",inst->mem_ops.mem_vaddr_ld[1], inst->mem_ops.ld_size[1] );
				inst->mem_ops.mem_paddr_ld[1]= inst->mem_ops.mem_vaddr_ld[1];
				inst->mem_ops.ld_dequeued[1]=false;
				inst->mem_ops.memops++;
			}
		}else if (ch==1)
		      {
			if(inst->mem_ops.mem_vaddr_str[0]==0)
			{
				fscanf(cores[core_id]->current_thread->pin_trace,"%llx %d",&(inst->mem_ops.mem_vaddr_str[0]), &(inst->mem_ops.str_size[0]) );
				//fprintf(stdout,"\nTrace Write First %llx Size %d ",inst->mem_ops.mem_vaddr_str[0], inst->mem_ops.str_size[0]);
				inst->mem_ops.mem_paddr_str[0]= inst->mem_ops.mem_vaddr_str[0];
				inst->mem_ops.str_dequeued[0]=false;
				inst->mem_ops.memops++;	
			}
			else
			{
				fscanf(cores[core_id]->current_thread->pin_trace,"%llx %d",&(inst->mem_ops.mem_vaddr_str[1]), &(inst->mem_ops.str_size[1]) );
				//fprintf(stdout,"\nTrace Write Second %llx Size %d ",inst->mem_ops.mem_vaddr_str[1], inst->mem_ops.str_size[1]);
				inst->mem_ops.mem_paddr_str[1]= inst->mem_ops.mem_vaddr_str[1];
				inst->mem_ops.str_dequeued[1]=false;
				inst->mem_ops.memops++;
			}
		      }
		      else
			{
				//fprintf(stdout, "\nTrace Encountered New instruction");
				store_nextPC[core_id]=ch;
				use_stored_nextPC[core_id]=true;
				break;
			}
		fscanf(cores[core_id]->current_thread->pin_trace," %llx ",&ch);
		//fprintf(stdout,"\nTrace Another memory op out of %d %llx ", memops,ch);
	}
  }
  else
	use_stored_nextPC[core_id]=false;

#else
  //Queue *q_ptr = cores[core_id]->current_thread->q;
  
  Queue *q_ptr = q[core_id]; 
refetch_instr:

  if(!q_ptr->empty())
  {
        if (q_ptr->front().type == QueueItem::INST)
	{
		//fprintf(stdout,"\n[%lld]Oracle.start DequeuedPC: 0x%llx   ",sim_cycle,q_ptr->front().data.inst.vaddr);
		for (i = 0; i < q_ptr->front().data.inst.len; i++) 
		{
			inst->code[i] = q_ptr->front().data.inst.bytes[i]; 
            		//fprintf(stdout,"%x ", inst->code[i]);
		}
		//fprintf(stdout," Length %d",q_ptr->front().data.inst.len );
		inst->vaddr=q_ptr->front().data.inst.vaddr;
		inst->paddr=q_ptr->front().data.inst.paddr;
		inst->qemu_len=q_ptr->front().data.inst.len;
		inst->mem_ops.mem_vaddr_ld[0]=0;
		inst->mem_ops.mem_vaddr_ld[1]=0;
		inst->mem_ops.mem_vaddr_str[0]=0;
		inst->mem_ops.mem_vaddr_str[1]=0;
		inst->mem_ops.memops=0;
	}else if (q_ptr->front().type == QueueItem::INTR)
	{
	  q_ptr->pop();
	  goto refetch_instr;
        }else
		fprintf(stdout,"\nMemory Op found where looking for an instruction");
	q_ptr->pop();
	if(q_ptr->empty())
	{
		//fprintf(stdout,"\nRunning QSIM in md_fetch_pc Queue empty after instruction dequeued ");
		cd->run(core_id, 500);
	}
	int index=0;
	while (q_ptr->front().type == QueueItem::MEM)
	{
		//fprintf(stdout,"\nSize of the Queue %d",q_ptr->size());
		//fprintf(stdout,"\nMemory-op found\n");
		index++;
		if(q_ptr->front().data.mem.type==MEM_RD)
		{
			if(inst->mem_ops.mem_vaddr_ld[0]==0)
			{
				inst->mem_ops.mem_vaddr_ld[0]=q_ptr->front().data.mem.vaddr;
				inst->mem_ops.mem_paddr_ld[0]=q_ptr->front().data.mem.paddr;
				inst->mem_ops.ld_dequeued[0]=false;
				inst->mem_ops.ld_size[0]=q_ptr->front().data.mem.size;
				inst->mem_ops.memops++;
				//fprintf(stdout,"\nLoad MemOp address is 0x%llx Size=%d ",q_ptr->front().data.mem.paddr,q_ptr->front().data.mem.size );
			}else if(inst->mem_ops.mem_vaddr_ld[1]==0)
			      {
				inst->mem_ops.mem_vaddr_ld[1]=q_ptr->front().data.mem.vaddr;
				inst->mem_ops.mem_paddr_ld[1]=q_ptr->front().data.mem.paddr;
				inst->mem_ops.ld_dequeued[1]=false;
				inst->mem_ops.ld_size[1]=q_ptr->front().data.mem.size;
				inst->mem_ops.memops++;
				//fprintf(stdout,"\nLoad MemOp address is 0x%llx ",q_ptr->front().data.mem.vaddr);
			      }//else
					//fprintf(stdout,"\n[%lld]Oracle More than two Load micro-ops for one instruction",sim_cycle);
		}
		if(q_ptr->front().data.mem.type==MEM_WR)
		{
			
			if(inst->mem_ops.mem_vaddr_str[0]==0)
			{
				inst->mem_ops.mem_vaddr_str[0]=q_ptr->front().data.mem.vaddr;
				inst->mem_ops.mem_paddr_str[0]=q_ptr->front().data.mem.paddr;
				inst->mem_ops.str_dequeued[0]=false;
				inst->mem_ops.str_size[0]=q_ptr->front().data.mem.size;
				inst->mem_ops.memops++;
				//fprintf(stdout,"\nStore MemOp address is 0x%llx ",q_ptr->front().data.mem.vaddr);
			}else if(inst->mem_ops.mem_vaddr_str[1]==0)
			      {
				inst->mem_ops.mem_vaddr_str[1]=q_ptr->front().data.mem.vaddr;
				inst->mem_ops.mem_paddr_str[1]=q_ptr->front().data.mem.paddr;
				inst->mem_ops.str_dequeued[1]=false;
				inst->mem_ops.str_size[1]=q_ptr->front().data.mem.size;
				inst->mem_ops.memops++;
				//fprintf(stdout,"\nStore MemOp address is 0x%llx ",q_ptr->front().data.mem.vaddr);
			      }//else
					//fprintf(stdout,"[%lld]Oracle More than two Store micro-ops for one instruction",sim_cycle);
		}
		q_ptr->pop();
		if(q_ptr->empty())
		{
//			fprintf(stdout,"\n[%lld][%d]Running QSIM in md_fetch_pc in end",sim_cycle,core_id);
			cd->run(core_id, 500);
		}
	}	//end of while
	//fprintf(stdout, " --Number of memory-ops %d",index);
   }else
   {
	//fprintf(stdout,"\nRunning QSIM in md_fetch_pc Queue empty in begining");
//	fprintf(stdout,"\n[%lld][%d]Running QSIM in md_fetch_pc",sim_cycle,core_id);
	cd->run(core_id, 500);
	goto refetch_instr;
   }
#endif
}




#ifdef USE_PIN_TRACES

/*fetch PC of next instruction but not the instruction*/
md_addr_t md_fetch_next_pc(md_addr_t *nextPC,unsigned int core_id)
{	
	if(use_stored_nextPC[core_id]==true)
		*nextPC=store_nextPC[core_id];
	else
	{
		if(!feof(cores[core_id]->current_thread->pin_trace))
			fscanf(cores[core_id]->current_thread->pin_trace,"%llx", &store_nextPC[core_id]);
		else
		{
			fclose(cores[core_id]->current_thread->pin_trace);
			char ch[50];
			sprintf (ch, "source/zesto/itrace%d.out",core_id);
			//printf ("[%s] is a pin trace file\n",ch);
			cores[core_id]->current_thread->pin_trace =fopen(ch,"r");
			//longjmp(sim_exit_buf, /* exitcode + fudge */0 + 1);
			fscanf(cores[core_id]->current_thread->pin_trace,"%llx", &store_nextPC[core_id]);
			cores[core_id]->current_thread->regs.regs_PC=store_nextPC[core_id];
			cores[core_id]->current_thread->regs.regs_NPC=store_nextPC[core_id];
			cores[core_id]->fetch->PC = cores[core_id]->current_thread->regs.regs_PC;
		}
		*nextPC=store_nextPC[core_id];
	}
}	


#else


/*fetch PC of next instruction but not the instruction*/
md_addr_t md_fetch_next_pc(md_addr_t *nextPC, unsigned int core_id)
{
  int i;
  Queue *q_ptr = q[core_id]; 
refetch_instr:

  //fprintf(stdout,"\nNumber of instructions %ld", cores[core_id]->stat.oracle_total_insn);
  md_addr_t PC;
  if(!q_ptr->empty())
  {
        if (q_ptr->front().type == QueueItem::INST)
        {
                *nextPC=q_ptr->front().data.inst.vaddr;
		//fprintf(stdout,"\n[%d]--Queued NextPC is 0x%llx",core_id,q_ptr->front().data.inst.vaddr);
		for (i = 0; i < q_ptr->front().data.inst.len; i++)
                {
                       //fprintf(stdout," %x", q_ptr->front().data.inst.bytes[i]);
                }
                //fprintf(stdout," Length %d", q_ptr->front().data.inst.len);
        }else if(q_ptr->front().type == QueueItem::INTR)
        {
	  //fprintf(stdout,"\n[%d]Interrrupt seen in md_fetch_next_PC",core_id);
          q_ptr->pop();
          goto refetch_instr;
        }else if(q_ptr->front().type == QueueItem::MEM)
	{	
	  //fprintf(stdout,"\n[%d]Memory ops found in the nextPC entry WIERD",core_id);
	  //fprintf(stdout,"\n[%d]WIERD MemOp address is 0x%llx ",core_id,q_ptr->front().data.mem.vaddr);
	  
          q_ptr->pop();
          goto refetch_instr;
	}
  }
  else
  {
//	fprintf(stdout,"\n[%lld][%d]Running QSIM in md_fetch_next_pc",sim_cycle,core_id);
	cd->run(core_id, 500);
	goto refetch_instr;
  }
  return(PC);

}
#endif	

