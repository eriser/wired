LDFLAGS = $(shell wx-config --libs) -L/usr/pkg/lib/ `xml2-config --libs`

CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../libs/WiredAkai/include -I../plugins -I../fileloader -I/opt/gnome/include -I../audio -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../asm -I../editmidi -I../undo -I.. -I../libs/WiredWidgets/src -I/usr/pkg/include/libxml2/ -I/usr/pkg/include `xml2-config --cflags` -I../portaudio/include

noinst_LIBRARIES = libsequencer.a

libsequencer_a_SOURCES = 		\
		Sequencer.cpp		\
		Sequencer.h		\
		../gui/AudioPattern.cpp \
		Track.cpp		\
		Track.h			\
		../gui/WaveDrawer.cpp

