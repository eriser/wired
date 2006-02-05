CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../plugins -I../fileloader -I/opt/gnome/include -I../sequencer -I../redist -I../audio -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../editmidi -I../undo -I../libs/WiredWidgets/src -I/usr/pkg/include/libxml2/ -I/usr/pkg/include `xml2-config --cflags`

LDFLAGS = $(shell wx-config --libs) -L/usr/pkg/lib/ `xml2-config --libs`

noinst_LIBRARIES = libwiredxml.a

libwiredxml_a_SOURCES =			\
		../redist/Plugin.cpp	\
		WiredXml.cpp		\
		WiredXml.h		\
		WiredSessionXml.cpp	\
		WiredSessionXml.h	