#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([Wired], [0.2.2], [wired@bloodshed.net])

# Auxiliary files.
#AC_CONFIG_AUX_DIR([config])
#AC_CONFIG_FILES([config/Makefile])

# Needed for plugins
AM_PROG_LIBTOOL

# Automake.
AM_INIT_AUTOMAKE([1.8.2 foreign check-news dist-bzip2 no-dist-gzip])

# Look for a C++ compiler.
AC_LANG([C++])

# Look for a gnu compiler
if test $ac_cv_c_compiler_gnu = yes ; then
	AC_DEFINE([COMPILER_IS_GCC],1, [Set to 1 if the compile is GNU GCC.])
	GCC_MAJOR_VERSION=`$CC -dumpversion | sed "s/\..*//"`
	AC_DEFINE_UNQUOTED([GCC_MAJOR_VERSION],${GCC_MAJOR_VERSION}, [Major version of GCC or 3 otherwise.])
fi

#Get some libs versions

GTK_VERSION=`pkg-config gtk+-2.0 --modversion`
WXWIDGET_VERSION=`wx-config --version`
SAMPLERATE_VERSION=`pkg-config samplerate --modversion`
SNDFILE_VERSION=`pkg-config sndfile --modversion`

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
AC_CHECK_LIB([asound], [snd_asoundlib_version], [ASOUND='yes'], AC_MSG_ERROR([Couldn't find libasound]))
# FIXME: Replace `main' with a function in `-lm':
#AC_CHECK_LIB([m], [main])
#AC_CHECK_LIB([portaudio], [Pa_Sleep], [PORTAUDIO='yes'], AC_MSG_ERROR([Couldn't find portaudio]))
AC_CHECK_LIB([pthread], [pthread_create], [PTHREAD='yes'], AC_MSG_ERROR([Couldn't find libpthread]))
AC_CHECK_LIB([sndfile], [sf_open], [LIBSNDFILE='yes'], AC_MSG_ERROR([Couldn't find libsndfile ]))
AC_CHECK_LIB([SoundTouch], [soundtouch_ac_test], [SNDTOUC='yes'], AC_MSG_ERROR([Couldn't find lib SoundTouch ]))
#AC_CHECK_LIB([ltdl], [lt_dlmutex_lock], [LTDL='yes'], AC_MSG_ERROR([Couldn't find lib libtool or ltdl ]))
#AC_CHECK_LIB([FLAC++], [FLAC__file_decoder_init], [FLAC='yes'], AC_MSG_ERROR([Couldn't find lib FLAC++ ]))
#AC_CHECK_LIB([vorbis], [vorbis_info_init], [VORBIS='yes'], AC_MSG_ERROR([Couldn't find lib VorbisFile ]))
AC_CHECK_LIB([xml2], [xmlParseMemory], [ XML2F='yes' ], AC_MSG_ERROR([Couldn't find libxml2 ]))
AC_CHECK_LIB([samplerate], [src_new], [SRC='yes' ], AC_MSG_ERROR([Couldn't find libsamplerate]))

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h netinet/in.h stdlib.h string.h unistd.h math.h sys/types.h sys/stat.h dlfcn.h stdint.h stdio.h dirent.h alsa/seq_event.h cmath wx/wx.h sndfile.h portaudio.h byteswap.h list.h alsa/asoundlib.h errno.h soundtouch/SoundTouch.h assert.h samplerate.h io.h direct.h])

#Checks for libsamplerate
PKG_CHECK_MODULES(SAMPLERATE, samplerate >= 0.0.15,
            ac_cv_samplerate=1, ac_cv_samplerate=0)

AC_DEFINE_UNQUOTED([HAVE_SAMPLERATE],${ac_cv_samplerate},
            [Set to 1 if you have libsamplerate.])

AC_SUBST(SAMPLERATE_CFLAGS)
AC_SUBST(SAMPLERATE_LIBS)


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_VOLATILE

# checks for options
AC_ARG_ENABLE(dssi, 
	[  --enable-dssi Enable or disable DSSI plugins support (default disabled)])

AC_ARG_ENABLE(debug, 
	[  --enable-debug Enable or disable debug symbol generation (default disabled)])
	

AC_ARG_ENABLE(optimize, 
	[  --disable-optimize Enable or disable compilation optimizations (default enabled)],disable_optimize=yes, disable_optimize=no)

AC_ARG_ENABLE(purify, 
	[  --enable-purify Enable building process with purify (default disabled)])

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([floor ftime getcwd memmove memset pow select sqrt strdup bzero open])

AC_PREFIX_DEFAULT(/usr/local)
if [[ $prefix = "NONE" ]] ; then
        AC_DEFINE(INSTALL_PREFIX, "/usr/local",
        [define as prefix where Wired is installed])
else
        AC_DEFINE_UNQUOTED(INSTALL_PREFIX, "$prefix")
fi


if test x$enable_dssi = xyes;
then
 CFGDSSI=Yes
 OPTION_DSSI="DSSI_ENABLED"
 ALL_SRC="${ALL_SRC} src/dssi/Makefile"
else
 CFGDSSI=No
 OPTION_DSSI="DSSI_DISABLED"
fi

if test x$enable_purify = xyes;
then
 CXX='g++' 
 CC='gcc'
fi

 CXXFLAGS="-O3"

if test "$disable_optimize" = "yes";
then
 CFGOPTIMIZE=No
 CXXFLAGS=""
else
 CFGOPTIMIZE=Yes
fi

if test x$enable_debug = xyes;
then
 CFGDEBUG=Yes
 CXXFLAGS="${CXXFLAGS} -g"
else
 CFGDEBUG=No
fi

#Some locale things

ALL_LINGUAS="fr" 
AM_GNU_GETTEXT 

if test "x${prefix}" = "xNONE"; then 
 AC_DEFINE(PACKAGE_LOCALE_DIR, "/usr/local/share/wired/locale") 
else 
 AC_DEFINE(PACKAGE_LOCALE_DIR, "${prefix}/share/wired/locale") 
fi 

AC_CONFIG_FILES([Makefile po/Makefile.in
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
		src/data/dssi/Makefile		
		src/conf/Makefile
		src/xml/Makefile
		src/dssi/Makefile
		src/samplerate/Makefile
		src/codec/Makefile
		src/codec/wiredcodec/Makefile
		src/codec/wiredcodec/mpeg/Makefile
		src/wiredvideo/Makefile])

AC_OUTPUT

AC_MSG_RESULT([
-=-=-=-=-=-=-=-=-=-= Configuration Complete =-=-=-=-=-=-=-=-=-=-=-

  Configuration summary :

    Version : ..................... ${VERSION}])
AC_MSG_RESULT([    Enable debugging : ............ ${CFGDEBUG}])
AC_MSG_RESULT([    Enable optimizations : ........ ${CFGOPTIMIZE}
])

if test x$ac_cv_c_compiler_gnu = xyes ; then
	echo -e "  Tools :\n"
	echo "    Compiler is GCC : ............. ${ac_cv_c_compiler_gnu}"
	echo "    GCC major version : ........... ${GCC_MAJOR_VERSION}"
	fi

AC_MSG_RESULT([
  Extra tools required :

    GTK : ......................... ${GTK_VERSION}
    wxWidgets : ................... ${WXWIDGET_VERSION}
    libsamplerate : ............... ${SAMPLERATE_VERSION}
    libsndfile : .................. ${SNDFILE_VERSION}
 ])
#    Portaudio : .................... ${ac_cv_fftw3}])
#  Soundtouch : .................... ${ac_cv_fftw3}])


AC_MSG_RESULT([  Installation directories :

    Library directory : ........... ${prefix}/lib
    Program directory : ........... ${prefix}/bin
    Data directory : .............. ${prefix}/shared/wired
])
