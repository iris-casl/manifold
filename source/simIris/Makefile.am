lib_LIBRARIES = libIris.a

IRIS_SOURCES = \
		  ../kernel/link.cc \
		  ../kernel/clock.cc \
		  ../kernel/simulator.cc \
		  ../kernel/component.cc \
		  ../util/genericData.cc \
		  data_types/impl/irisEvent.cc \
		  data_types/impl/flit.cc \
		  data_types/impl/highLevelPacket.cc \
		  data_types/impl/lowLevelPacket.cc \
		  components/interfaces/router.cc \
		  components/interfaces/processor.cc \
		  components/interfaces/interface.cc \
		  components/interfaces/irisLink.cc \
		  components/interfaces/networkComponent.cc \
		  components/impl/genericBuffer.cc \
		  components/impl/genericRC.cc \
		  components/impl/genericCrossbar.cc \
		  components/impl/genericInterfaceVcs.cc \
		  components/impl/genericVcArbiter.cc \
		  components/impl/genericArbiter.cc \
		  components/impl/genericLink.cc \
		  components/impl/ptopSwaVcs.cc \
		  components/impl/pvtopv_swa.cc \
		  components/impl/ptop_swa.cc \
		  components/impl/genericRouterVcs.cc \
		  components/impl/genericRouterAdaptive.cc \
		  components/impl/genericTPGVcs.cc \
		  components/impl/genericVcAllocator.cc \
		  components/impl/genericFlatMc.cc \
		  components/impl/memctrlFE.cc

##libIris
libIris_a_CXXFLAGS = $(CXX_FLAGS)
libIris_a_SOURCES = $(IRIS_SOURCES)


##Headers
noinst_HEADERS = $(ALL_HDRS)
