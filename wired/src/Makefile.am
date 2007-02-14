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



DEPS_LIBS = libs/WiredWidgets/src/.libs/libWiredWidgets.so libs/WiredAkai/src/.libs/libWiredAkai.so

wired_LDFLAGS = $(shell wx-config --libs) -lsndfile -lasound -Lmidi/portmidi $(DEPS_LIBS) -Lfileloader -lfileloader `xml2-config --libs` @SAMPLERATE_LIBS@ -lportaudio

wired_CPPFLAGS = $(shell wx-config --cflags) -Igui -Iplugins -Ifileloader -Isequencer -Iredist -Iaudio -Iengine -Imixer -Imidi -Imidi/portmidi/pm_common -Imidi/portmidi/porttime -I. -Ieditmidi -Iundo `xml2-config --cflags` @SAMPLERATE_CFLAGS@

if PORTAUDIO
wired_LDFLAGS += -Lportaudio/lib/.libs/
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
	midi/portmidi/libportmidi.a		\
	midi/portmidi/porttime/libporttime.a	\
	mixer/libmixer.a			\
	plugins/libplugins.a			\
	redist/libredist.a			\
	undo/libundo.a				\
	xml/libwiredxml.a			\
	samplerate/libwiredsamplerate.a		\
	codec/libwiredCodec.a			\
	wiredvideo/libwiredvideo.a		\
	dssi/libwireddssi.a			
