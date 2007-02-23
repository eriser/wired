LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src

pkglib_LTLIBRARIES = libWiredDelay.la

libWiredDelay_la_LDFLAGS = -avoid-version
libWiredDelay_la_SOURCES =				\
			DelayPlug.cpp		\
			../../redist/Plugin.cpp

libWiredDelay_la_LIBADD = $(DEPS_LIBS)
