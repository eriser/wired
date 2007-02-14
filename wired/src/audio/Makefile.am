LDFLAGS = $(shell wx-config --libs) -lsndfile

CPPFLAGS = $(shell wx-config --cflags) -I../gui -I../plugins -I../fileloader -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../editmidi -I../undo -I../

noinst_LIBRARIES = libaudio.a

libaudio_a_SOURCES = 			\
		ClipBoard.cpp		\
		ClipBoard.h		\
		Error.hh		\
		WaveFile.cpp		\
		WaveFile.h		\
		WriteWaveFile.cpp	\
		WriteWaveFile.h

