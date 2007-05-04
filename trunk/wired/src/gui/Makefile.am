LDFLAGS = $(shell wx-config --libs) `xml2-config --libs`

# CPPFLAGS = $(shell wx-config --cflags) -I../xml -I../save -I../audio -I../libs/WiredAkai/include -I../plugins -I../fileloader -I/opt/gnome/include -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../asm -I../editmidi -I../undo -I../libs/WiredWidgets/src -I/usr/pkg/include/libxml2/ -I/usr/pkg/include `xml2-config --cflags `
# XXX merge conflict...
CPPFLAGS = $(shell wx-config --cflags) -I../samplerate -I../xml -I../save -I../audio -I../libs/WiredAkai/include -I../plugins -I../fileloader -I/opt/gnome/include -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../asm -I../editmidi -I../undo -I../libs/WiredWidgets/src `xml2-config --cflags ` -I../

if PORTAUDIO
CPPFLAGS += -I../portaudio/include
endif

noinst_LIBRARIES = libgui.a

libgui_a_SOURCES = 			\
		AccelCenter.h		\
		AccelCenter.cpp		\
		AudioPattern.cpp	\
		AutomationTrack.cpp	\
		ChannelGui.cpp		\
		ChannelGui.h		\
		ColoredLine.cpp		\
		ColoredLine.h		\
		ColoredBox.cpp		\
		ColoredBox.h		\
		Colour.h		\
		Cursor.cpp		\
		Cursor.h		\
		HelpCtrl.cpp		\
		HelpCtrl.h		\
		HelpPanel.cpp		\
		HelpPanel.h		\
		MainApp.cpp		\
		MainApp.h		\
		MainWindow.cpp		\
		MainWindow.h		\
		MediaLibrary.cpp	\
		MediaLibrary.h		\
		MLTraverser.cpp		\
		MLTraverser.h		\
		MLTree.cpp		\
		MLTree.h		\
		MLTreeInfos.cpp		\
		MLTreeInfos.h		\
		MidiPattern.cpp		\
		MidiPattern.h		\
		MixerGui.cpp		\
		MixerGui.h		\
		OptionPanel.cpp		\
		OptionPanel.h		\
		Pattern.cpp		\
		Pattern.h		\
		Rack.cpp		\
		Rack.h			\
		Ruler.cpp		\
		Ruler.h			\
		SeqTrack.cpp		\
		SeqTrack.h		\
		SequencerView.cpp	\
		SequencerView.h		\
		SequencerGui.cpp	\
		SequencerGui.h		\
		SelectionZone.h		\
		SelectionZone.cpp	\
		SettingWindow.cpp	\
		SettingWindow.h		\
		StaticLabel.cpp		\
		StaticLabel.h		\
		Transport.cpp		\
		Transport.h		\
		WaveEditor.cpp		\
		WaveEditor.h		\
		WaveView.cpp		\
		WaveView.h		\
		WaveDrawer.cpp		\
		WaveDrawer.h		\
		WaveEditorDrawer.cpp	\
		WaveEditorDrawer.h	\
		WavePanel.cpp		\
		WavePanel.h		\
		PluginEffect.cpp	\
		PluginEffect.h		\
		FileConversion.h	\
		FileConversion.cpp	\
		FloatingFrame.h		\
		FloatingFrame.cpp	\
		../save/WiredDocument.cpp	\
		../save/SaveCenter.cpp

libgui_a_LIBADD = ../save/libsave.la
