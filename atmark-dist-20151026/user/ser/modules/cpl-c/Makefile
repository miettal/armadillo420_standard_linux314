# $Id: Makefile,v 1.7 2003/09/04 14:12:56 bogdan Exp $
#
# example module makefile
#
# 
# WARNING: do not run this directly, it should be run by the master Makefile

include ../../Makefile.defs
auto_gen=
NAME=cpl-c.so

DEFS +=-I/usr/include/libxml2

LIBS= -L$(LOCALBASE)/lib -L/usr/pkg/lib -lxml2

include ../../Makefile.modules
