LDFLAGS = $(shell wx-config --libs) `xml2-config --libs`

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../asm -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src -I../.. -I../../xml `xml2-config --cflags` -I../../save

pkglib_LTLIBRARIES = libWiredLoopSampler.la

libWiredLoopSampler_la_LDFLAGS = -version-info @LIB_WIREDPLUGIN_VERSION@
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
