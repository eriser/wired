if PORTMIDI
SUBDIRS = portmidi
endif

LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../plugins -I../fileloader -I../sequencer -I../redist -I../engine -I../mixer -I../audio -I../editmidi -I../undo -I../libs/WiredWidgets/src

if PORTMIDI
CPPFLAGS += -I../midi/portmidi/pm_common -I../midi/portmidi/porttime 
endif

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

