TAR = mcpat

ALL_MCPAT_OBJS=obj_opt/Ucache.o obj_opt/XML_Parse.o obj_opt/arbiter.o \
	       obj_opt/area.o \
	       obj_opt/array.o obj_opt/bank.o obj_opt/basic_circuit.o \
	       obj_opt/basic_components.o obj_opt/cacti_interface.o \
	       obj_opt/component.o obj_opt/core.o obj_opt/crossbar.o \
	       obj_opt/decoder.o obj_opt/htree2.o obj_opt/interconnect.o \
	       obj_opt/io.o obj_opt/logic.o \
	       obj_opt/iris_interface.o obj_opt/mat.o obj_opt/memoryctrl.o \
	       obj_opt/noc.o obj_opt/nuca.o obj_opt/parameter.o \
	       obj_opt/processor.o obj_opt/router.o obj_opt/sharedcache.o \
	       obj_opt/subarray.o obj_opt/technology.o obj_opt/uca.o \
	       obj_opt/wire.o obj_opt/xmlParser.o

.PHONY: dbg opt depend clean clean_dbg clean_opt

all: opt

maintainer-clean: 

dbg: $(TAR).mk obj_dbg
	$(TAR).mk

#@$(MAKE) TAG=dbg -C . -f $(TAR).mk
opt: $(TAR).mk obj_opt
	make TAG=opt -C . -f $(TAR).mk 

#ar cru libMcpat.a $(ALL_MCPAT_OBJS)
#	ranlib libMcpat.a

#@$(MAKE) TAG=opt -C . -f $(TAR).mk 
obj_dbg:
	mkdir $@

obj_opt:
	mkdir $@

clean: clean_dbg clean_opt

clean_dbg: obj_dbg
	@$(MAKE) TAG=dbg -C . -f $(TAR).mk clean
	rm -rf $<

clean_opt: obj_opt
	@$(MAKE) TAG=opt -C . -f $(TAR).mk clean
	rm -rf $<

