###############################################################
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([Wired], [0.6.1], [wired@bloodshed.net])


# Auxiliary files.
AC_CONFIG_AUX_DIR([config])

# Needed for plugins
AC_DISABLE_STATIC
AC_ENABLE_SHARED
AC_PROG_LIBTOOL

# Automake.
AM_INIT_AUTOMAKE([1.8.2 foreign check-news dist-bzip2 no-dist-gzip])

# Look for a gnu compiler
if test $ac_cv_c_compiler_gnu = yes ; then
	AC_DEFINE([COMPILER_IS_GCC],1, [Set to 1 if the compile is GNU GCC.])
	GCC_MAJOR_VERSION=`gcc -dumpversion 2>/dev/null | awk -F. '{print $1}'`
	GCC_MINOR_VERSION=`gcc -dumpversion 2>/dev/null | awk -F. '{print $2}'`
	AC_DEFINE_UNQUOTED([GCC_MAJOR_VERSION],${GCC_MAJOR_VERSION}, [Major version of GCC or 3 otherwise.])
fi

#Get some libs versions
SAMPLERATE_VERSION=`pkg-config samplerate --modversion`
SNDFILE_VERSION=`pkg-config sndfile --modversion`
PORTAUDIO_VERSION=`pkg-config portaudio-2.0 --modversion`

# for autoheader .. investigate this further :)
AC_CONFIG_HEADERS(src/include/config.h)


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_GCC_TRADITIONAL
AC_LANG([C++])
AC_PROG_CXXCPP


####################
# checks for options
AC_ARG_ENABLE(debug,
	[  --enable-debug Enable or disable debug symbol generation (default disabled)],
	[enable_debug="$enableval"],
	[enable_debug="no"])

# can enable or disable Wired plugins
AC_ARG_ENABLE(plugins,
	[  --enable-plugins Enable or disable Wired plugins (default enabled)],
	[enable_plugins="$enableval"],
	[enable_plugins="yes"])
AM_CONDITIONAL(PLUGINS, test x$enable_plugins = xyes)

# can enable or disable Wired codecs
AC_ARG_ENABLE(codecs,
	[  --enable-codecs Enable or disable Wired codecs (default disabled)],
	[enable_codecs="$enableval"],
	[enable_codecs="no"])
AM_CONDITIONAL(CODECS, test x$enable_codecs = xyes)

AC_ARG_ENABLE(optimize,
	[  --enable-optimize Enable or disable compilation optimizations (default enabled)],
	[enable_optimize="$enableval"],
	[enable_optimize="yes"])

# can enable or disable portmidi integration
AC_ARG_ENABLE(portmidi,
	[  --disable-portmidi Enable or disable portmidi built-in (default enabled)],
	[enable_portmidi="$enableval"],
	[enable_portmidi="yes"])
AM_CONDITIONAL(PORTMIDI, test x$enable_portmidi = xyes)

