# Process this file with automake to produce Makefile.in (in this,
# and all subdirectories).
# Makefile for the top-level directory of GNU hello.
#
# Copyright 1997, 1998, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2013,
# 2014, 2015 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Find gnulib headers.
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -Ilib -I$(top_srcdir)/lib -Isrc -I$(top_srcdir)/src

# Additional files to distribute.
EXTRA_DIST = doc/xprintidle.x

# Subdirectories to descend into.
SUBDIRS = po

bin_PROGRAMS = xprintidle

xprintidle_SOURCES = src/xprintidle.c src/system.h

xprintidle_CFLAGS = \
	$(XEXT_CFLAGS) \
	$(XSS_CFLAGS) \
	$(X11_CFLAGS)

xprintidle_LDADD = \
	$(LIBINTL) \
	$(top_builddir)/lib/lib$(PACKAGE).a \
	$(XEXT_LIBS) \
	$(XSS_LIBS) \
	$(X11_LIBS) 

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

CLEANFILES =

xprintidle.1: xprintidle
	$(HELP2MAN) --include=$(top_srcdir)/doc/xprintidle.x \
				$(top_builddir)/xprintidle -o $@-t
	chmod a=r $@-t
	mv -f $@-t $@

CLEANFILES += $(man_MANS)

noinst_LIBRARIES =
MOSTLYCLEANFILES =
MOSTLYCLEANDIRS =
BUILT_SOURCES =

LOG_COMPILER = $(SHELL)

# Verify that all source files using _() are listed in po/POTFILES.in.
# The idea is to run this before making pretests, as well as official
# releases, so that translators will be sure to have all the messages.
# (From coreutils.)
po-check:
	if test -f po/POTFILES.in; then										\
	  grep -E -v '^(#|$$)' po/POTFILES.in								\
	    | grep -v '^src/false\.c$$' | sort > $@-1;						\
	  files=;															\
	  for file in $$($(CVS_LIST_EXCEPT)) `find * -name '*.[ch]'`; do	\
	    case $$file in													\
	    gnulib/* | doc/*) continue;;									\
	    esac;															\
	    case $$file in													\
	    *.[ch])															\
	      base=`expr " $$file" : ' \(.*\)\..'`;							\
	      { test -f $$base.l || test -f $$base.y; } && continue;;		\
	    esac;															\
	    files="$$files $$file";											\
	  done;																\
	  grep -E -l '\b(N?_|gettext *)\([^)"]*("|$$)' $$files				\
	    | sort -u > $@-2;												\
	  diff -u $@-1 $@-2 || exit 1;										\
	  rm -f $@-1 $@-2;													\
	fi

include $(top_srcdir)/lib/local.mk
include $(top_srcdir)/doc/local.mk
