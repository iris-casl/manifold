# Makefile.in generated by automake 1.10.2 from Makefile.am.
# Makefile.  Generated from Makefile.in by configure.

# Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
# 2003, 2004, 2005, 2006, 2007, 2008  Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.





pkgdatadir = $(datadir)/Iris
pkglibdir = $(libdir)/Iris
pkgincludedir = $(includedir)/Iris
am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
bin_PROGRAMS = manifold_simIris$(EXEEXT)
subdir = .
DIST_COMMON = README $(am__configure_deps) $(noinst_HEADERS) \
	$(srcdir)/Makefile.am $(srcdir)/Makefile.in \
	$(srcdir)/config.h.in $(top_srcdir)/configure AUTHORS COPYING \
	ChangeLog INSTALL NEWS depcomp install-sh missing
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps = $(top_srcdir)/configure.ac
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
am__CONFIG_DISTCLEAN_FILES = config.status config.cache config.log \
 configure.lineno config.status.lineno
mkinstalldirs = $(install_sh) -d
CONFIG_HEADER = config.h
CONFIG_CLEAN_FILES =
am__installdirs = "$(DESTDIR)$(bindir)"
binPROGRAMS_INSTALL = $(INSTALL_PROGRAM)
PROGRAMS = $(bin_PROGRAMS)
am__objects_1 = manifold_simIris-mesh.$(OBJEXT) \
	manifold_simIris-torus.$(OBJEXT) \
	manifold_simIris-ring.$(OBJEXT) \
	manifold_simIris-visual.$(OBJEXT)
am_manifold_simIris_OBJECTS = $(am__objects_1) \
	manifold_simIris-manifold_simiris.$(OBJEXT)
manifold_simIris_OBJECTS = $(am_manifold_simIris_OBJECTS)
manifold_simIris_DEPENDENCIES = source/simIris/libIris.a
manifold_simIris_LINK = $(CXXLD) $(manifold_simIris_CXXFLAGS) \
	$(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
DEFAULT_INCLUDES = -I.
depcomp = $(SHELL) $(top_srcdir)/depcomp
am__depfiles_maybe = depfiles
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
CXXLINK = $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) \
	-o $@
SOURCES = $(manifold_simIris_SOURCES)
DIST_SOURCES = $(manifold_simIris_SOURCES)
RECURSIVE_TARGETS = all-recursive check-recursive dvi-recursive \
	html-recursive info-recursive install-data-recursive \
	install-dvi-recursive install-exec-recursive \
	install-html-recursive install-info-recursive \
	install-pdf-recursive install-ps-recursive install-recursive \
	installcheck-recursive installdirs-recursive pdf-recursive \
	ps-recursive uninstall-recursive
HEADERS = $(noinst_HEADERS)
RECURSIVE_CLEAN_TARGETS = mostlyclean-recursive clean-recursive	\
  distclean-recursive maintainer-clean-recursive
ETAGS = etags
CTAGS = ctags
DIST_SUBDIRS = $(SUBDIRS)
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
distdir = $(PACKAGE)-$(VERSION)
top_distdir = $(distdir)
am__remove_distdir = \
  { test ! -d $(distdir) \
    || { find $(distdir) -type d ! -perm -200 -exec chmod u+w {} ';' \
         && rm -fr $(distdir); }; }
