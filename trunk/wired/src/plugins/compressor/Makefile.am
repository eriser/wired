LDFLAGS = $(shell wx-config --libs) -g3 -O3

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I/opt/gnome/include -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../asm -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src -g3 -O3

lib_LTLIBRARIES = libWiredCompressor.la

libWiredCompressor_la_SOURCES =			\
			CompressorPlug.cpp	\
			../../redist/Plugin.cpp

libWiredCompressor_la_LIBADD = $(DEPS_LIBS)