LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../libs/WiredAkai/include -I../gui -I../plugins -I../audio -I/opt/gnome/include -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../asm -I../editmidi -I../undo -I../libs/WiredAkai/include

noinst_LIBRARIES = libfileloader.a

libfileloader_a_SOURCES =		\
		FileLoader.cpp		\
		FileLoader.h		\
		TreeItemData.cpp	\
		TreeItemData.h		\
		akai.h			\
		list.h		\
		FileStat.cpp