DIST_ARCHIVES = $(distdir).tar.gz
GZIP_ENV = --best
distuninstallcheck_listfiles = find . -type f -print
distcleancheck_listfiles = find . -type f -print
ACLOCAL = ${SHELL} /home/sharda/Desktop/manifold/missing --run aclocal-1.10
AMTAR = ${SHELL} /home/sharda/Desktop/manifold/missing --run tar
AUTOCONF = ${SHELL} /home/sharda/Desktop/manifold/missing --run autoconf
AUTOHEADER = ${SHELL} /home/sharda/Desktop/manifold/missing --run autoheader
AUTOMAKE = ${SHELL} /home/sharda/Desktop/manifold/missing --run automake-1.10
AWK = gawk
CC = gcc
CCDEPMODE = depmode=gcc3
CFLAGS = -g -O2
CPP = gcc -E
CPPFLAGS = 
CXX = g++
CXXDEPMODE = depmode=gcc3
CXXFLAGS = -g -O2
CYGPATH_W = echo
DEFS = -DHAVE_CONFIG_H
DEPDIR = .deps
ECHO_C = 
ECHO_N = -n
ECHO_T = 
EGREP = /bin/grep -E
EXEEXT = 
GREP = /bin/grep
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = $(install_sh) -c -s
LDFLAGS = 
LIBOBJS = 
LIBS = 
LTLIBOBJS = 
MAKEINFO = ${SHELL} /home/sharda/Desktop/manifold/missing --run makeinfo
MKDIR_P = /bin/mkdir -p
OBJEXT = o
PACKAGE = Iris
PACKAGE_BUGREPORT = mitchelle dot rasquinha at gatech.edu
PACKAGE_NAME = iris
PACKAGE_STRING = iris 0.1
PACKAGE_TARNAME = iris
PACKAGE_URL = 
PACKAGE_VERSION = 0.1
PATH_SEPARATOR = :
RANLIB = ranlib
SET_MAKE = 
SHELL = /bin/bash
STRIP = 
VERSION = 1.0
abs_builddir = /home/sharda/Desktop/manifold
abs_srcdir = /home/sharda/Desktop/manifold
abs_top_builddir = /home/sharda/Desktop/manifold
abs_top_srcdir = /home/sharda/Desktop/manifold
ac_ct_CC = gcc
ac_ct_CXX = g++
am__include = include
am__leading_dot = .
am__quote = 
am__tar = ${AMTAR} chof - "$$tardir"
am__untar = ${AMTAR} xf -
bindir = ${exec_prefix}/bin
build_alias = 
builddir = .
datadir = ${datarootdir}
datarootdir = ${prefix}/share
docdir = ${datarootdir}/doc/${PACKAGE_TARNAME}
dvidir = ${docdir}
exec_prefix = ${prefix}
host_alias = 
htmldir = ${docdir}
includedir = ${prefix}/include
infodir = ${datarootdir}/info
install_sh = $(SHELL) /home/sharda/Desktop/manifold/install-sh
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localedir = ${datarootdir}/locale
localstatedir = ${prefix}/var
mandir = ${datarootdir}/man
mkdir_p = /bin/mkdir -p
oldincludedir = /usr/include
pdfdir = ${docdir}
prefix = /usr/local
program_transform_name = s,x,x,
psdir = ${docdir}
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
srcdir = .
sysconfdir = ${prefix}/etc
target_alias = 
top_build_prefix = 
top_builddir = .
top_srcdir = .
OBJDIR = $(TOPDIR)/build
BUILDDIR = $(TOPDIR)/build
CXX_FLAGS = -D_FILE_OFFSET_BITS=64 -I source -m32 -ansi -g -gstabs+ -O0 -static-libgcc
ZESTO_FLAGS = -O0 -g -m32 -DMIN_SYSCALL_MODE -DUSE_SSE_MOVE -Wall -DDEBUG -msse4a -mfpmath=sse
MLIBS = -lm  
ZESTOCXXLD = g++ -lc -lm  
CXXLD = g++ -lc -lm -ldl -lpthread
CXXLD_PROFILE = g++ -lgcov -lc
#DEBUG_FLAGS = -D_DEBUG -D_DEBUG_ROUTER 
DEBUG_FLAGS = -g -pg -D_STLP_DEBUG -D_DEBUG -DDEBUG -D_DEBUG_ROUTER -D_DEBUG_INTERFACE -DDEBUG
PROFILE_FLAGS = -fprofile-arcs -ftest-coverage -lgcov -g -pg
FE_SOURCES = \
	     source/frontend/impl/mesh.cc \
	     source/frontend/impl/torus.cc \
	     source/frontend/impl/ring.cc \
	     source/frontend/impl/visual.cc


#SUBDIRS = source/zesto source/memctrl source/simIris .
SUBDIRS = source/simIris .
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
	   source/util/stats.h \
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
	   source/simIris/components/impl/genericInterfaceNB.h \
	   source/simIris/components/impl/genericArbiter.h \
	   source/simIris/components/impl/genericLink.h \
	   source/simIris/components/impl/pvtopv_swa.h \
	   source/simIris/components/impl/ptop_swa.h \
	   source/simIris/components/impl/ptopSwaVcs.h \
	   source/simIris/components/impl/vcaMsgAware.h \
	   source/simIris/components/impl/genericRouter4Stg.h \
	   source/simIris/components/impl/routerVcMP.h \
	   source/simIris/components/impl/routerReqReply.h \
	   source/simIris/components/impl/genericRouterPhy.h \
	   source/simIris/components/impl/genericPktGen.h \
	   source/simIris/components/impl/genericSink.h \
	   source/simIris/components/impl/genericVcAllocator.h \
	   source/frontend/impl/mesh.h \
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
	   source/simIris/components/impl/genericFlatMc.h \
	   source/memctrl/mshr.h \
	   source/memctrl/mshr_standalone.h 

ZESTO_HDRS = \
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

