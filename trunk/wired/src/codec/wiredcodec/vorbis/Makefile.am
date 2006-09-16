LDFLAGS =  $(shell wx-config --libs)

CPPFLAGS =  $(shell wx-config --cflags)

lib_LTLIBRARIES = libWiredVorbis.la

libWiredVorbis_la_SOURCES =		\
			WiredLibVorbis.cpp	

libWiredVorbis_la_LIBADD = $(DEPS_LIBS)
