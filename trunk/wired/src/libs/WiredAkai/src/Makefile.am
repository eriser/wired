LDFLAGS = $(shell wx-config --libs) -Wl,-soname=libWiredAkai.so

CPPFLAGS = $(shell wx-config --cflags) -I../include -I../../../gui -I../../../plugins -I../../../fileloader -I../../../sequencer -I../../../redist -I../../../engine -I../../../mixer -I../../../midi -I../../../midi/portmidi/pm_common -I../../../midi/portmidi/porttime -I../../../editmidi -I../../../undo

pkglib_LTLIBRARIES = libWiredAkai.la

libWiredAkai_la_LDFLAGS = -version-info @LIB_WIREDAKAI_VERSION@
libWiredAkai_la_SOURCES = akai.cpp list.cpp
