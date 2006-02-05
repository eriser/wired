SUBDIRS = wiredcodec

CPPFLAGS = $(shell wx-config --cflags)

LDFLAGS = $(shell wx-config --libs) -ldl

noinst_LIBRARIES = libwiredCodec.a

libwiredCodec_a_SOURCES =			\
		WiredCodec.cpp			\
		WiredCodec.h			\
		wiredcodec/WiredApiCodec.h