MCPAT_OBJS = \
	    source/mcpat/obj_opt/Ucache.o source/mcpat/obj_opt/XML_Parse.o \
	    source/mcpat/obj_opt/arbiter.o source/mcpat/obj_opt/area.o \
	    source/mcpat/obj_opt/array.o source/mcpat/obj_opt/bank.o \
	    source/mcpat/obj_opt/basic_circuit.o source/mcpat/obj_opt/basic_components.o \
	    source/mcpat/obj_opt/cacti_interface.o \
	    source/mcpat/obj_opt/component.o source/mcpat/obj_opt/core.o \
	    source/mcpat/obj_opt/crossbar.o source/mcpat/obj_opt/decoder.o \
	    source/mcpat/obj_opt/htree2.o source/mcpat/obj_opt/interconnect.o \
	    source/mcpat/obj_opt/io.o source/mcpat/obj_opt/logic.o \
	    source/mcpat/obj_opt/iris_interface.o source/mcpat/obj_opt/mat.o \
	    source/mcpat/obj_opt/memoryctrl.o source/mcpat/obj_opt/noc.o \
	    source/mcpat/obj_opt/nuca.o source/mcpat/obj_opt/parameter.o source/mcpat/obj_opt/processor.o \
	    source/mcpat/obj_opt/router.o \
	    source/mcpat/obj_opt/sharedcache.o source/mcpat/obj_opt/subarray.o \
	    source/mcpat/obj_opt/technology.o source/mcpat/obj_opt/uca.o \
	    source/mcpat/obj_opt/wire.o source/mcpat/obj_opt/xmlParser.o


### manifold_fullSim
#manifold_fullSim_CXXFLAGS = $(CXX_FLAGS) 
#manifold_fullSim_CXXLD= $(ZESTOCXXLD) 

#manifold_fullSim_SOURCES = $(FE_SOURCES)\
#    #			   source/frontend/impl/manifold_fullsim.cc

#manifold_fullSim_LDADD = source/simIris/libIris.a \
#    #			 source/memctrl/libsimMC.a \
#    #			 source/zesto/libZesto.a 
manifold_simIris_CXXFLAGS = $(CXX_FLAGS) -lpthread -lgsl -lgslcblas
manifold_simIris_CXXLD = $(CXXLD)
manifold_simIris_SOURCES = $(FE_SOURCES)\
			   source/frontend/impl/manifold_simiris.cc

manifold_simIris_LDADD = source/simIris/libIris.a 

#manifold_simMC_CXXFLAGS = $(CXX_FLAGS) 
#manifold_simMC_CXXLD= $(CXXLD) 
#manifold_simMC_SOURCES = $(FE_SOURCES)\
#			 source/frontend/impl/manifold_simmc.cc

#manifold_simMC_LDADD = source/simIris/libIris.a \
#		       source/memctrl/libsimMCSA.a 
noinst_HEADERS = \
		 source/frontend/impl/mesh.h \
		 source/util/config_params.h \
		 $(ALL_HDRS)

all: config.h
	$(MAKE) $(AM_MAKEFLAGS) all-recursive

.SUFFIXES:
.SUFFIXES: .cc .o .obj
am--refresh:
	@:
$(srcdir)/Makefile.in:  $(srcdir)/Makefile.am  $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      echo ' cd $(srcdir) && $(AUTOMAKE) --gnu '; \
	      cd $(srcdir) && $(AUTOMAKE) --gnu  \
		&& exit 0; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --gnu  Makefile'; \
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --gnu  Makefile
.PRECIOUS: Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    echo ' $(SHELL) ./config.status'; \
	    $(SHELL) ./config.status;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $@ $(am__depfiles_maybe)'; \
	    cd $(top_builddir) && $(SHELL) ./config.status $@ $(am__depfiles_maybe);; \
	esac;

