SUBDIRS =				\
	gui 				\
	fileloader			\
	sequencer 			\
	audio				\
	editmidi			\
	engine 				\
	midi 				\
	mixer 				\
	redist 				\
	undo 				\
	libs				\
	plugins				\
	data				\
	conf				\
	xml

DEPS_LIBS = libs/WiredWidgets/src/.libs/libWiredWidgets.so libs/WiredAkai/src/.libs/libWiredAkai.so -Lfileloader -lfileloader

wired_LDFLAGS = $(shell wx-config --libs) -lsndfile -lasound -Lmidi/portmidi $(DEPS_LIBS) -lportaudio  -L/usr/pkg/lib/ `xml2-config --libs` -g3 -O3

wired_CPPFLAGS = $(shell wx-config --cflags) -Igui -Iplugins -Ifileloader -I/opt/gnome/include -Isequencer -Iredist -Iaudio -Iengine -Imixer -Imidi -Imidi/portmidi/pm_common -Imidi/portmidi/porttime -I. -Iasm -Ieditmidi -Iundo -I.  -I/usr/pkg/include/libxml2/ -I/usr/pkg/include `xml2-config --cflags` -g3 -O3

bin_PROGRAMS = wired

wired_SOURCES = 

wired_LDADD =				\
	gui/libgui.a			\
	sequencer/libsequencer.a	\
	audio/libaudio.a		\
	editmidi/libeditmidi.a		\
	engine/libengine.a		\
	fileloader/libfileloader.a	\
	midi/libmidi.a			\
	midi/portmidi/libportmidi.a	\
	midi/portmidi/porttime/libporttime.a	\
	mixer/libmixer.a		\
	plugins/libplugins.a		\
	redist/libredist.a		\
	undo/libundo.a			\
	xml/libwiredxml.a
