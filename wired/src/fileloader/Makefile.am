LDFLAGS = $(shell wx-config --libs) `xml2-config --libs`

CPPFLAGS = $(shell wx-config --cflags) -I../libs/WiredAkai/include -I../gui -I../plugins -I../audio -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../editmidi -I../undo -I../libs/WiredAkai/include -I../save

if PORTAUDIO
CPPFLAGS += -I../portaudio/include
endif

noinst_LIBRARIES = libfileloader.a

libfileloader_a_SOURCES =		\
		FileLoader.cpp		\
		FileLoader.h		\
		TreeItemData.cpp	\
		TreeItemData.h		\
		akai.h			\
		list.h		\
		FileStat.cpp
