LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../include -I../../../gui -I../../../plugins -I../../../fileloader -I../../../sequencer -I../../../redist -I../../../engine -I../../../mixer -I../../../midi -I../../../midi/portmidi/pm_common -I../../../midi/portmidi/porttime -I../../../editmidi -I../../../undo

lib_LTLIBRARIES = libWiredAkai.la

libWiredAkai_la_LDFLAGS = -avoid-version
libWiredAkai_la_SOURCES = akai.c list.c
