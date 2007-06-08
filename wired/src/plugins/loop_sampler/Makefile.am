LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../asm -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src -I../..

if PORTAUDIO
CPPFLAGS += -I../../portaudio/include
endif


pkglib_LTLIBRARIES = libWiredLoopSampler.la

libWiredLoopSampler_la_LDFLAGS = -avoid-version
libWiredLoopSampler_la_SOURCES =			\
			LoopSampler.cpp			\
			LoopSamplerView.cpp		\
			WaveLoop.cpp			\
			RulerLoop.cpp			\
			Slice.cpp			\
			SliceGui.cpp			\
			BeatDialog.cpp			\
			LoopCursor.cpp			\
			../../redist/Polyphony.cpp	\
			../../redist/Plugin.cpp		\
			../../gui/WaveView.cpp		\
			../../gui/WaveDrawer.cpp	\
			../../engine/Settings.cpp	\
			../../audio/WaveFile.cpp

libWiredLoopSampler_la_LIBADD = $(DEPS_LIBS) -lSoundTouch
