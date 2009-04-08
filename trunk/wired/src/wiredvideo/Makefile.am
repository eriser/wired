LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) \
		-I../gui \
		-I../redist \
		-I../xml \
		$(shell xml2-config --cflags) \
		-I../save

noinst_LIBRARIES = libwiredvideo.a

libwiredvideo_a_SOURCES = 			\
		WiredVideo.cpp		\
		WiredVideo.h		\
		WiredMplayer.h	\
		WiredMplayer.cpp
