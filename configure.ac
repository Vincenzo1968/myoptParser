#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(myoptParser, 1.0, vincenzo.locicero@libero.it)

#AM_ENABLE_MULTILIB(, ..)
 
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([gnu -Wall -Werror])
LT_INIT([disable-shared])
AC_CONFIG_SRCDIR([src/myoptSymtab.h])
AC_CONFIG_HEADERS([config.h])

AM_MAINTAINER_MODE

#AM_ENABLE_MULTILIB(, ..)

# Checks for programs.
#AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([malloc.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
