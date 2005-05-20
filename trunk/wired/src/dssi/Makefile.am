CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../plugins -I../fileloader -I/opt/gnome/include -I../sequencer -I../redist -I../audio -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../editmidi -I../undo -I../libs/WiredWidgets/src -g3 -O3 

LDFLAGS = $(shell wx-config --libs) -lm

noinst_LIBRARIES = libwireddssi.a

libwireddssi_a_SOURCES =		\
		WiredExternalPluginMgr.cpp		\
		WiredExternalPluginMgr.h		\
		WiredExternalPlugin.cpp		\
		WiredExternalPlugin.h		\
		WiredExternalPluginLADSPA.cpp		\
		WiredExternalPluginLADSPA.h		\
		dssi.h			\
		ladspa.h