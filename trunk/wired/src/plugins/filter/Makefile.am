LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src

pkglib_LTLIBRARIES = libWiredFilter.la

libWiredFilter_la_LDFLAGS = -avoid-version
libWiredFilter_la_SOURCES =				\
			FilterPlug.cpp		\
			../../redist/Plugin.cpp

libWiredFilter_la_LIBADD = $(DEPS_LIBS)
