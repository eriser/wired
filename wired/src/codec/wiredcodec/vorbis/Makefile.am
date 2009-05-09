LDFLAGS =  $(shell wx-config --libs)

CPPFLAGS =  $(shell wx-config --cflags) -I../

pkglib_LTLIBRARIES = libWiredVorbis.la

libWiredVorbis_la_LDFLAGS = -version-info @LIB_WIREDCODECS_VERSION@
libWiredVorbis_la_SOURCES =		\
			WiredLibVorbis.cpp	

libWiredVorbis_la_LIBADD = $(DEPS_LIBS)
