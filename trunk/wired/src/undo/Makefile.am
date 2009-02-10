LDFLAGS = $(shell wx-config --libs) `xml2-config --libs`

CPPFLAGS = $(shell wx-config --cflags) -I../libs/WiredAkai/include -I../gui -I../plugins -I../fileloader -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../editmidi -I../audio -I../libs/WiredWidgets/src -I../wiredvideo -I../samplerate -I../codec -I../dssi -I../xml `xml2-config --cflags` -I../save

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
