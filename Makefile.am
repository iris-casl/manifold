bin_PROGRAMS = manifold_simIris manifold_fullSim manifold_simMC 

OBJDIR = $(TOPDIR)/build
BUILDDIR = $(TOPDIR)/build
CXX_FLAGS = -D_FILE_OFFSET_BITS=64 -I source -m32 -ansi -g -gstabs+ -O0 -static-libgcc
ZESTO_FLAGS = -O0 -g -m32 -DMIN_SYSCALL_MODE -DUSE_SSE_MOVE -Wall -DDEBUG -msse4a -mfpmath=sse
#Zesto needs to be compiled as 64 bit
#ZESTO_FLAGS = -O0 -g -m64 -DMIN_SYSCALL_MODE -DUSE_SSE_MOVE -Wall -DDEBUG -msse4a -mfpmath=sse
MLIBS  = -lm -ldl -lqsim -L../qsim-0.1-pre5/
#for zesto to be compiled with qsim and g++ 4.4
#ZESTOCXXLD = g++-4.4 -lc -lm -ldl -lqsim -L../qsim-0.1-pre5/
ZESTOCXXLD = g++ -lc -lm -ldl -lqsim -L../qsim-0.1-pre5/
CXXLD = g++ -lc -lm -ldl
CXXLD_PROFILE = g++ -lgcov -lc
#DEBUG_FLAGS = -D_DEBUG -D_DEBUG_ROUTER 
DEBUG_FLAGS = -g -pg -D_STLP_DEBUG -D_DEBUG -DDEBUG -D_DEBUG_ROUTER -D_DEBUG_INTERFACE -DDEBUG
PROFILE_FLAGS = -fprofile-arcs -ftest-coverage -lgcov -g -pg
FE_SOURCES = \
		  source/frontend/impl/mesh.cc \
 		  source/frontend/impl/torus.cc \
		  source/frontend/impl/visual.cc

##source/util/mc_constants.h \

