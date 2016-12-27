dnl Process this file with autoconf to produce a configure script.

AC_INIT([libcerializer], [1.1.0])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_PROG_LIBTOOL

AC_CHECK_LIB(rt, clock_gettime, [], [])

AC_PROG_CC
AC_PROG_RANLIB

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES(Makefile src/Makefile tools/Makefile examples/Makefile)
AC_OUTPUT

