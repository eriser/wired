LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../plugins -I../fileloader -I../sequencer -I../redist -I../audio -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../undo -I../libs/WiredWidgets/src -I../save

if PORTMIDI
CPPFLAGS += -I../midi/portmidi/pm_common -I../midi/portmidi/porttime 
endif

if PORTAUDIO
CPPFLAGS += -I../portaudio/include
endif

noinst_LIBRARIES = libeditmidi.a

libeditmidi_a_SOURCES =			\
		Clavier.cpp		\
		Clavier.h		\
		EditMidi.cpp		\
		EditMidi.h		\
		EditNote.cpp		\
		EditNote.h		\
		Key.cpp			\
		Key.h			\
		MidiAttr.cpp		\
		MidiAttr.h		\
		MidiPart.cpp		\
		MidiPart.h		\
		Note.cpp		\
		Note.h			\
		RulerMidi.cpp		\
		RulerMidi.h
