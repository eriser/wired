LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../save -I../plugins -I../fileloader -I../sequencer -I../redist -I../engine -I../audio -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../editmidi -I../undo -I../libs/WiredWidgets/src

noinst_LIBRARIES = libmixer.a

libmixer_a_SOURCES = 			\
		Channel.cpp		\
		Channel.h		\
		Mixer.cpp		\
		Mixer.h			\
		MixerError.h


