if PLUGINS
SUBDIRS = beatbox crusher delay loop_sampler akai_sampler reverb chorus wahwah 

# experimental or bugged plugins:
#SUBDIRS += synth compressor filter test
endif

LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../libs/WiredAkai/include -I../gui -I../audio -I../fileloader -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../editmidi -I../undo -I../libs/WiredWidgets/src -I../save -I../wiredvideo -I../dssi

if PORTAUDIO
CPPFLAGS += -I../portaudio/include
endif


#DEPS_LIBS = -L../libs/WiredWidgets/src/ -lWiredWidgets

noinst_LIBRARIES = libplugins.a

libplugins_a_SOURCES = 			\
		HostCallback.cpp	\
		HostCallback.h		\
		PluginLoader.cpp	\
		PluginLoader.h