$(top_builddir)/config.status: $(top_srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	$(SHELL) ./config.status --recheck

$(top_srcdir)/configure:  $(am__configure_deps)
	cd $(srcdir) && $(AUTOCONF)
$(ACLOCAL_M4):  $(am__aclocal_m4_deps)
	cd $(srcdir) && $(ACLOCAL) $(ACLOCAL_AMFLAGS)

config.h: stamp-h1
	@if test ! -f $@; then \
	  rm -f stamp-h1; \
	  $(MAKE) $(AM_MAKEFLAGS) stamp-h1; \
	else :; fi

stamp-h1: $(srcdir)/config.h.in $(top_builddir)/config.status
	@rm -f stamp-h1
	cd $(top_builddir) && $(SHELL) ./config.status config.h
$(srcdir)/config.h.in:  $(am__configure_deps) 
	cd $(top_srcdir) && $(AUTOHEADER)
	rm -f stamp-h1
	touch $@

distclean-hdr:
	-rm -f config.h stamp-h1
install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	test -z "$(bindir)" || $(MKDIR_P) "$(DESTDIR)$(bindir)"
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  p1=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  if test -f $$p \
	  ; then \
	    f=`echo "$$p1" | sed 's,^.*/,,;$(transform);s/$$/$(EXEEXT)/'`; \
	   echo " $(INSTALL_PROGRAM_ENV) $(binPROGRAMS_INSTALL) '$$p' '$(DESTDIR)$(bindir)/$$f'"; \
	   $(INSTALL_PROGRAM_ENV) $(binPROGRAMS_INSTALL) "$$p" "$(DESTDIR)$(bindir)/$$f" || exit 1; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  f=`echo "$$p" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  echo " rm -f '$(DESTDIR)$(bindir)/$$f'"; \
	  rm -f "$(DESTDIR)$(bindir)/$$f"; \
	done

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)
manifold_simIris$(EXEEXT): $(manifold_simIris_OBJECTS) $(manifold_simIris_DEPENDENCIES) 
	@rm -f manifold_simIris$(EXEEXT)
	$(manifold_simIris_LINK) $(manifold_simIris_OBJECTS) $(manifold_simIris_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT)

distclean-compile:
	-rm -f *.tab.c

include ./$(DEPDIR)/manifold_simIris-manifold_simiris.Po
include ./$(DEPDIR)/manifold_simIris-mesh.Po
include ./$(DEPDIR)/manifold_simIris-ring.Po
include ./$(DEPDIR)/manifold_simIris-torus.Po
include ./$(DEPDIR)/manifold_simIris-visual.Po

.cc.o:
	$(CXXCOMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ $<

.cc.obj:
	$(CXXCOMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ `$(CYGPATH_W) '$<'`
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ `$(CYGPATH_W) '$<'`

manifold_simIris-mesh.o: source/frontend/impl/mesh.cc
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -MT manifold_simIris-mesh.o -MD -MP -MF $(DEPDIR)/manifold_simIris-mesh.Tpo -c -o manifold_simIris-mesh.o `test -f 'source/frontend/impl/mesh.cc' || echo '$(srcdir)/'`source/frontend/impl/mesh.cc
	mv -f $(DEPDIR)/manifold_simIris-mesh.Tpo $(DEPDIR)/manifold_simIris-mesh.Po
#	source='source/frontend/impl/mesh.cc' object='manifold_simIris-mesh.o' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -c -o manifold_simIris-mesh.o `test -f 'source/frontend/impl/mesh.cc' || echo '$(srcdir)/'`source/frontend/impl/mesh.cc

manifold_simIris-mesh.obj: source/frontend/impl/mesh.cc
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -MT manifold_simIris-mesh.obj -MD -MP -MF $(DEPDIR)/manifold_simIris-mesh.Tpo -c -o manifold_simIris-mesh.obj `if test -f 'source/frontend/impl/mesh.cc'; then $(CYGPATH_W) 'source/frontend/impl/mesh.cc'; else $(CYGPATH_W) '$(srcdir)/source/frontend/impl/mesh.cc'; fi`
	mv -f $(DEPDIR)/manifold_simIris-mesh.Tpo $(DEPDIR)/manifold_simIris-mesh.Po
#	source='source/frontend/impl/mesh.cc' object='manifold_simIris-mesh.obj' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -c -o manifold_simIris-mesh.obj `if test -f 'source/frontend/impl/mesh.cc'; then $(CYGPATH_W) 'source/frontend/impl/mesh.cc'; else $(CYGPATH_W) '$(srcdir)/source/frontend/impl/mesh.cc'; fi`

manifold_simIris-torus.o: source/frontend/impl/torus.cc
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -MT manifold_simIris-torus.o -MD -MP -MF $(DEPDIR)/manifold_simIris-torus.Tpo -c -o manifold_simIris-torus.o `test -f 'source/frontend/impl/torus.cc' || echo '$(srcdir)/'`source/frontend/impl/torus.cc
	mv -f $(DEPDIR)/manifold_simIris-torus.Tpo $(DEPDIR)/manifold_simIris-torus.Po
#	source='source/frontend/impl/torus.cc' object='manifold_simIris-torus.o' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -c -o manifold_simIris-torus.o `test -f 'source/frontend/impl/torus.cc' || echo '$(srcdir)/'`source/frontend/impl/torus.cc

manifold_simIris-torus.obj: source/frontend/impl/torus.cc
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -MT manifold_simIris-torus.obj -MD -MP -MF $(DEPDIR)/manifold_simIris-torus.Tpo -c -o manifold_simIris-torus.obj `if test -f 'source/frontend/impl/torus.cc'; then $(CYGPATH_W) 'source/frontend/impl/torus.cc'; else $(CYGPATH_W) '$(srcdir)/source/frontend/impl/torus.cc'; fi`
	mv -f $(DEPDIR)/manifold_simIris-torus.Tpo $(DEPDIR)/manifold_simIris-torus.Po
#	source='source/frontend/impl/torus.cc' object='manifold_simIris-torus.obj' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -c -o manifold_simIris-torus.obj `if test -f 'source/frontend/impl/torus.cc'; then $(CYGPATH_W) 'source/frontend/impl/torus.cc'; else $(CYGPATH_W) '$(srcdir)/source/frontend/impl/torus.cc'; fi`

manifold_simIris-ring.o: source/frontend/impl/ring.cc
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -MT manifold_simIris-ring.o -MD -MP -MF $(DEPDIR)/manifold_simIris-ring.Tpo -c -o manifold_simIris-ring.o `test -f 'source/frontend/impl/ring.cc' || echo '$(srcdir)/'`source/frontend/impl/ring.cc
	mv -f $(DEPDIR)/manifold_simIris-ring.Tpo $(DEPDIR)/manifold_simIris-ring.Po
#	source='source/frontend/impl/ring.cc' object='manifold_simIris-ring.o' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -c -o manifold_simIris-ring.o `test -f 'source/frontend/impl/ring.cc' || echo '$(srcdir)/'`source/frontend/impl/ring.cc

manifold_simIris-ring.obj: source/frontend/impl/ring.cc
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -MT manifold_simIris-ring.obj -MD -MP -MF $(DEPDIR)/manifold_simIris-ring.Tpo -c -o manifold_simIris-ring.obj `if test -f 'source/frontend/impl/ring.cc'; then $(CYGPATH_W) 'source/frontend/impl/ring.cc'; else $(CYGPATH_W) '$(srcdir)/source/frontend/impl/ring.cc'; fi`
	mv -f $(DEPDIR)/manifold_simIris-ring.Tpo $(DEPDIR)/manifold_simIris-ring.Po
#	source='source/frontend/impl/ring.cc' object='manifold_simIris-ring.obj' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -c -o manifold_simIris-ring.obj `if test -f 'source/frontend/impl/ring.cc'; then $(CYGPATH_W) 'source/frontend/impl/ring.cc'; else $(CYGPATH_W) '$(srcdir)/source/frontend/impl/ring.cc'; fi`

manifold_simIris-visual.o: source/frontend/impl/visual.cc
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -MT manifold_simIris-visual.o -MD -MP -MF $(DEPDIR)/manifold_simIris-visual.Tpo -c -o manifold_simIris-visual.o `test -f 'source/frontend/impl/visual.cc' || echo '$(srcdir)/'`source/frontend/impl/visual.cc
	mv -f $(DEPDIR)/manifold_simIris-visual.Tpo $(DEPDIR)/manifold_simIris-visual.Po
#	source='source/frontend/impl/visual.cc' object='manifold_simIris-visual.o' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -c -o manifold_simIris-visual.o `test -f 'source/frontend/impl/visual.cc' || echo '$(srcdir)/'`source/frontend/impl/visual.cc

manifold_simIris-visual.obj: source/frontend/impl/visual.cc
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -MT manifold_simIris-visual.obj -MD -MP -MF $(DEPDIR)/manifold_simIris-visual.Tpo -c -o manifold_simIris-visual.obj `if test -f 'source/frontend/impl/visual.cc'; then $(CYGPATH_W) 'source/frontend/impl/visual.cc'; else $(CYGPATH_W) '$(srcdir)/source/frontend/impl/visual.cc'; fi`
	mv -f $(DEPDIR)/manifold_simIris-visual.Tpo $(DEPDIR)/manifold_simIris-visual.Po
#	source='source/frontend/impl/visual.cc' object='manifold_simIris-visual.obj' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -c -o manifold_simIris-visual.obj `if test -f 'source/frontend/impl/visual.cc'; then $(CYGPATH_W) 'source/frontend/impl/visual.cc'; else $(CYGPATH_W) '$(srcdir)/source/frontend/impl/visual.cc'; fi`

manifold_simIris-manifold_simiris.o: source/frontend/impl/manifold_simiris.cc
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -MT manifold_simIris-manifold_simiris.o -MD -MP -MF $(DEPDIR)/manifold_simIris-manifold_simiris.Tpo -c -o manifold_simIris-manifold_simiris.o `test -f 'source/frontend/impl/manifold_simiris.cc' || echo '$(srcdir)/'`source/frontend/impl/manifold_simiris.cc
	mv -f $(DEPDIR)/manifold_simIris-manifold_simiris.Tpo $(DEPDIR)/manifold_simIris-manifold_simiris.Po
#	source='source/frontend/impl/manifold_simiris.cc' object='manifold_simIris-manifold_simiris.o' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -c -o manifold_simIris-manifold_simiris.o `test -f 'source/frontend/impl/manifold_simiris.cc' || echo '$(srcdir)/'`source/frontend/impl/manifold_simiris.cc

manifold_simIris-manifold_simiris.obj: source/frontend/impl/manifold_simiris.cc
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -MT manifold_simIris-manifold_simiris.obj -MD -MP -MF $(DEPDIR)/manifold_simIris-manifold_simiris.Tpo -c -o manifold_simIris-manifold_simiris.obj `if test -f 'source/frontend/impl/manifold_simiris.cc'; then $(CYGPATH_W) 'source/frontend/impl/manifold_simiris.cc'; else $(CYGPATH_W) '$(srcdir)/source/frontend/impl/manifold_simiris.cc'; fi`
	mv -f $(DEPDIR)/manifold_simIris-manifold_simiris.Tpo $(DEPDIR)/manifold_simIris-manifold_simiris.Po
#	source='source/frontend/impl/manifold_simiris.cc' object='manifold_simIris-manifold_simiris.obj' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(manifold_simIris_CXXFLAGS) $(CXXFLAGS) -c -o manifold_simIris-manifold_simiris.obj `if test -f 'source/frontend/impl/manifold_simiris.cc'; then $(CYGPATH_W) 'source/frontend/impl/manifold_simiris.cc'; else $(CYGPATH_W) '$(srcdir)/source/frontend/impl/manifold_simiris.cc'; fi`

# This directory's subdirectories are mostly independent; you can cd
# into them and run `make' without going through this Makefile.
# To change the values of `make' variables: instead of editing Makefiles,
# (1) if the variable is set in `config.status', edit `config.status'
#     (which will cause the Makefiles to be regenerated when you run `make');
# (2) otherwise, pass the desired values on the `make' command line.
$(RECURSIVE_TARGETS):
	@failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	  || eval $$failcom; \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; test -z "$$fail"

$(RECURSIVE_CLEAN_TARGETS):
	@failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	dot_seen=no; \
	case "$@" in \
	  distclean-* | maintainer-clean-*) list='$(DIST_SUBDIRS)' ;; \
	  *) list='$(SUBDIRS)' ;; \
	esac; \
	rev=''; for subdir in $$list; do \
	  if test "$$subdir" = "."; then :; else \
	    rev="$$subdir $$rev"; \
	  fi; \
	done; \
	rev="$$rev ."; \
	target=`echo $@ | sed s/-recursive//`; \
	for subdir in $$rev; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	  || eval $$failcom; \
	done && test -z "$$fail"
