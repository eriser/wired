#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([Wired], [0.11], [wired@bloodshed.net])

# Auxiliary files.
#AC_CONFIG_AUX_DIR([config])
#AC_CONFIG_FILES([config/Makefile])

# Needed for plugins
AM_PROG_LIBTOOL

# Automake.
AM_INIT_AUTOMAKE([1.8.2 foreign check-news dist-bzip2 no-dist-gzip])

# Look for a C++ compiler.
AC_LANG([C++])

# Accelerate the compilation.
if test "$GXX" = yes; then
  CXXFLAGS="-ggdb"
fi

# for autoheader .. investigate this further :)
AC_CONFIG_HEADER(src/include/config.h)

# CXX_WARNINGS([[-Wall],
#   	        [-W],
#               [-Wcast-align],
#               [-Wcast-qual],
#               [-Wformat],
#               [-Wmissing-prototypes],
#               [-Wshadow],
#               [-Wstrict-prototypes],
#               [-Wwrite-strings],
#               [-Wbad-function-cast],
#               [-Wmissing-declarations],
#               [-Wnested-externs]])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lasound':
#AC_CHECK_LIB([asound], [main])
# FIXME: Replace `main' with a function in `-lm':
#AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lportaudio':
#AC_CHECK_LIB([portaudio], [main])
# FIXME: Replace `main' with a function in `-lportmidi':
# FIXME: Replace `main' with a function in `-lpthread':
#AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lsndfile':
#AC_CHECK_LIB([sndfile], [main])
# FIXME: Replace `main' with a function in `-lsndfile':
#AC_CHECK_LIB([SoundTouch], [main])
#AC_CHECK_LIB([ltdl], [lt_dlopen])

AC_CHECK_LIB([xml2], [xmlParseMemory], [ XML2F='yes' ], AC_MSG_ERROR([Couldn't find libxml2 ]))

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h netinet/in.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([floor ftime getcwd memmove memset pow select sqrt strdup])

AC_PREFIX_DEFAULT(/usr/local)
if [[ $prefix = "NONE" ]] ; then
        AC_DEFINE(INSTALL_PREFIX, "/usr/local",
        [define as prefix where Wired is installed])
else
        AC_DEFINE_UNQUOTED(INSTALL_PREFIX, "$prefix")
fi

AC_CONFIG_FILES([Makefile
		src/Makefile
          	src/audio/Makefile
		src/editmidi/Makefile
		src/engine/Makefile
		src/fileloader/Makefile
		src/gui/Makefile
		src/midi/Makefile
		src/midi/portmidi/Makefile
		src/midi/portmidi/porttime/Makefile
		src/mixer/Makefile
		src/plugins/Makefile
		src/plugins/beatbox/Makefile
		src/plugins/compressor/Makefile
		src/plugins/crusher/Makefile
		src/plugins/delay/Makefile
		src/plugins/filter/Makefile
		src/plugins/loop_sampler/Makefile
		src/plugins/akai_sampler/Makefile
		src/plugins/reverb/Makefile
		src/plugins/synth/Makefile
		src/plugins/chorus/Makefile
		src/plugins/wahwah/Makefile
		src/plugins/test/Makefile
		src/redist/Makefile
		src/sequencer/Makefile
		src/undo/Makefile
		src/libs/Makefile
                src/libs/WiredWidgets/Makefile
                src/libs/WiredWidgets/src/Makefile
                src/libs/WiredAkai/Makefile
		src/libs/WiredAkai/src/Makefile
		src/data/Makefile
		src/data/ihm/Makefile	
		src/data/ihm/mixer/Makefile
		src/data/ihm/opt/Makefile
		src/data/ihm/player/Makefile
		src/data/ihm/seqtrack/Makefile
		src/data/ihm/splash/Makefile
		src/data/ihm/toolbar/Makefile
		src/data/ihm/widgets/Makefile
		src/data/plugins/Makefile
		src/data/plugins/akaisampler/Makefile
		src/data/plugins/beatbox/Makefile
		src/data/plugins/compressor/Makefile
		src/data/plugins/crusher/Makefile
		src/data/plugins/delay/Makefile
		src/data/plugins/filter/Makefile
		src/data/plugins/loopsampler/Makefile
		src/data/plugins/reverb/Makefile
		src/data/plugins/wahwah/Makefile
		src/data/plugins/chorus/Makefile
		src/conf/Makefile
		src/xml/Makefile])

AC_OUTPUT
