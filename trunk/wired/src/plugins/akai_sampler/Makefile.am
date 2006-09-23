LDFLAGS = $(shell wx-config --libs) -L../../fileloader -lfileloader

CPPFLAGS = $(shell wx-config --cflags) -I../../libs/WiredAkai/include -I../../gui -I../../plugins -I../../fileloader -I/opt/gnome/include -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src

if PORTAUDIO
CPPFLAGS += -I../../portaudio/include
endif


lib_LTLIBRARIES = libWiredSampler.la

libWiredSampler_la_SOURCES =				\
			AkaiSampler.cpp			\
			ASClavier.cpp    		\
			ASPlugin.cpp			\
			ASEnvel.cpp      		\
			ASWaveView.cpp      		\
			ASKey.cpp 			\
			ASList.cpp			\
			ASPlugPanel.cpp			\
			ASSampleList.cpp		\
			ASKeygroupEditor.cpp		\
			ASKeygroupList.cpp		\
			ASLoop.cpp			\
			AkaiSampler.h			\
			ASClavier.h    			\
			ASPlugin.h			\
			ASEnvel.h      			\
			AkaiSampler.h			\
			ASKey.h 			\
		  	Solver.h			\
			ASList.h			\
			Splines.h			\
			ASPlugPanel.h			\
			ASSampleList.h			\
			ASWaveView.h			\
			ASKeygroupList.h		\
			ASLoop.h			\
			../../redist/Polyphony.cpp	\
			../../redist/Plugin.cpp		\
			../../gui/WaveView.cpp		\
			../../gui/WaveDrawer.cpp	\
			../../engine/Settings.cpp	\
			../../audio/WaveFile.cpp

libWiredSampler_la_LIBADD = $(DEPS_LIBS)
