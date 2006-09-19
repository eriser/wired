#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([Wired], [0.3], [wired@bloodshed.net])


# Auxiliary files.
AC_CONFIG_AUX_DIR([config])

# Needed for plugins
AM_PROG_LIBTOOL

# Automake.
AM_INIT_AUTOMAKE([1.8.2 foreign check-news dist-bzip2 no-dist-gzip])

# Look for a gnu compiler
if test $ac_cv_c_compiler_gnu = yes ; then
	AC_DEFINE([COMPILER_IS_GCC],1, [Set to 1 if the compile is GNU GCC.])
	GCC_MAJOR_VERSION=`$CC -dumpversion | sed "s/\..*//"`
	AC_DEFINE_UNQUOTED([GCC_MAJOR_VERSION],${GCC_MAJOR_VERSION}, [Major version of GCC or 3 otherwise.])
fi

#Get some libs versions
WXWIDGET_VERSION=`wx-config --version`
SAMPLERATE_VERSION=`pkg-config samplerate --modversion`
SNDFILE_VERSION=`pkg-config sndfile --modversion`


# for autoheader .. investigate this further :)
AC_CONFIG_HEADER(src/include/config.h)


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_GCC_TRADITIONAL
AC_LANG([C++])
AC_PROG_CXXCPP


# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create], [PTHREAD='yes'], AC_MSG_ERROR([Couldn't find libpthread]))
AC_CHECK_LIB([sndfile], [sf_open], [LIBSNDFILE='yes'], AC_MSG_ERROR([Couldn't find libsndfile]))
AC_CHECK_LIB([SoundTouch], [soundtouch_ac_test], [SNDTOUC='yes'], AC_MSG_ERROR([Couldn't find lib SoundTouch]))
AC_CHECK_LIB([FLAC++], [FLAC__file_decoder_init], [FLAC='yes'], AC_MSG_ERROR([Couldn't find libflac++]))
AC_CHECK_LIB([vorbis], [vorbis_info_init], [VORBIS='yes'], AC_MSG_ERROR([Couldn't find libvorbis]))
AC_CHECK_LIB([xml2], [xmlParseMemory], [ XML2F='yes' ], AC_MSG_ERROR([Couldn't find libxml2]))
AC_CHECK_LIB([samplerate], [src_new], [SRC='yes' ], AC_MSG_ERROR([Couldn't find libsamplerate]))



#Checking for wxWidgets
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

	if test `${WX_CONFIG} --unicode` ; then
	   	AC_MSG_ERROR([
		wxWidgets must be installed with unicode support.

		Please try wx-config --unicode=yes.
		])
	fi

fi

CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
LIBS="$LIBS $WX_LIBS"

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h netinet/in.h stdlib.h string.h unistd.h math.h sys/types.h sys/stat.h dlfcn.h stdint.h stdio.h dirent.h cmath wx/wx.h sndfile.h byteswap.h list.h errno.h soundtouch/SoundTouch.h assert.h samplerate.h io.h direct.h libintl.h FLAC++/all.h vorbis/vorbisfile.h])

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
AC_ARG_ENABLE(debug,
	[  --enable-debug Enable or disable debug symbol generation (default disabled)],
	[enable_debug="$enableval"],
	[enable_debug="no"])

AC_ARG_ENABLE(optimize,
	[  --enable-optimize Enable or disable compilation optimizations (default enabled)],
	[enable_optimize="$enableval"],
	[enable_optimize="yes"])


AC_ARG_ENABLE(jack,
	[  --enable-jack Enable or disable jack support (default disabled)],
	[enable_jack="$enableval"],
	[enable_jack="no"])


# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([floor ftime getcwd memmove memset pow select sqrt strdup memset open])

AC_PREFIX_DEFAULT(/usr/local)
if [[ $prefix = "NONE" ]] ; then
        AC_DEFINE(INSTALL_PREFIX, "/usr/local",
        [define as prefix where Wired is installed])
else
        AC_DEFINE_UNQUOTED(INSTALL_PREFIX, "$prefix")
fi


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


#Portaudio configuration
AC_CONFIG_SUBDIRS([src/portaudio])

if test x"$enable_jack" = xyes;
then

#AC_DEFINE(with_jack, "yes")
#AC_DEFINE(with_oss, "no")
#AC_DEFINE(with_alsa, "no")

   with_jack=yes
   with_oss=no
   with_alsa=no

#DLL_LIBS="$DLL_LIBS $JACK_LIBS"
#CFLAGS="$CFLAGS $JACK_CFLAGS"
#OTHER_OBJS="$OTHER_OBJS src/portaudio/src/hostapi/jack/pa_jack.o"
#AC_DEFINE(PA_USE_JACK)

#AC_DEFINE(PA_USE_ALSA, "0")
#AC_DEFINE(PA_USE_OSS, "0")

else

#DLL_LIBS="$DLL_LIBS -lasound"
#OTHER_OBJS="$OTHER_OBJS src/portaudio/src/hostapi/alsa/pa_linux_alsa.o"
#AC_DEFINE(PA_USE_ALSA)

#OTHER_OBJS="$OTHER_OBJS src/portaudio/src/hostapi/oss/pa_unix_oss.o"
#AC_DEFINE(PA_USE_OSS)

#AC_DEFINE(PA_USE_JACK, "0")


#AC_DEFINE(with_jack, "no")
#AC_DEFINE(with_oss, "yes")
#AC_DEFINE(with_alsa, "yes")

   with_jack=no
   with_oss=yes
   with_alsa=yes

fi

#AC_CHECK_LIB(asound, snd_pcm_open, have_alsa=yes, have_alsa=no)
#if [[ $have_alsa = "yes" ]] ; then
#   LIBS="$LIBS -lasound"
#fi
#PKG_CHECK_MODULES(JACK, jack, have_jack=yes, have_jack=no)
#if [[ $have_jack = "yes" ]] ; then
#   LIBS="$LIBS $JACK_LIBS"
#fi



#Some locale things
ALL_LINGUAS="fr"
AM_GNU_GETTEXT([use-libtool])
AM_GNU_GETTEXT_VERSION([0.14.4])

if test "x${prefix}" = "xNONE"; then
 AC_DEFINE(PACKAGE_LOCALE_DIR, "/usr/local/share/wired/locale")
else
 AC_DEFINE(PACKAGE_LOCALE_DIR, "${prefix}/share/wired/locale")
fi

AC_CONFIG_FILES([Makefile po/Makefile.in intl/Makefile
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
	echo -e "  Tools :\n"
	echo "    Compiler is GCC : ............. ${ac_cv_c_compiler_gnu}"
	echo "    GCC major version : ........... ${GCC_MAJOR_VERSION}"
	fi


AC_MSG_RESULT([
   Options :

    Jack : ........................ ${with_jack}
    OSS : ......................... ${with_oss}
    ALSA : ........................ ${with_alsa}
 ])

AC_MSG_RESULT([
  Extra tools required :

    wxWidgets : ................... ${WXWIDGET_VERSION}
    libsamplerate : ............... ${SAMPLERATE_VERSION}
    libsndfile : .................. ${SNDFILE_VERSION}
 ])

AC_MSG_RESULT([  Installation directories :

    Library directory : ........... ${prefix}/lib
    Program directory : ........... ${prefix}/bin
    Data directory : .............. ${prefix}/shared/wired
])
