CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../plugins -I../fileloader  \
	-I../sequencer -I../redist -I../audio -I../mixer \
	-I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime    \
	-I../editmidi -I../undo -I../libs/WiredWidgets/src                    \
	-I../../libs/WiredWidgets/src -I../engine \
	-I../xml `xml2-config --cflags` -I../save

LDFLAGS = $(shell wx-config --libs) -lm `xml2-config --libs`

noinst_LIBRARIES = libwireddssi.a

libwireddssi_a_SOURCES =				\
		WiredExternalPluginMgr.cpp		\
		WiredExternalPluginMgr.h		\
		WiredExternalPlugin.cpp			\
		WiredExternalPlugin.h			\
		WiredExternalPluginLADSPA.cpp		\
		WiredExternalPluginLADSPA.h		\
		dssi.h					\
		WiredExternalPluginGui.cpp		\
		WiredExternalPluginGui.h		\
		ladspa.h				\
		../redist/Plugin.cpp
