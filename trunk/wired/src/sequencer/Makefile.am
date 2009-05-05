LDFLAGS = $(shell wx-config --libs) -L/usr/pkg/lib/ `xml2-config --libs`

CPPFLAGS = $(shell wx-config --cflags) \
	-I../xml \
	-I../save \
	-I../gui \
	-I../libs/WiredAkai/include \
	-I../plugins \
	-I../fileloader \
	-I../audio \
	-I../redist \
	-I../engine \
	-I../mixer \
	-I../midi \
	-I../midi/portmidi/pm_common \
	-I../midi/portmidi/porttime \
	-I../asm \
	-I../editmidi \
	-I../undo \
	-I.. \
	-I../libs/WiredWidgets/src \
	-I../wiredvideo \
	-I../samplerate \
	-I../codec \
	-I../dssi \
	`xml2-config --cflags`

noinst_LIBRARIES = libsequencer.a

libsequencer_a_SOURCES = 		\
		Sequencer.cpp		\
		Sequencer.h		\
		Track.cpp		\
		Track.h			\
		SeqTrackPattern.cpp	\
		../gui/WaveDrawer.cpp

