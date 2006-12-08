###############################################################
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([Wired], [0.4alpha], [wired@bloodshed.net])


# Auxiliary files.
AC_CONFIG_AUX_DIR([config])

# Needed for plugins
AC_PROG_LIBTOOL

# Automake.
AM_INIT_AUTOMAKE([1.8.2 foreign check-news dist-bzip2 no-dist-gzip])

# Look for a gnu compiler
if test $ac_cv_c_compiler_gnu = yes ; then
	AC_DEFINE([COMPILER_IS_GCC],1, [Set to 1 if the compile is GNU GCC.])
	GCC_MAJOR_VERSION=`$CC -dumpversion | sed "s/\..*//"`
	AC_DEFINE_UNQUOTED([GCC_MAJOR_VERSION],${GCC_MAJOR_VERSION}, [Major version of GCC or 3 otherwise.])
fi

#Get some libs versions
SAMPLERATE_VERSION=`pkg-config samplerate --modversion`
SNDFILE_VERSION=`pkg-config sndfile --modversion`

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

# can enable or disable portaudio integration
# Portaudio options
AC_ARG_ENABLE(portaudio,
	[  --enable-portaudio Enable or disable portaudio v19 built-in (default enabled)],
	[enable_portaudio="$enableval"],
	[enable_portaudio="yes"])
AM_CONDITIONAL(PORTAUDIO, test x$enable_portaudio = xyes)

AC_ARG_WITH(alsa, 
            [  --with-alsa (default=auto)],
            with_alsa=$withval, with_alsa="yes")

AC_ARG_WITH(jack, 
            [  --with-jack (default=auto)],
            with_jack=$withval, with_jack="yes")

AC_ARG_WITH(oss, 
            [  --with-oss (default=yes)],
            with_oss=$withval, with_oss="yes")

# Portaudio configuration
if test "$enable_portaudio" = yes; then
   AC_CONFIG_SUBDIRS([src/portaudio])
fi

######################
# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create], [PTHREAD='yes'], AC_MSG_ERROR([Couldn't find libpthread]))
AC_CHECK_LIB([sndfile], [sf_open], [LIBSNDFILE='yes'], AC_MSG_ERROR([Couldn't find libsndfile]))
AC_CHECK_LIB([xml2], [xmlParseMemory], [ XML2F='yes' ], AC_MSG_ERROR([Couldn't find libxml2]))
AC_CHECK_LIB([samplerate], [src_new], [SRC='yes' ], AC_MSG_ERROR([Couldn't find libsamplerate]))

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
		wxWidgets must be installed on your system.

		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
else
	AC_PATH_PROG(WX_CONFIG, wx-config, no)
fi

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"

#Checks for libsamplerate
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
		Makefile
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
		src/midi/portmidi/Makefile
		src/midi/portmidi/porttime/Makefile
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
		src/data/ihm/settings/Makefile
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
		src/samplerate/Makefile
		src/codec/Makefile
		src/codec/wiredcodec/Makefile
		src/codec/wiredcodec/mpeg/Makefile
		src/codec/wiredcodec/flac/Makefile
		src/codec/wiredcodec/vorbis/Makefile
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
AC_MSG_RESULT([
   Tools :

     Compiler is GCC : ............. ${ac_cv_c_compiler_gnu}
     GCC major version : ........... ${GCC_MAJOR_VERSION}
])
fi

# these vars are only used for output
if test x"$with_jack" = xyes; then
   with_jack=yes
else
   with_jack=no
fi

if test x"$with_oss" = xyes; then
   with_oss=yes
else
   with_oss=no
fi

if test x"$with_alsa" = xyes; then
   with_alsa=yes
else
   with_alsa=no
fi

if test x"$enable_portaudio" = xyes; then
AC_MSG_RESULT([
   Portaudio options :

    Jack : ........................ ${with_jack}
    OSS : ......................... ${with_oss}
    ALSA : ........................ ${with_alsa}

WARNING: If you already have Portaudio in your System, it may be replaced by this one !
 ])
fi

AC_MSG_RESULT([
  Extra tools required :

    wxWidgets : ................... ${WXWIDGET_VERSION}
    libsamplerate : ............... ${SAMPLERATE_VERSION}
    libsndfile : .................. ${SNDFILE_VERSION}
 ])

AC_MSG_RESULT([  Installation directories :
    Wired plugins directory: .............. ${LIB_DIR}
    Wired binary directory : .............. ${prefix}/bin
    Wired data directory   : .............. ${DATA_DIR}
    Wired conf directory   : .............. ${SYSCONF_DIR}
])
