LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../plugins -I../fileloader -I/opt/gnome/include -I../sequencer -I../redist -I../audio -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../asm -I../undo -I../libs/WiredWidgets/src

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
