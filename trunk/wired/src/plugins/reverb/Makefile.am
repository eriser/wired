LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../../gui -I../../plugins -I../../fileloader -I/opt/gnome/include -I../../audio -I../../redist -I../../engine -I../../mixer -I../../midi -I../../midi/portmidi/pm_common -I../../midi/portmidi/porttime -I../../asm -I../../editmidi -I../../undo -I../../libs/WiredWidgets/src  -Wall  -Wunused

pkglib_LTLIBRARIES = libWiredReverb.la

libWiredReverb_la_SOURCES =				\
			Stk.cpp			\
			Effect.cpp 		\
			Filter.cpp		\
			PRCRev.cpp		\
			NRev.cpp		\
			JCRev.cpp		\
			Delay.cpp		\
			ReverbPlug.cpp		\
			../../redist/Plugin.cpp

libWiredReverb_la_LIBADD = $(DEPS_LIBS)