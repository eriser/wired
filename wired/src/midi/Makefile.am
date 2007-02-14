SUBDIRS = portmidi

LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../plugins -I../fileloader -I../sequencer -I../redist -I../engine -I../mixer -I../audio -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../editmidi -I../undo -I../libs/WiredWidgets/src -I../save

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
		midi.h			\
		MidiController.cpp	\
		MidiController.h

