LDFLAGS = -lFLAC++ $(shell wx-config --libs)

CPPFLAGS =  $(shell wx-config --cflags)

pkglib_LTLIBRARIES = libWiredFlac.la

libWiredFlac_la_SOURCES =		\
			WiredLibFlac.cpp	

libWiredFlac_la_LIBADD = $(DEPS_LIBS)
