LDFLAGS = $(shell wx-config --libs)

CPPFLAGS =  $(shell wx-config --cflags) -I../ #-fmessage-length=0 -funroll-all-loops -fomit-frame-pointer -falign-loops=2 -falign-jumps=2 -falign-functions=2 

pkglib_LTLIBRARIES = libWiredMpeg.la

libWiredMpeg_la_LDFLAGS = -version-info @LIB_WIREDCODECS_VERSION@
libWiredMpeg_la_SOURCES =		\
			WiredLibMpeg.cpp	

libWiredMpeg_la_LIBADD = $(DEPS_LIBS)
