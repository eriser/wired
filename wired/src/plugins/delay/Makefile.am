LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I/opt/gnome/include -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../asm -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src

lib_LTLIBRARIES = libWiredDelay.la

libWiredDelay_la_SOURCES =				\
			DelayPlug.cpp		\
			../../redist/Plugin.cpp

libWiredDelay_la_LIBADD = $(DEPS_LIBS)