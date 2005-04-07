LDFLAGS = $(shell wx-config --libs) -g3 -O3

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I/opt/gnome/include -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../asm -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src -Wall -Wunused -g3 -O3

lib_LTLIBRARIES = libWiredChorus.la

libWiredChorus_la_SOURCES =			\
			ChorusPlug.cpp		\
			Effect.cpp		\
			WaveLoop.cpp		\
			WvIn.cpp		\
			WvOut.cpp		\
			DelayL.cpp		\
			Delay.cpp		\
			Stk.cpp			\
			Filter.cpp		\
			Chorus.cpp		\
			../../redist/Plugin.cpp

libWiredChorus_la_LIBADD = $(DEPS_LIBS)