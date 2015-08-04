# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Requires Autoconf 2.62 or above
AC_PREREQ(2.62)

# Initialization
AC_INIT([xprintidle], [0.2], [henoch@dtek.chalmers.se])
AM_INIT_AUTOMAKE([dist-bzip2 -Wall foreign])

# Set src dir
AC_CONFIG_SRCDIR([xprintidle.c])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Check for libraries
PKG_CHECK_MODULES([X11],  [x11])
PKG_CHECK_MODULES([XEXT], [xext])
PKG_CHECK_MODULES([XSS],  [xscrnsaver])

# Output
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
