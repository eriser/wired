SUBDIRS = beatbox compressor crusher delay filter loop_sampler akai_sampler test reverb

LDFLAGS = $(shell wx-config --libs)  -g3 -O3

CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../audio -I../fileloader -I/opt/gnome/include -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../editmidi -I../undo -I../libs/WiredWidgets/src -g3 -O3

#DEPS_LIBS = -L../libs/WiredWidgets/src/ -lWiredWidgets

noinst_LIBRARIES = libplugins.a

libplugins_a_SOURCES = 			\
		HostCallback.cpp	\
		HostCallback.h		\
		PluginLoader.cpp	\
		PluginLoader.h