tags-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) tags); \
	done
ctags-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) ctags); \
	done

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '{ files[$$0] = 1; nonempty = 1; } \
	      END { if (nonempty) { for (i in files) print i; }; }'`; \
	mkid -fID $$unique
tags: TAGS

TAGS: tags-recursive $(HEADERS) $(SOURCES) config.h.in $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	if ($(ETAGS) --etags-include --version) >/dev/null 2>&1; then \
	  include_option=--etags-include; \
	  empty_fix=.; \
	else \
	  include_option=--include; \
	  empty_fix=; \
	fi; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  if test "$$subdir" = .; then :; else \
	    test ! -f $$subdir/TAGS || \
	      tags="$$tags $$include_option=$$here/$$subdir/TAGS"; \
	  fi; \
	done; \
	list='$(SOURCES) $(HEADERS) config.h.in $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '{ files[$$0] = 1; nonempty = 1; } \
	      END { if (nonempty) { for (i in files) print i; }; }'`; \
	if test -z "$(ETAGS_ARGS)$$tags$$unique"; then :; else \
	  test -n "$$unique" || unique=$$empty_fix; \
	  $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	    $$tags $$unique; \
	fi
ctags: CTAGS
CTAGS: ctags-recursive $(HEADERS) $(SOURCES) config.h.in $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	list='$(SOURCES) $(HEADERS) config.h.in $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '{ files[$$0] = 1; nonempty = 1; } \
	      END { if (nonempty) { for (i in files) print i; }; }'`; \
	test -z "$(CTAGS_ARGS)$$tags$$unique" \
	  || $(CTAGS) $(CTAGSFLAGS) $(AM_CTAGSFLAGS) $(CTAGS_ARGS) \
	     $$tags $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH tags

