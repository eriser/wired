LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src

pkglib_LTLIBRARIES = libWiredSynth.la

libWiredSynth_la_LDFLAGS = -avoid-version
libWiredSynth_la_SOURCES =				\
			Adsr.cpp		\
			Lfo.cpp		\
			Osc.cpp		\
			SynthPlug.cpp		\
			../../redist/Plugin.cpp

libWiredSynth_la_LIBADD = $(DEPS_LIBS)
