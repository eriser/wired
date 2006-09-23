LDFLAGS = $(shell wx-config --libs) -L../../fileloader -lfileloader

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I/opt/gnome/include -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../asm -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src

if PORTAUDIO
CPPFLAGS += -I../../portaudio/include
endif


lib_LTLIBRARIES = libWiredBeatBox.la


libWiredBeatBox_la_SOURCES =				\
			../../redist/Polyphony.cpp	\
			BeatBox.cpp			\
			BeatButton.cpp			\
			BeatBoxChannel.cpp		\
			BeatBoxView.cpp			\
			BeatNote.cpp			\
			../../redist/Plugin.cpp		\
			../../gui/WaveView.cpp          \
			../../gui/WaveDrawer.cpp        \
			../../engine/Settings.cpp	\
			../../audio/WaveFile.cpp

libWiredBeatBox_la_LIBADD = $(DEPS_LIBS)