distdir: $(DISTFILES)
	$(am__remove_distdir)
	test -d $(distdir) || mkdir $(distdir)
	@srcdirstrip=`echo "$(srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	list='$(DISTFILES)'; \
	  dist_files=`for file in $$list; do echo $$file; done | \
	  sed -e "s|^$$srcdirstrip/||;t" \
	      -e "s|^$$topsrcdirstrip/|$(top_builddir)/|;t"`; \
	case $$dist_files in \
	  */*) $(MKDIR_P) `echo "$$dist_files" | \
			   sed '/\//!d;s|^|$(distdir)/|;s,/[^/]*$$,,' | \
			   sort -u` ;; \
	esac; \
	for file in $$dist_files; do \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  if test -d $$d/$$file; then \
	    dir=`echo "/$$file" | sed -e 's,/[^/]*$$,,'`; \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -pR $(srcdir)/$$file $(distdir)$$dir || exit 1; \
	    fi; \
	    cp -pR $$d/$$file $(distdir)$$dir || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
	list='$(DIST_SUBDIRS)'; for subdir in $$list; do \
	  if test "$$subdir" = .; then :; else \
	    test -d "$(distdir)/$$subdir" \
	    || $(MKDIR_P) "$(distdir)/$$subdir" \
	    || exit 1; \
	    distdir=`$(am__cd) $(distdir) && pwd`; \
	    top_distdir=`$(am__cd) $(top_distdir) && pwd`; \
	    (cd $$subdir && \
	      $(MAKE) $(AM_MAKEFLAGS) \
	        top_distdir="$$top_distdir" \
	        distdir="$$distdir/$$subdir" \
		am__remove_distdir=: \
		am__skip_length_check=: \
	        distdir) \
	      || exit 1; \
	  fi; \
	done
	-find $(distdir) -type d ! -perm -777 -exec chmod a+rwx {} \; -o \
	  ! -type d ! -perm -444 -links 1 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -400 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -444 -exec $(install_sh) -c -m a+r {} {} \; \
	|| chmod -R a+r $(distdir)
dist-gzip: distdir
	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

dist-bzip2: distdir
	tardir=$(distdir) && $(am__tar) | bzip2 -9 -c >$(distdir).tar.bz2
	$(am__remove_distdir)

dist-lzma: distdir
	tardir=$(distdir) && $(am__tar) | lzma -9 -c >$(distdir).tar.lzma
	$(am__remove_distdir)

dist-tarZ: distdir
	tardir=$(distdir) && $(am__tar) | compress -c >$(distdir).tar.Z
	$(am__remove_distdir)

dist-shar: distdir
	shar $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).shar.gz
	$(am__remove_distdir)

dist-zip: distdir
	-rm -f $(distdir).zip
	zip -rq $(distdir).zip $(distdir)
	$(am__remove_distdir)

dist dist-all: distdir
	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

# This target untars the dist file and tries a VPATH configuration.  Then
# it guarantees that the distribution is self-contained by making another
# tarfile.
distcheck: dist
	case '$(DIST_ARCHIVES)' in \
	*.tar.gz*) \
	  GZIP=$(GZIP_ENV) gunzip -c $(distdir).tar.gz | $(am__untar) ;;\
	*.tar.bz2*) \
	  bunzip2 -c $(distdir).tar.bz2 | $(am__untar) ;;\
	*.tar.lzma*) \
	  unlzma -c $(distdir).tar.lzma | $(am__untar) ;;\
	*.tar.Z*) \
	  uncompress -c $(distdir).tar.Z | $(am__untar) ;;\
	*.shar.gz*) \
	  GZIP=$(GZIP_ENV) gunzip -c $(distdir).shar.gz | unshar ;;\
	*.zip*) \
	  unzip $(distdir).zip ;;\
	esac
	chmod -R a-w $(distdir); chmod a+w $(distdir)
	mkdir $(distdir)/_build
	mkdir $(distdir)/_inst
	chmod a-w $(distdir)
	dc_install_base=`$(am__cd) $(distdir)/_inst && pwd | sed -e 's,^[^:\\/]:[\\/],/,'` \
	  && dc_destdir="$${TMPDIR-/tmp}/am-dc-$$$$/" \
	  && cd $(distdir)/_build \
	  && ../configure --srcdir=.. --prefix="$$dc_install_base" \
	    $(DISTCHECK_CONFIGURE_FLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) dvi \
	  && $(MAKE) $(AM_MAKEFLAGS) check \
	  && $(MAKE) $(AM_MAKEFLAGS) install \
	  && $(MAKE) $(AM_MAKEFLAGS) installcheck \
	  && $(MAKE) $(AM_MAKEFLAGS) uninstall \
	  && $(MAKE) $(AM_MAKEFLAGS) distuninstallcheck_dir="$$dc_install_base" \
	        distuninstallcheck \
	  && chmod -R a-w "$$dc_install_base" \
	  && ({ \
	       (cd ../.. && umask 077 && mkdir "$$dc_destdir") \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" install \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" uninstall \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" \
	            distuninstallcheck_dir="$$dc_destdir" distuninstallcheck; \
	      } || { rm -rf "$$dc_destdir"; exit 1; }) \
	  && rm -rf "$$dc_destdir" \
	  && $(MAKE) $(AM_MAKEFLAGS) dist \
	  && rm -rf $(DIST_ARCHIVES) \
	  && $(MAKE) $(AM_MAKEFLAGS) distcleancheck
	$(am__remove_distdir)
	@(echo "$(distdir) archives ready for distribution: "; \
	  list='$(DIST_ARCHIVES)'; for i in $$list; do echo $$i; done) | \
	  sed -e 1h -e 1s/./=/g -e 1p -e 1x -e '$$p' -e '$$x'
distuninstallcheck:
	@cd $(distuninstallcheck_dir) \
	&& test `$(distuninstallcheck_listfiles) | wc -l` -le 1 \
	   || { echo "ERROR: files left after uninstall:" ; \
	        if test -n "$(DESTDIR)"; then \
	          echo "  (check DESTDIR support)"; \
	        fi ; \
	        $(distuninstallcheck_listfiles) ; \
	        exit 1; } >&2
distcleancheck: distclean
	@if test '$(srcdir)' = . ; then \
	  echo "ERROR: distcleancheck can only run from a VPATH build" ; \
	  exit 1 ; \
	fi
	@test `$(distcleancheck_listfiles) | wc -l` -eq 0 \
	  || { echo "ERROR: files left in build directory after distclean:" ; \
	       $(distcleancheck_listfiles) ; \
	       exit 1; } >&2
check-am: all-am
check: check-recursive
all-am: Makefile $(PROGRAMS) $(HEADERS) config.h
installdirs: installdirs-recursive
installdirs-am:
	for dir in "$(DESTDIR)$(bindir)"; do \
	  test -z "$$dir" || $(MKDIR_P) "$$dir"; \
	done
install: install-recursive
install-exec: install-exec-recursive
install-data: install-data-recursive
uninstall: uninstall-recursive

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-recursive
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:

distclean-generic:
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-recursive

clean-am: clean-binPROGRAMS clean-generic mostlyclean-am

distclean: distclean-recursive
	-rm -f $(am__CONFIG_DISTCLEAN_FILES)
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
distclean-am: clean-am distclean-compile distclean-generic \
	distclean-hdr distclean-tags

dvi: dvi-recursive

dvi-am:

html: html-recursive

info: info-recursive

info-am:

install-data-am:

install-dvi: install-dvi-recursive

install-exec-am: install-binPROGRAMS

install-html: install-html-recursive

install-info: install-info-recursive

install-man:

install-pdf: install-pdf-recursive

install-ps: install-ps-recursive

installcheck-am:

maintainer-clean: maintainer-clean-recursive
	-rm -f $(am__CONFIG_DISTCLEAN_FILES)
	-rm -rf $(top_srcdir)/autom4te.cache
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-recursive

mostlyclean-am: mostlyclean-compile mostlyclean-generic

pdf: pdf-recursive

pdf-am:

ps: ps-recursive

ps-am:

uninstall-am: uninstall-binPROGRAMS

.MAKE: $(RECURSIVE_CLEAN_TARGETS) $(RECURSIVE_TARGETS) install-am \
	install-strip

.PHONY: $(RECURSIVE_CLEAN_TARGETS) $(RECURSIVE_TARGETS) CTAGS GTAGS \
	all all-am am--refresh check check-am clean clean-binPROGRAMS \
	clean-generic ctags ctags-recursive dist dist-all dist-bzip2 \
	dist-gzip dist-lzma dist-shar dist-tarZ dist-zip distcheck \
	distclean distclean-compile distclean-generic distclean-hdr \
	distclean-tags distcleancheck distdir distuninstallcheck dvi \
	dvi-am html html-am info info-am install install-am \
	install-binPROGRAMS install-data install-data-am install-dvi \
	install-dvi-am install-exec install-exec-am install-html \
	install-html-am install-info install-info-am install-man \
	install-pdf install-pdf-am install-ps install-ps-am \
	install-strip installcheck installcheck-am installdirs \
	installdirs-am maintainer-clean maintainer-clean-generic \
	mostlyclean mostlyclean-compile mostlyclean-generic pdf pdf-am \
	ps ps-am tags tags-recursive uninstall uninstall-am \
	uninstall-binPROGRAMS


###Headers
#noinst_HEADERS = \
#    #		 source/frontend/impl/mesh.h \
#    #		 source/util/config_params.h \
#    #		 $(ALL_HDRS) $(ZESTO_HDRS)
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
