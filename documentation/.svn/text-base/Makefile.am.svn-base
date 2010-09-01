## ---
##
## $Id: Makefile.am,v 1.8 2005/06/28 12:10:45 nilu Exp $
##
## CppTest - A C++ Unit Testing Framework
## Copyright (c) 2003 Niklas Lundell
##
## ---
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the
## Free Software Foundation, Inc., 59 Temple Place - Suite 330,
## Boston, MA 02111-1307, USA.
##
## ---

doc_input = \
	main.dox 
	
EXTRA_DIST = \
	$(doc_input) \
	Iris_doxygen.config 

htmldir= 	./html
html_DATA= 	html/index.html

IMAGES=		images/*.png

all:
	doxygen Iris_doxygen.config
	cp -p $(IMAGES) html/

html/index.html:

install-data-hook:
	cp -p $(IMAGES) $(DESTDIR)$(htmldir)

uninstall-local:
	rm -rf $(DESTDIR)$(htmldir)

MOSTLYCLEANFILES =

CLEANFILES = \
	html/*

DISTCLEANFILES = \
	Makefile \
	html/*
	
MAINTAINERCLEANFILES = \
	Makefile.in

