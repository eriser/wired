LDFLAGS = $(shell wx-config --libs) -L/usr/pkg/lib/ `xml2-config --libs` -g3 -O3

CPPFLAGS = $(shell wx-config --cflags) -I../audio -I../libs/WiredAkai/include -I../plugins -I../fileloader -I/opt/gnome/include -I../sequencer -I../redist -I../engine -I../mixer -I../midi -I../midi/portmidi/pm_common -I../midi/portmidi/porttime -I../asm -I../editmidi -I../undo -I../libs/WiredWidgets/src -I/usr/pkg/include/libxml2/ -I/usr/pkg/include `xml2-config --cflags` -g3 -O3

noinst_LIBRARIES = libgui.a

libgui_a_SOURCES = 			\
		AccelCenter.h		\
		AccelCenter.cpp		\
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
		SeqTrackPattern.cpp	\
		SeqTrackPattern.h	\
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
		PluginEffect.h
