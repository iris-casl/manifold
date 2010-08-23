#ifndef __QSIM_H
#define __QSIM_H
/*****************************************************************************\
* Qemu Simulation Framework (qsim)                                            *
* Qsim is a modified version of the Qemu emulator (www.qemu.org), coupled     *
* a C++ API, for the use of computer architecture researchers.                *
*                                                                             *
* This work is licensed under the terms of the GNU GPL, version 2. See the    *
* COPYING file in the top-level directory.                                    *
\*****************************************************************************/
#include <map>
#include <sstream>
#include <string>
#include <queue>
#include <algorithm>
#include <stdint.h>

namespace Qsim {
  typedef uint64_t Time;

  enum MemOpType { MEM_READ = 0, MEM_WRITE };

  struct ParamError { };

  class Callback {
  public:
    virtual void inst(uint64_t vaddr, uint64_t paddr, 
                      uint8_t len, uint8_t inst[]) {}
    virtual void memOp(uint64_t vaddr, uint64_t paddr, 
                       uint8_t size, MemOpType type) {}
    virtual void magic(uint64_t rax) {}
  };

  class Params : public std::map<std::string, std::string> {
  public:
    Params() {}
    template <typename T> Params(std::string k, const T& v) {
      operator()(k, v);
    }

    template <typename T> Params & operator()(std::string k, const T& v) {
      std::stringstream ss;
      ss << v;
      Params &p = *this;
      p[k] = ss.str();
      return p;
    }
  };

  class Vm {
  public:
    Vm(Params &p);
    ~Vm();
    bool run(unsigned long cycles);
    void setIpc(double ipc);

    uint8_t memRead(uint64_t addr);
    void memWrite(uint64_t paddr, uint8_t data);
    uint64_t memSize();

    Callback *cb;

  private:
    int vmId;
    static int lastVmId;

    unsigned long clockFreq;
    double ipc;
    Time time;
    std::string vmPath;
    std::string hdPath;
    std::string biosPath;
    std::string serialPath;

    static Callback nullCallback;

    // dlopened object representing Qemu
    void *dlHandle;

    // Function pointers into Qemu
    int (*qemuMain)(int id, int argc, const char **argv);
    int (*qemuRun)(int instructions, double ipc, uint64_t start_cycle);
    
    uint8_t (*qemuMemRead)(uint64_t addr);
    void (*qemuMemWrite)(uint64_t addr, uint8_t data);
    uint64_t (*qemuMemSize)();

  };

  struct VmQItem { public: virtual ~VmQItem() {} };

  struct VmQInst : public VmQItem {
    VmQInst(uint64_t vaddr, uint64_t paddr, uint8_t len, uint8_t data[]) :
      vaddr(vaddr), paddr(paddr), len(len) {
      inst = new uint8_t[len];
      std::copy(data, data+len, inst);
    }

    ~VmQInst() { delete inst; }

    uint64_t vaddr, paddr;
    uint8_t len;
    uint8_t *inst;
  };

  struct VmQMemOp : public VmQItem {
    VmQMemOp(uint64_t vaddr, uint64_t paddr, uint8_t size, MemOpType type) :
      vaddr(vaddr), paddr(paddr), size(size), type(type) {}
    uint64_t vaddr, paddr;
    uint8_t size;
    MemOpType type;
  };

  struct VmQMagic : public VmQItem {
    VmQMagic(uint64_t rax): rax(rax) {}
    uint64_t rax;
  };

  class VmQ {
  public:
    VmQ(Vm &v);
    ~VmQ();

    VmQItem *front() { return q.front(); }
    void push(VmQItem *i) { q.push(i); }
    void pop() { delete q.front(); q.pop(); }
    bool empty() { return q.empty(); }
      
  private:
    std::queue<VmQItem *> q;

    class VmQCallback : public Callback {
    public:
      VmQ *vQ;
      VmQCallback(VmQ& v) : vQ(&v) { }
      
      void inst(uint64_t vaddr, uint64_t paddr, uint8_t len, uint8_t inst[]);
      void memOp(uint64_t vaddr, uint64_t paddr, uint8_t size, MemOpType type);
      void magic(uint64_t rax);
    };

    VmQCallback callback;
  };

};

#endif
