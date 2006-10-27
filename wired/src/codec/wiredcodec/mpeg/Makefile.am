LDFLAGS = $(shell wx-config --libs)

CPPFLAGS =  $(shell wx-config --cflags) #-fmessage-length=0 -funroll-all-loops -fomit-frame-pointer -falign-loops=2 -falign-jumps=2 -falign-functions=2 

pkglib_LTLIBRARIES = libWiredMpeg.la

libWiredMpeg_la_SOURCES =		\
			WiredLibMpeg.cpp	

libWiredMpeg_la_LIBADD = $(DEPS_LIBS)
