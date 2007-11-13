LDFLAGS = $(shell wx-config --libs) `xml2-config --libs`

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src -I../../xml `xml2-config --cflags` -I../../save

pkglib_LTLIBRARIES = libWiredCompressor.la

libWiredCompressor_la_LDFLAGS = -avoid-version
libWiredCompressor_la_SOURCES =			\
			CompressorPlug.cpp	\
			../../redist/Plugin.cpp

libWiredCompressor_la_LIBADD = $(DEPS_LIBS)
