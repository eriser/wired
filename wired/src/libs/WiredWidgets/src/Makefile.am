LDFLAGS = $(shell wx-config --libs) -g3 -O3

CPPFLAGS = $(shell wx-config --cflags) -I../../../gui -I../../../plugins -I../../../fileloader -I/opt/gnome/include -I../../../sequencer -I../../../redist -I../../../engine -I../../../mixer -I../../../midi -I../../../midi/portmidi/pm_common -I../../../midi/portmidi/porttime -I../../../asm -I../../../editmidi -I../../../undo  -g3 -O3

lib_LTLIBRARIES = libWiredWidgets.la

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