######################
# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create], [PTHREAD='yes'], AC_MSG_ERROR([Couldn't find libpthread]))
AC_CHECK_LIB([sndfile], [sf_open], [LIBSNDFILE='yes'], AC_MSG_ERROR([Couldn't find libsndfile]))
AC_CHECK_LIB([xml2], [xmlParseMemory], [ XML2F='yes' ], AC_MSG_ERROR([Couldn't find libxml2]))
AC_CHECK_LIB([samplerate], [src_new], [SRC='yes' ], AC_MSG_ERROR([Couldn't find libsamplerate]))
AC_CHECK_LIB([portaudio], [Pa_GetDeviceInfo], [SRC='yes' ], AC_MSG_ERROR([Couldn't find libportaudio]))

# If we don't use our portmidi built-in
if test "$enable_portmidi" = no; then
   AC_CHECK_LIB([portmidi], [Pm_Initialize], [PORTMIDI='yes' ], AC_MSG_ERROR([Couldn't find libportmidi]))
fi

# libs used in plugins
if test x"$enable_plugins" = xyes; then
   AC_CHECK_LIB([SoundTouch], [soundtouch_ac_test], [SNDTOUC='yes'], AC_MSG_ERROR([Couldn't find lib SoundTouch]))
fi

# libs used in codecs
if test x"$enable_codecs" = xyes; then
   AC_CHECK_LIB([FLAC++], [FLAC__file_decoder_init], [FLAC='yes'], AC_MSG_ERROR([Couldn't find libflac++]))
   AC_CHECK_LIB([vorbis], [vorbis_info_init], [VORBIS='yes'], AC_MSG_ERROR([Couldn't find libvorbis]))
fi

#Checking for wxWidgets
WXWIDGET_VERSION=`wx-config --version`
AM_OPTIONS_WXCONFIG
reqwx=2.6.0
AM_PATH_WXCONFIG($reqwx, wxWin=1)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
                wxWidgets must be installed on your system
                but wx-config script couldn't be found.

		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
else
	AC_PATH_PROG(WX_CONFIG, wx-config, no)
fi

if test "$PORTAUDIO_VERSION" != 19; then
	AC_MSG_ERROR([
                Portaudio v19 must be installed on your system.
		])
fi   

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"

#Checks with pkg-config MACRO :
PKG_CHECK_MODULES(SAMPLERATE, samplerate >= 0.0.15,
            ac_cv_samplerate=1, ac_cv_samplerate=0)

AC_DEFINE_UNQUOTED([HAVE_SAMPLERATE],${ac_cv_samplerate},
            [Set to 1 if you have libsamplerate.])

AC_SUBST(SAMPLERATE_CFLAGS)
AC_SUBST(SAMPLERATE_LIBS)

#########################
# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC

# std headers
AC_CHECK_HEADERS([fcntl.h netinet/in.h stdlib.h string.h unistd.h math.h sys/types.h sys/stat.h dlfcn.h stdint.h stdio.h dirent.h cmath byteswap.h list.h errno.h assert.h io.h direct.h libintl.h])

# libs headers
AC_CHECK_HEADERS([sndfile.h samplerate.h])

# If we don't use our portmidi built-in
if test "$enable_portmidi" = no; then
   AC_CHECK_HEADERS([portmidi.h])
fi

# headers used in plugins
if test x"$enable_plugins" = xyes; then
   AC_CHECK_HEADERS([soundtouch/SoundTouch.h])
fi

# headers used in codecs
if test x"$enable_codecs" = xyes; then
   AC_CHECK_HEADERS([FLAC++/all.h vorbis/vorbisfile.h])
fi


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
AC_CHECK_FUNCS([floor ftime getcwd memmove memset pow select sqrt strdup memset open])


# check prefix var
AC_PREFIX_DEFAULT(/usr/local)
if [[ $prefix = "NONE" ]] ; then
	prefix="/usr/local"
        AC_DEFINE(INSTALL_PREFIX, "/usr/local",
        [define as prefix where Wired is installed])
else
        AC_DEFINE_UNQUOTED(INSTALL_PREFIX, "$prefix", 
	[define as prefix where Wired is installed])
fi

if [[ $exec_prefix = "NONE" ]] ; then
exec_prefix=`eval echo ${prefix}`
fi

# For include
includedir="`eval echo ${includedir}/wired`"
INCLUDE_DIR="${includedir}"
AC_DEFINE_UNQUOTED(INCLUDEDIR, "$INCLUDE_DIR", [define as includes dir])

# For libdir
LIB_DIR="`eval echo ${libdir}/wired`"
AC_DEFINE_UNQUOTED(LIB_DIR, "$LIB_DIR", [define as libraries dir])

# For sysconfdir
sysconfdir="`eval echo ${sysconfdir}/wired`"
SYSCONF_DIR="${sysconfdir}"
AC_DEFINE_UNQUOTED(SYSCONF_DIR, "$SYSCONF_DIR", [define as sysconf dir])

# For share
# ${datadir}
DATA_DIR="`eval echo ${prefix}/share`"
AC_DEFINE_UNQUOTED(DATA_DIR, "$DATA_DIR", [define as datas dir])

# for locale
PACKAGE_LOCALE_DIR="`eval echo ${datadir}/locale`"
AC_DEFINE_UNQUOTED(PACKAGE_LOCALE_DIR, "$PACKAGE_LOCALE_DIR", [define the locale dir path])


##################
# Checking Options
AC_MSG_CHECKING([for enable-optimize])
if test x"$enable_optimize" = xyes;
then
 CFGOPTIMIZE=Yes
 CXXFLAGS="-Os"
else
 CFGOPTIMIZE=No
 CXXFLAGS=""
fi
AC_MSG_RESULT([$CFGDEBUG])

# debugging
AC_MSG_CHECKING([for enable-debug])
if test x"$enable_debug" = xyes;
then
 CFGDEBUG=Yes
 CXXFLAGS="${CXXFLAGS} -g"
else
 CFGDEBUG=No
fi
AC_MSG_RESULT([$CFGDEBUG])


#Some locale things
ALL_LINGUAS="fr_FR it_IT zh_TW.Big5"
AM_GNU_GETTEXT([use-libtool])
AM_GNU_GETTEXT_VERSION([0.14.4])

AC_CONFIG_FILES([Makefile
		po/Makefile.in
		intl/Makefile
		src/Makefile
		src/save/Makefile
		src/audio/Makefile
		src/editmidi/Makefile
		src/engine/Makefile
		src/fileloader/Makefile
		src/gui/Makefile
		src/midi/Makefile
		src/mixer/Makefile
		src/plugins/Makefile
		src/redist/Makefile
		src/sequencer/Makefile
		src/dssi/Makefile
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
		src/data/dssi/Makefile
		src/conf/Makefile
		src/xml/Makefile
		src/samplerate/Makefile
		src/codec/Makefile
		src/wiredvideo/Makefile])

# Add Plugins files
if test "$enable_plugins" = yes; then
AC_CONFIG_FILES([src/plugins/beatbox/Makefile
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
		src/data/plugins/akaisampler/Makefile
		src/data/plugins/beatbox/Makefile
		src/data/plugins/compressor/Makefile
		src/data/plugins/crusher/Makefile
		src/data/plugins/delay/Makefile
		src/data/plugins/filter/Makefile
		src/data/plugins/loopsampler/Makefile
		src/data/plugins/reverb/Makefile
		src/data/plugins/wahwah/Makefile
		src/data/plugins/chorus/Makefile])
fi

# Add Codecs files
if test "$enable_codecs" = yes; then
AC_CONFIG_FILES([src/codec/wiredcodec/Makefile
		src/codec/wiredcodec/mpeg/Makefile
		src/codec/wiredcodec/flac/Makefile
		src/codec/wiredcodec/vorbis/Makefile
])
fi

# Add Portmidi files
if test "$enable_portmidi" = yes; then
   AC_CONFIG_FILES([src/midi/portmidi/Makefile
		    src/midi/portmidi/porttime/Makefile])
fi

AC_OUTPUT


AC_MSG_RESULT([
-=-=-=-=-=-=-=-=-=-= Configuration Complete =-=-=-=-=-=-=-=-=-=-=-

  Configuration summary :

    Version : ..................... ${VERSION}])
AC_MSG_RESULT([    Enable debugging : ............ ${CFGDEBUG}])
AC_MSG_RESULT([    Enable optimizations : ........ ${CFGOPTIMIZE}
])

if test x$ac_cv_c_compiler_gnu = xyes ; then
AC_MSG_RESULT([
   Tools :

     Compiler is GCC : ............. ${ac_cv_c_compiler_gnu}
     GCC major version : ........... ${GCC_MAJOR_VERSION}
     GCC minor version : ........... ${GCC_MINOR_VERSION}
])
fi

AC_MSG_RESULT([
  Dependencies required :

    wxWidgets : ................... ${WXWIDGET_VERSION}
    libportaudio : ................ ${PORTAUDIO_VERSION}
    libsamplerate : ............... ${SAMPLERATE_VERSION}
    libsndfile : .................. ${SNDFILE_VERSION}
 ])

AC_MSG_RESULT([  Installation directories :
    Wired plugins directory: .............. ${LIB_DIR}
    Wired binary directory : .............. ${prefix}/bin
    Wired data directory   : .............. ${DATA_DIR}
    Wired conf directory   : .............. ${SYSCONF_DIR}
])

if test "$enable_codecs" = yes; then
AC_MSG_RESULT([
    Wired codecs : MPEG, FLAC, Vorbis
        WARNING : Codecs support are in a TESTING stage.
		  Wired could be UNSTABLE.
])
fi
