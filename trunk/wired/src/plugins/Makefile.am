if PLUGINS
SUBDIRS = beatbox compressor crusher delay filter loop_sampler akai_sampler test reverb chorus wahwah
endif

LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../libs/WiredAkai/include -I../gui -I../audio -I../fileloader -I/opt/gnome/include -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../editmidi -I../undo -I../libs/WiredWidgets/src -I../portaudio/include

#DEPS_LIBS = -L../libs/WiredWidgets/src/ -lWiredWidgets

noinst_LIBRARIES = libplugins.a

libplugins_a_SOURCES = 			\
		HostCallback.cpp	\
		HostCallback.h		\
		PluginLoader.cpp	\
		PluginLoader.h
