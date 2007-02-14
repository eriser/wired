CPPFLAGS = $(shell wx-config --cflags) `xml2-config --cflags`

if PORTAUDIO
CPPFLAGS += -I../portaudio/include
endif

LDFLAGS = $(shell wx-config --libs) `xml2-config --libs`

noinst_LIBRARIES = libwiredxml.a

libwiredxml_a_SOURCES =			\
		WiredXml.cpp		\
		WiredXml.h		