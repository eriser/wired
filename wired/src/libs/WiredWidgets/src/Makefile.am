LDFLAGS = $(shell wx-config --libs) -Wl,-soname=libWiredWidgets.so

CPPFLAGS = $(shell wx-config --cflags) -I../../../gui -I../../../plugins -I../../../fileloader -I../../../sequencer -I../../../redist -I../../../engine -I../../../mixer -I../../../midi -I../../../midi/portmidi/pm_common -I../../../midi/portmidi/porttime -I../../../editmidi -I../../../undo

pkglib_LTLIBRARIES = libWiredWidgets.la

libWiredWidgets_la_LDFLAGS = -version-info @LIB_WIREDWIDGETS_VERSION@
libWiredWidgets_la_SOURCES = 			\
		ChoiceButton.cpp	\
		ChoiceButton.h		\
		DownButton.cpp		\
		DownButton.h		\
		FaderCtrl.cpp		\
		FaderCtrl.h		\
		HoldButton.cpp		\
		HoldButton.h		\
		IdButton.cpp		\
		IdButton.h		\
		KnobCtrl.cpp		\
		KnobCtrl.h		\
		StaticBitmap.cpp	\
		StaticBitmap.h		\
		VUMCtrl.cpp		\
		VUMCtrl.h		\
		Hint.cpp		\
		Hint.h			\
		HintedKnob.cpp		\
		HintedKnob.h		\
		StaticPosKnob.cpp	\
		StaticPosKnob.h		\
		CycleKnob.cpp		\
		CycleKnob.h		\
		TransparentStaticText.cpp	\
		TransparentStaticText.h
