LDFLAGS = -lFLAC++ $(shell wx-config --libs)

CPPFLAGS =  $(shell wx-config --cflags) -I../

pkglib_LTLIBRARIES = libWiredFlac.la

libWiredFlac_la_LDFLAGS = -version-info @LIB_WIREDCODECS_VERSION@
libWiredFlac_la_SOURCES =		\
			WiredLibFlac.cpp	

libWiredFlac_la_LIBADD = $(DEPS_LIBS)
