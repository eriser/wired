LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src

pkglib_LTLIBRARIES = libWiredChorus.la

libWiredChorus_la_LDFLAGS = -avoid-version
libWiredChorus_la_SOURCES =			\
			ChorusPlug.cpp		\
			Effect.cpp		\
			WvLoop.cpp		\
			WvIn.cpp		\
			WvOut.cpp		\
			DelayL.cpp		\
			Delay.cpp		\
			Stk.cpp			\
			Filter.cpp		\
			Chorus.cpp		\
			../../redist/Plugin.cpp

libWiredChorus_la_LIBADD = $(DEPS_LIBS)
