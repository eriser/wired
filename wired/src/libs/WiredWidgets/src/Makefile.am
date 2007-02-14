LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../../../gui -I../../../plugins -I../../../fileloader -I../../../sequencer -I../../../redist -I../../../engine -I../../../mixer -I../../../midi -I../../../midi/portmidi/pm_common -I../../../midi/portmidi/porttime -I../../../editmidi -I../../../undo

lib_LTLIBRARIES = libWiredWidgets.la

libWiredWidgets_la_LDFLAGS = -avoid-version
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
		CycleKnob.h
