LDFLAGS = $(shell wx-config --libs) -g3 -O3

CPPFLAGS = $(shell wx-config --cflags) -I../libs/WiredAkai/include -I../gui -I../plugins -I../fileloader -I/opt/gnome/include -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../asm -I../editmidi -I../audio -I../libs/WiredWidgets/src -g3 -O3

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