SUBDIRS = source/zesto source/memctrl source/simIris .
ALL_HDRS = \
	   source/kernel/simulator.h \
	   source/kernel/link.h \
	   source/kernel/component.h \
	   source/util/genericData.h \
	   source/util/simIrisComponentHeader.h \
	   source/simIris/data_types/impl/irisEvent.h \
	   source/simIris/data_types/impl/flit.h \
	   source/simIris/data_types/impl/highLevelPacket.h \
	   source/simIris/data_types/impl/lowLevelPacket.h \
	   source/simIris/components/interfaces/crossbar.h \
	   source/simIris/components/interfaces/addressDecoder.h \
	   source/simIris/components/interfaces/interface.h \
	   source/simIris/components/interfaces/buffer.h \
	   source/simIris/components/interfaces/outputBuffer.h \
	   source/simIris/components/interfaces/virtualChannelArbiter.h \
	   source/simIris/components/interfaces/router.h \
	   source/simIris/components/interfaces/portArbiter.h \
	   source/simIris/components/interfaces/processor.h \
	   source/simIris/components/interfaces/irisLink.h \
	   source/simIris/components/interfaces/networkComponent.h \
	   source/simIris/components/impl/genericRC.h \
	   source/simIris/components/impl/genericCrossbar.h \
	   source/simIris/components/impl/genericBuffer.h \
	   source/simIris/components/impl/genericInterfaceVcs.h \
	   source/simIris/components/impl/genericVcArbiter.h \
	   source/simIris/components/impl/genericArbiter.h \
	   source/simIris/components/impl/genericLink.h \
	   source/simIris/components/impl/pvtopv_swa.h \
	   source/simIris/components/impl/ptop_swa.h \
	   source/simIris/components/impl/ptopSwaVcs.h \
	   source/simIris/components/impl/genericRouter4Stg.h \
	   source/simIris/components/impl/genericRouter3Stg.h \
	   source/simIris/components/impl/genericTracePktGen.h \
	   source/simIris/components/impl/genericVcAllocator.h \
	   source/simIris/components/impl/genericFlatMc.h \
	   source/simIris/components/impl/memctrlFE.h \
	   source/frontend/impl/topology.h \
	   source/frontend/impl/mesh.h \
           source/frontend/impl/visual.h \
	   source/frontend/impl/torus.h \
	   source/memctrl/addr_map.h \
	   source/memctrl/request.h \
	   source/memctrl/request_handler.h \
	   source/memctrl/channel_handler.h \
	   source/memctrl/rank_handler.h \
	   source/memctrl/bank_handler.h \
	   source/memctrl/bus_handler.h \
	   source/memctrl/cmd_issuer.h \
	   source/memctrl/bus.h \
	   source/memctrl/data_bus_handler.h \
	   source/memctrl/cmd_bus_handler.h \
	   source/memctrl/dram.h \
	   source/memctrl/refresh_manager.h \
	   source/memctrl/response_handler.h \
	   source/memctrl/stats.h \
	   source/memctrl/mshr.h \
	   source/memctrl/mshr_standalone.h \
	   source/zesto/bbtracker.h \
	   source/zesto/thread.h    \
	   source/zesto/eio.h \
	   source/zesto/endian.h \
	   source/zesto/eval.h \
	   source/zesto/host.h \
	   source/zesto/loader.h \
	   source/zesto/machine.h \
	   source/zesto/memory.h  \
	   source/zesto/misc.h \
	   source/zesto/options.h \
	   source/zesto/range.h \
	   source/zesto/regs.h \
	   source/zesto/sim.h  \
	   source/zesto/stats.h \
	   source/zesto/symbol.h \
	   source/zesto/syscall.h \
	   source/zesto/version.h  \
	   source/zesto/machine.def \
	   source/zesto/elf.h \
	   source/zesto/x86flow.def \
	   source/zesto/zesto-structs.h \
	   source/zesto/zesto-core.h \
	   source/zesto/zesto-opts.h \
	   source/zesto/zesto-oracle.h \
	   source/zesto/zesto-fetch.h \
	   source/zesto/zesto-decode.h \
	   source/zesto/zesto-alloc.h \
	   source/zesto/zesto-exec.h \
	   source/zesto/zesto-commit.h \
	   source/zesto/zesto-cache.h \
	   source/zesto/zesto-bpred.h \
	   source/zesto/zesto-memdep.h \
	   source/zesto/zesto-prefetch.h \
	   source/zesto/zesto-uncore.h 


## manifold_fullSim
manifold_fullSim_CXXFLAGS = $(CXX_FLAGS) 
manifold_fullSim_CXXLD= $(ZESTOCXXLD) 

manifold_fullSim_SOURCES = $(FE_SOURCES)\
			   source/frontend/impl/manifold_fullsim.cc

manifold_fullSim_LDADD = source/simIris/libIris.a \
			 source/memctrl/libsimMC.a \
			 source/zesto/libZesto.a 

## manifold_simIris
manifold_simIris_CXXFLAGS = $(CXX_FLAGS) 
manifold_simIris_CXXLD= $(CXXLD) 

manifold_simIris_SOURCES = $(FE_SOURCES)\
			   source/frontend/impl/manifold_simiris.cc

manifold_simIris_LDADD = source/simIris/libIris.a \
			 source/memctrl/libsimMC.a 

## manifold_simMC
manifold_simMC_CXXFLAGS = $(CXX_FLAGS) 
manifold_simMC_CXXLD= $(CXXLD) 
manifold_simMC_SOURCES = $(FE_SOURCES)\
			 source/frontend/impl/manifold_simmc.cc

manifold_simMC_LDADD = source/simIris/libIris.a \
		       source/memctrl/libsimMCSA.a 

##Headers
noinst_HEADERS = \
		 source/frontend/impl/mesh.h \
		 source/util/config_params.h \
		 $(ALL_HDRS)
