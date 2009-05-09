LDFLAGS = $(shell wx-config --libs) -L../../fileloader -lfileloader `xml2-config --libs`

CPPFLAGS = $(shell wx-config --cflags) -I../../xml -I../../save -I../../gui   \
	-I../../plugins -I../../fileloader -I../../audio \
	-I../../redist -I../../engine -I../../mixer -I../../midi	      \
	-I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime	      \
	-I../../editmidi -I../../undo -I../../libs/WiredWidgets/src -I../..   \
	`xml2-config --cflags`

pkglib_LTLIBRARIES = libWiredBeatBox.la

libWiredBeatBox_la_LDFLAGS = -version-info @LIB_WIREDPLUGIN_VERSION@
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

libWiredBeatBox_la_LIBADD = $(DEPS_LIBS) ../../save/libsave.la
