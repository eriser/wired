SUBDIRS = portmidi

LDFLAGS = $(shell wx-config --libs) -g3 -O3

CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../plugins -I../fileloader -I/opt/gnome/include -I../sequencer -I../redist -I../engine -I../mixer -I../audio -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../asm -I../editmidi -I../undo -I../libs/WiredWidgets/src -g3 -O3

noinst_LIBRARIES = libmidi.a

libmidi_a_SOURCES = 			\
		MidiDevice.cpp		\
		MidiDevice.h		\
		MidiFile.cpp		\
		MidiFile.h		\
		MidiInDevice.cpp	\
		MidiInDevice.h		\
		MidiThread.cpp		\
		MidiThread.h		\
		midi.h

