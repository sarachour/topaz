AC_INIT([Body Tracking Benchmark], [1.0], [cbienia@cs.princeton.edu])
AC_CONFIG_SRCDIR([TrackingBenchmark/main.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror])

# Arguments
AC_ARG_ENABLE([gprof], AC_HELP_STRING([--enable-gprof], [add profiling code for gprof @<:@default=no@:>@]), [], [enable_gprof=no])

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET


AC_CHECK_HEADERS([stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_C_CONST
AC_C_INLINE

TOPAZ_ROOT="/mnt/hgfs/sachour/Documents/git/research/topaz"
TOPAZ_INCLUDE=$TOPAZ_ROOT/include
TOPAZ_LIB=$TOPAZ_ROOT/lib

# Checks for library functions.
#AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset sqrt])

# Process arguments
if test "x$enable_gprof" == "xyes"; then
  CXXFLAGS="$CXXFLAGS -pg"
fi


# Extra arguments
CXXFLAGS="$CXXFLAGS -Wall -I$TOPAZ_INCLUDE"

# Never warn about unknown pragmas
CXXFLAGS="$CXXFLAGS -Wno-unknown-pragmas"

LDFLAGS="$LDFLAGS -L$TOPAZ_LIB -ltopaz -lglpk"
# Make sure VPATH is always defined
#if test "x$VPATH" == "x"; then
#  VPATH="."
#  AC_SUBST([VPATH])
#fi

# Finish up
AC_CONFIG_FILES([Makefile
                 FlexImageLib/Makefile
                 TrackingBenchmark/Makefile])
AC_OUTPUT
