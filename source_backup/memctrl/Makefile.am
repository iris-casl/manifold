#################################################################
#	This is the automake file for compiling
#	the libsimMC library. Files to be added to
#	libsimMC.a should be added in this file to
#	SIMMC_SOURCES and noinst_HEADERS
#	Author: Mitchelle Rasquinha [lotdinity.casl@gmail.com]
#################################################################

#bin_PROGRAMS = manifold_simMC 
lib_LIBRARIES = libsimMCSA.a libsimMC.a

SIMMC_SOURCES = \
		  addr_map.cc \
		  request.cc \
		  request_handler.cc \
		  channel_handler.cc \
		  rank_handler.cc \
		  bank_handler.cc \
		  bus_handler.cc \
		  cmd_issuer.cc \
		  bus.cc \
		  data_bus_handler.cc \
		  cmd_bus_handler.cc \
		  dram.cc \
		  refresh_manager.cc \
		  response_handler.cc \
		  stats.cc \
		  MC.cc 

##libsimMC
libsimMC_a_CXXFLAGS = $(CXX_FLAGS) -DMEM_STANDALONE=0
libsimMC_a_SOURCES = $(SIMMC_SOURCES) \
		     mshr.cc

##libsimMCSA
libsimMCSA_a_CXXFLAGS = $(CXX_FLAGS) -DMEM_STANDALONE=1
libsimMCSA_a_SOURCES = $(SIMMC_SOURCES) \
		       mshr_standalone.cc


## manifold_simMC
#manifold_simMC_CXXFLAGS = $(CXX_FLAGS) 
#manifold_simMC_CXXLD= $(CXXLD) 
#manifold_simMC_SOURCES = $(FE_SOURCES)\
    #			 ../frontend/impl/manifold_simmc.cc

#manifold_simMC_LDADD = libsimMCSA.a  ../simIris/libIris.a

##Headers
noinst_HEADERS = $(ALL_HDRS)
