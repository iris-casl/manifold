lib_LIBRARIES = libIris.a

IRIS_SOURCES = \
		  ../kernel/link.cc \
		  ../kernel/clock.cc \
		  ../kernel/simulator.cc \
		  ../kernel/component.cc \
		  ../util/genericData.cc \
		  ../util/stats.cc \
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
		  components/impl/genericInterfaceNB.cc \
		  components/impl/genericArbiter.cc \
		  components/impl/genericLink.cc \
		  components/impl/ptopSwaVcs.cc \
		  components/impl/pvtopv_swa.cc \
		  components/impl/ptop_swa.cc \
		  components/impl/vcaMsgAware.cc \
		  components/impl/routerVcMP.cc \
		  components/impl/routerReqReply.cc \
		  components/impl/genericRouterPhy.cc \
		  components/impl/genericTracePktGen.cc \
		  components/impl/genericSink.cc \
		  components/impl/genericPktGen.cc \
		  components/impl/genericVcAllocator.cc \
		  components/impl/mcFrontEnd.cc \
		  components/impl/genericFlatMc.cc 

#		  components/impl/genericRouter4Stg.cc \
##libIris
libIris_a_CXXFLAGS = $(CXX_FLAGS)
libIris_a_SOURCES = $(IRIS_SOURCES)


##Headers
noinst_HEADERS = $(ALL_HDRS)
