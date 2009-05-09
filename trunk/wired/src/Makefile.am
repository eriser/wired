SUBDIRS =				\
	save				\
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
	xml				\
	samplerate			\
	codec				\
	wiredvideo			\
	dssi



DEPS_LIBS = libs/WiredWidgets/src/.libs/libWiredWidgets.so \
	    libs/WiredAkai/src/.libs/libWiredAkai.so

wired_LDFLAGS = $(shell wx-config --libs) \
		$(shell pkg-config portaudio-2.0 --libs) \
		$(shell xml2-config --libs) \
		@SAMPLERATE_LIBS@ \
		-lsndfile \
		-Lfileloader -lfileloader \
		-rpath ${libdir}/wired \
		$(DEPS_LIBS)

wired_CPPFLAGS = $(shell wx-config --cflags) \
		 $(shell xml2-config --cflags) \
		@SAMPLERATE_CFLAGS@ \
		-Igui -Iplugins -Ifileloader -Isequencer -Iredist \
		-Iaudio -Iengine -Imixer -Iundo \
		-Imidi -Ieditmidi


if PORTMIDI
wired_CPPFLAGS += -Imidi/portmidi/pm_common -Imidi/portmidi/porttime
wired_LDFLAGS += -lasound -Lmidi/portmidi
else
wired_LDFLAGS += -lportmidi -lporttime
endif

bin_PROGRAMS = wired

wired_SOURCES =

wired_LDADD =					\
	save/libsave.la				\
	gui/libgui.a				\
	sequencer/libsequencer.a		\
	audio/libaudio.a			\
	editmidi/libeditmidi.a			\
	engine/libengine.a			\
	fileloader/libfileloader.a		\
	midi/libmidi.a				\
	mixer/libmixer.a			\
	plugins/libplugins.a			\
	redist/libredist.a			\
	undo/libundo.a				\
	xml/libwiredxml.a			\
	samplerate/libwiredsamplerate.a		\
	codec/libwiredCodec.a			\
	wiredvideo/libwiredvideo.a		\
	dssi/libwireddssi.a

if PORTMIDI
wired_LDADD +=\
	midi/portmidi/libportmidi.a		\
	midi/portmidi/porttime/libporttime.a	
endif
