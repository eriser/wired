LDFLAGS = $(shell wx-config --libs) -L../../fileloader -lfileloader -g3 -O3

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I/opt/gnome/include -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../asm -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src -g3 -O3

lib_LTLIBRARIES = libWiredLoopSampler.la

libWiredLoopSampler_la_SOURCES =				\
			LoopSampler.cpp			\
			LoopSamplerView.cpp		\
			WaveLoop.cpp			\
			RulerLoop.cpp			\
			Slice.cpp			\
			../../redist/Polyphony.cpp	\
			../../redist/Plugin.cpp		\
			../../gui/WaveView.cpp		\
			../../gui/WaveDrawer.cpp	\
			../../engine/Settings.cpp	\
			../../audio/WaveFile.cpp

libWiredLoopSampler_la_LIBADD = $(DEPS_LIBS) -lSoundTouch
