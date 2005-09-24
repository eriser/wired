LDFLAGS = -lmpeg3 -g3 -O3

CPPFLAGS =  -O3 -march=i686 -fmessage-length=0 -funroll-all-loops -fomit-frame-pointer -falign-loops=2 -falign-jumps=2 -falign-functions=2

lib_LTLIBRARIES = libWiredMpeg.la

libWiredMpeg_la_SOURCES =		\
			WiredLibMpeg.cpp	

libWiredMpeg_la_LIBADD = $(DEPS_LIBS)
