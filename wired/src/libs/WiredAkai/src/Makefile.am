LDFLAGS = $(shell wx-config --libs) -g3 -O3

CPPFLAGS = $(shell wx-config --cflags) -I../include -I../../../gui -I../../../plugins -I../../../fileloader -I/opt/gnome/include -I../../../sequencer -I../../../redist -I../../../engine -I../../../mixer -I../../../midi -I../../../midi/portmidi/pm_common -I../../../midi/portmidi/porttime -I../../../asm -I../../../editmidi -I../../../undo  -g3 -O3

lib_LTLIBRARIES = libWiredAkai.la

libWiredAkai_la_SOURCES = akai.c list.c
