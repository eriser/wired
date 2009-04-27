LDFLAGS = $(shell wx-config --libs) `xml2-config --libs`

CPPFLAGS = $(shell wx-config --cflags) \
	-I../libs/WiredAkai/include \
	-I../gui \
	-I../plugins \
	-I../audio \
	-I../sequencer \
	-I../redist \
	-I../engine \
	-I../mixer \
	-I../midi \
	-I../midi/portmidi/pm_common \
	-I../midi/portmidi/porttime \
	-I../editmidi \
	-I../undo \
	-I../samplerate \
	-I../codec \
	-I../libs/WiredWidgets/src \
	-I../xml \
	`xml2-config --cflags ` \
	-I../redist \
	-I../dssi \
	-I../save

noinst_LIBRARIES = libfileloader.a

libfileloader_a_SOURCES =		\
		FileConversion.h	\
		FileConversion.cpp	\
		TreeItemData.cpp	\
		TreeItemData.h		\
		akai.h			\
		list.h		\
		FileStat.cpp
