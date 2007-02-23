LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../libs/WiredAkai/include -I../gui -I../plugins -I../fileloader -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../editmidi -I../audio -I../libs/WiredWidgets/src -I../save

if PORTAUDIO
CPPFLAGS += -I../portaudio/include
endif


noinst_LIBRARIES = libundo.a

libundo_a_SOURCES = 			\
		Visitor.cpp		\
		Visitor.h		\
		cAction.cpp		\
		cAction.h		\
		cActionManager.cpp	\
		cActionManager.h	\
		cAddTrackAction.h	\
		cImportMidiAction.cpp	\
		cImportMidiAction.h
