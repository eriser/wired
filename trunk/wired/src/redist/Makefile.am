LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../plugins -I../fileloader -I/opt/gnome/include -I../sequencer -I../audio -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../asm -I../editmidi -I../undo

noinst_LIBRARIES = libredist.a

libredist_a_SOURCES = 			\
		Plugin.cpp		\
		Plugin.h		\
		Polyphony.cpp		\
		Polyphony.h
