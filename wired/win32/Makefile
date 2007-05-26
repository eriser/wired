# -----------------------------------------------------------------------------------
# Wired anternative Makefile written by Daniel Moura <oxe AT oxesoft.com> in May-2007
# -----------------------------------------------------------------------------------

SRCPATH    := ../src
ifeq ($(findstring MINGW32,$(shell uname)),MINGW32)
BINPATH    := ./.obj-win32
DLL        := dll
EXE        := .exe
WXLIBS     := -lwx_msw-2.8 -mthreads
WXCFLAGS   := -DWXUSINGDLL -D__WXMSW__ -mthreads
XMLLIBS    := -lxml2
XMLCFLAGS  :=
PLATLIBS   := -mconsole -lwinmm -lwsock32 -Wl,-u,_WinMain@16
RESFILE    := $(BINPATH)/resource.o
PLUGINSDEP := $(BINPATH)/libwiredwidgets.a
CONFPATH   := .
else
BINPATH    := ../.obj-linux
DLL        := so
EXE        :=
WXLIBS     := $(shell wx-config --libs)
WXCFLAGS   := $(shell wx-config --cflags)
XMLLIBS    := $(shell xml2-config --libs)
XMLCFLAGS  := $(shell xml2-config --cflags)
PLATLIBS   := 
RESFILE    :=
PLUGINSDEP := $(BINPATH)/libwiredwidgets.so
CONFPATH   := $(SRCPATH)/conf
endif

ifeq ($(PREFIX),)
PREFIX := ./bin
endif

WIREDLIBS                    := libsave.a                     \
                                libgui.a                      \
                                libsequencer.a                \
                                libaudio.a                    \
                                libeditmidi.a                 \
                                libengine.a                   \
                                libfileloader.a               \
                                libmidi.a                     \
                                libmixer.a                    \
                                libplugins.a                  \
                                libredist.a                   \
                                libundo.a                     \
                                libwiredxml.a                 \
                                libwiredsamplerate.a          \
                                libwiredcodec.a               \
                                libwiredvideo.a               \
                                libwireddssi.a                \
                                libwiredakai.$(DLL)           \
                                libwiredwidgets.$(DLL)
WIREDPLUG                    := libwiredbeatbox.$(DLL)        \
                                libwiredcrusher.$(DLL)        \
                                libwireddelay.$(DLL)          \
                                libwiredloopsampler.$(DLL)    \
                                libwiredakaisampler.$(DLL)    \
                                libwiredreverb.$(DLL)         \
                                libwiredchorus.$(DLL)         \
                                libwiredwahwah.$(DLL)
SOURCES_libsave              := SaveCenter.cpp                \
                                WiredDocument.cpp             \
                                WiredXml.cpp
SOURCES_libgui               := AccelCenter.cpp               \
                                AudioPattern.cpp              \
                                AutomationTrack.cpp           \
                                ChannelGui.cpp                \
                                ColoredLine.cpp               \
                                ColoredBox.cpp                \
                                Cursor.cpp                    \
                                HelpCtrl.cpp                  \
                                HelpPanel.cpp                 \
                                MainApp.cpp                   \
                                MainWindow.cpp                \
                                MediaLibrary.cpp              \
                                MLTraverser.cpp               \
                                MLTree.cpp                    \
                                MLTreeInfos.cpp               \
                                MidiPattern.cpp               \
                                MixerGui.cpp                  \
                                OptionPanel.cpp               \
                                Pattern.cpp                   \
                                Rack.cpp                      \
                                Ruler.cpp                     \
                                SeqTrack.cpp                  \
                                SequencerView.cpp             \
                                SequencerGui.cpp              \
                                SelectionZone.cpp             \
                                SettingWindow.cpp             \
                                StaticLabel.cpp               \
                                Transport.cpp                 \
                                WaveEditor.cpp                \
                                WaveView.cpp                  \
                                WaveDrawer.cpp                \
                                WaveEditorDrawer.cpp          \
                                WavePanel.cpp                 \
                                PluginEffect.cpp              \
                                FileConversion.cpp            \
                                FloatingFrame.cpp             \
                                WiredDocument.cpp             \
                                SaveCenter.cpp
SOURCES_libsequencer         := Sequencer.cpp                 \
                                Track.cpp                     \
                                SeqTrackPattern.cpp           \
                                WaveDrawer.cpp
SOURCES_libaudio             := ClipBoard.cpp                 \
                                WaveFile.cpp                  \
                                WriteWaveFile.cpp
SOURCES_libeditmidi          := Clavier.cpp                   \
                                EditMidi.cpp                  \
                                EditNote.cpp                  \
                                Key.cpp                       \
                                MidiAttr.cpp                  \
                                MidiPart.cpp                  \
                                Note.cpp                      \
                                RulerMidi.cpp
SOURCES_libengine            := AudioCenter.cpp               \
                                AudioEngine.cpp               \
                                Device.cpp                    \
                                Settings.cpp
SOURCES_libfileloader        := FileLoader.cpp                \
                                TreeItemData.cpp              \
                                FileStat.cpp
SOURCES_libmidi              := MidiDevice.cpp                \
                                MidiFile.cpp                  \
                                MidiInDevice.cpp              \
                                MidiThread.cpp                \
                                MidiController.cpp
SOURCES_libmixer             := Channel.cpp                   \
                                Mixer.cpp
SOURCES_libplugins           := HostCallback.cpp              \
                                PluginLoader.cpp
SOURCES_libredist            := Plugin.cpp                    \
                                Polyphony.cpp
SOURCES_libundo              := Visitor.cpp                   \
                                cAction.cpp                   \
                                cActionManager.cpp            \
                                cImportMidiAction.cpp
SOURCES_libwiredxml          := WiredXml.cpp
SOURCES_libwiredsamplerate   := WiredSampleRate.cpp           \
                                WiredSampleRateTypes.cpp
SOURCES_libwiredcodec        := WiredCodec.cpp
SOURCES_libwiredvideo        := WiredVideo.cpp                \
                                WiredMplayer.cpp
SOURCES_libwireddssi         := WiredExternalPluginMgr.cpp    \
                                WiredExternalPlugin.cpp       \
                                WiredExternalPluginLADSPA.cpp \
                                WiredExternalPluginGui.cpp    \
                                Plugin.cpp
SOURCES_libwiredakai         := akai.cpp                      \
                                list.cpp
SOURCES_libwiredwidgets      := ChoiceButton.cpp              \
                                DownButton.cpp                \
                                FaderCtrl.cpp                 \
                                HoldButton.cpp                \
                                IdButton.cpp                  \
                                KnobCtrl.cpp                  \
                                StaticBitmap.cpp              \
                                VUMCtrl.cpp                   \
                                Hint.cpp                      \
                                HintedKnob.cpp                \
                                StaticPosKnob.cpp             \
                                CycleKnob.cpp
SOURCES_libwiredbeatbox      := Polyphony.cpp                 \
                                BeatBox.cpp                   \
                                BeatButton.cpp                \
                                BeatBoxChannel.cpp            \
                                BeatBoxView.cpp               \
                                BeatNote.cpp
SOURCES_libwiredcrusher      := CrusherPlug.cpp
SOURCES_libwireddelay        := DelayPlug.cpp
SOURCES_libwiredloopsampler  := LoopSampler.cpp               \
                                LoopSamplerView.cpp           \
                                WaveLoop.cpp                  \
                                RulerLoop.cpp                 \
                                Slice.cpp                     \
                                SliceGui.cpp                  \
                                BeatDialog.cpp                \
                                LoopCursor.cpp
SOURCES_libwiredakaisampler  := AkaiSampler.cpp               \
                                ASClavier.cpp                 \
                                ASPlugin.cpp                  \
                                ASEnvel.cpp                   \
                                ASWaveView.cpp                \
                                ASKey.cpp                     \
                                ASList.cpp                    \
                                ASPlugPanel.cpp               \
                                ASSampleList.cpp              \
                                ASKeygroupEditor.cpp          \
                                ASKeygroupList.cpp            \
                                ASLoop.cpp
SOURCES_libwiredreverb       := Stk.cpp                       \
                                Effect.cpp                    \
                                Filter.cpp                    \
                                PRCRev.cpp                    \
                                NRev.cpp                      \
                                JCRev.cpp                     \
                                Delay.cpp                     \
                                ReverbPlug.cpp
SOURCES_libwiredchorus       := ChorusPlug.cpp                \
                                Effect.cpp                    \
                                WvLoop.cpp                    \
                                WvIn.cpp                      \
                                WvOut.cpp                     \
                                DelayL.cpp                    \
                                Delay.cpp                     \
                                Stk.cpp                       \
                                Filter.cpp                    \
                                Chorus.cpp
SOURCES_libwiredwahwah       := WahwahPlug.cpp

INCPATHS   := .                                        \
              $(SRCPATH)                               \
              $(SRCPATH)/gui                           \
              $(SRCPATH)/codec                         \
              $(SRCPATH)/codec/wiredcodec              \
              $(SRCPATH)/codec/wiredcodec/vorbis       \
              $(SRCPATH)/codec/wiredcodec/flac         \
              $(SRCPATH)/codec/wiredcodec/mpeg         \
              $(SRCPATH)/dssi                          \
              $(SRCPATH)/include                       \
              $(SRCPATH)/editmidi                      \
              $(SRCPATH)/portaudio/include             \
              $(SRCPATH)/portaudio/pablio              \
              $(SRCPATH)/portaudio/src/common          \
              $(SRCPATH)/portaudio/src/hostapi/dsound  \
              $(SRCPATH)/portaudio/src/os/win          \
              $(SRCPATH)/sequencer                     \
              $(SRCPATH)/xml                           \
              $(SRCPATH)/fileloader                    \
              $(SRCPATH)/engine                        \
              $(SRCPATH)/midi                          \
              $(SRCPATH)/midi/portmidi/porttime        \
              $(SRCPATH)/midi/portmidi/pm_common       \
              $(SRCPATH)/midi/portmidi/pm_win          \
              $(SRCPATH)/samplerate                    \
              $(SRCPATH)/redist                        \
              $(SRCPATH)/save                          \
              $(SRCPATH)/mixer                         \
              $(SRCPATH)/wiredvideo                    \
              $(SRCPATH)/undo                          \
              $(SRCPATH)/audio                         \
              $(SRCPATH)/plugins                       \
              $(SRCPATH)/plugins/reverb                \
              $(SRCPATH)/plugins/synth                 \
              $(SRCPATH)/plugins/filter                \
              $(SRCPATH)/plugins/akai_sampler          \
              $(SRCPATH)/plugins/beatbox               \
              $(SRCPATH)/plugins/crusher               \
              $(SRCPATH)/plugins/wahwah                \
              $(SRCPATH)/plugins/loop_sampler          \
              $(SRCPATH)/plugins/chorus                \
              $(SRCPATH)/plugins/delay                 \
              $(SRCPATH)/plugins/compressor            \
              $(SRCPATH)/libs/WiredAkai/include        \
              $(SRCPATH)/libs/WiredAkai/src            \
              $(SRCPATH)/libs/WiredWidgets/src

LIBSBIN    := $(addprefix $(BINPATH)/, $(WIREDLIBS))
PLUGBIN    := $(addprefix $(BINPATH)/, $(WIREDPLUG))
VPATH      := $(BINPATH) $(INCPATHS)

.PHONY: all plugins install clean

all: $(BINPATH) $(BINPATH)/wired$(EXE) plugins

$(BINPATH)/wired$(EXE): $(LIBSBIN) $(RESFILE)
	@echo "Linking $@"
ifeq ($(DLL),dll)
	@gcc -s -o $@ $(+:.$(DLL)=.a) -lstdc++ -lsamplerate -lSoundTouch -lsndfile -lportaudio -lportmidi            $(PLATLIBS) $(XMLLIBS) $(WXLIBS)
else
	@gcc -s -o $@ $+              -lstdc++ -lsamplerate -lSoundTouch -lsndfile -lportaudio -lportmidi -lporttime $(PLATLIBS) $(XMLLIBS) $(WXLIBS)
endif

plugins: $(BINPATH) $(PLUGBIN)

$(BINPATH):
	@mkdir --parents $(BINPATH)

$(RESFILE): resource.rc
	@windres -o $@ $<

$(BINPATH)/%.o $(BINPATH)/%.d: %.cpp
	@echo " ... compiling $<"
	@gcc $(addprefix -I,$(INCPATHS)) $(XMLCFLAGS) $(WXCFLAGS) -MMD -O2 -c -o $(@:.d=.o) $< #-Wall

install:
	@mkdir --parents $(PREFIX)
	@mkdir --parents $(PREFIX)/plugins
	@mkdir --parents $(PREFIX)/data/wired
	@mkdir --parents $(PREFIX)/conf
	@cp $(BINPATH)/wired$(EXE)            $(PREFIX)
	@cp $(BINPATH)/libwiredakai.$(DLL)    $(PREFIX)
	@cp $(BINPATH)/libwiredwidgets.$(DLL) $(PREFIX)
	@cp $(PLUGBIN)                        $(PREFIX)/plugins
	@cp -r $(SRCPATH)/data/*              $(PREFIX)/data/wired
	@find $(PREFIX)/data/wired -name Makefile.am | xargs rm
	@cp -r $(CONFPATH)/*.conf             $(PREFIX)/conf

clean:
	@rm    --force $(BINPATH)/*
	@echo " ... cleaned!"

###############################################################################
# It does not work (*** target pattern contains no `%'.  Stop.)
# For that reason, I had to replicate the libraries targets. :-(
# .SECONDEXPANSION:
# $(BINPATH)/%.a: $$(SOURCES_$$(*F):.cpp=.o)
# 	@echo " ... archiving $@"
# 	@ar cru $@ $+
# 	@ranlib $@
###############################################################################
$(BINPATH)/libsave.a: $(addprefix $(BINPATH)/,$(SOURCES_libsave:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libgui.a: $(addprefix $(BINPATH)/,$(SOURCES_libgui:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libsequencer.a: $(addprefix $(BINPATH)/,$(SOURCES_libsequencer:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libaudio.a: $(addprefix $(BINPATH)/,$(SOURCES_libaudio:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libeditmidi.a: $(addprefix $(BINPATH)/,$(SOURCES_libeditmidi:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libengine.a: $(addprefix $(BINPATH)/,$(SOURCES_libengine:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libfileloader.a: $(addprefix $(BINPATH)/,$(SOURCES_libfileloader:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libmidi.a: $(addprefix $(BINPATH)/,$(SOURCES_libmidi:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libmixer.a: $(addprefix $(BINPATH)/,$(SOURCES_libmixer:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libplugins.a: $(addprefix $(BINPATH)/,$(SOURCES_libplugins:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libredist.a: $(addprefix $(BINPATH)/,$(SOURCES_libredist:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libundo.a: $(addprefix $(BINPATH)/,$(SOURCES_libundo:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libwiredxml.a: $(addprefix $(BINPATH)/,$(SOURCES_libwiredxml:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libwiredsamplerate.a: $(addprefix $(BINPATH)/,$(SOURCES_libwiredsamplerate:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libwiredcodec.a: $(addprefix $(BINPATH)/,$(SOURCES_libwiredcodec:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libwiredvideo.a: $(addprefix $(BINPATH)/,$(SOURCES_libwiredvideo:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libwireddssi.a: $(addprefix $(BINPATH)/,$(SOURCES_libwireddssi:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libwiredakai.$(DLL): $(addprefix $(BINPATH)/,$(SOURCES_libwiredakai:.cpp=.o))
	@echo " ... linking $@"
ifeq ($(DLL),dll)
	@gcc -shared -s -o $@ $+ -lstdc++ $(WXLIBS) -Wl,--out-implib=$(@:.$(DLL)=.a)
else
	@gcc -shared -s -o $@ $+ -lstdc++ $(WXLIBS)
endif
$(BINPATH)/libwiredwidgets.$(DLL): $(addprefix $(BINPATH)/,$(SOURCES_libwiredwidgets:.cpp=.o))
	@echo " ... linking $@"
ifeq ($(DLL),dll)
	@gcc -shared -s -o $@ $+ -lstdc++ $(WXLIBS) -Wl,--out-implib=$(@:.$(DLL)=.a)
else
	@gcc -shared -s -o $@ $+ -lstdc++ $(WXLIBS)
endif
$(BINPATH)/libwiredbeatbox.$(DLL): $(addprefix $(BINPATH)/,$(SOURCES_libwiredbeatbox:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(BINPATH)/libsave.a $(BINPATH)/libaudio.a $(BINPATH)/libgui.a $(BINPATH)/libengine.a $(PLUGINSDEP) $(BINPATH)/libredist.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPATH)/libwiredcrusher.$(DLL): $(addprefix $(BINPATH)/,$(SOURCES_libwiredcrusher:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(BINPATH)/libsave.a $(BINPATH)/libaudio.a $(BINPATH)/libgui.a $(BINPATH)/libengine.a $(PLUGINSDEP) $(BINPATH)/libredist.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPATH)/libwireddelay.$(DLL): $(addprefix $(BINPATH)/,$(SOURCES_libwireddelay:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(BINPATH)/libsave.a $(BINPATH)/libaudio.a $(BINPATH)/libgui.a $(BINPATH)/libengine.a $(PLUGINSDEP) $(BINPATH)/libredist.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPATH)/libwiredloopsampler.$(DLL): $(addprefix $(BINPATH)/,$(SOURCES_libwiredloopsampler:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(BINPATH)/libsave.a $(BINPATH)/libaudio.a $(BINPATH)/libgui.a $(BINPATH)/libengine.a $(PLUGINSDEP) $(BINPATH)/libredist.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPATH)/libwiredakaisampler.$(DLL): $(addprefix $(BINPATH)/,$(SOURCES_libwiredakaisampler:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(BINPATH)/libsave.a $(BINPATH)/libaudio.a $(BINPATH)/libgui.a $(BINPATH)/libengine.a $(PLUGINSDEP) $(BINPATH)/libredist.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPATH)/libwiredreverb.$(DLL): $(addprefix $(BINPATH)/,$(SOURCES_libwiredreverb:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(BINPATH)/libsave.a $(BINPATH)/libaudio.a $(BINPATH)/libgui.a $(BINPATH)/libengine.a $(PLUGINSDEP) $(BINPATH)/libredist.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPATH)/libwiredchorus.$(DLL): $(addprefix $(BINPATH)/,$(SOURCES_libwiredchorus:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(BINPATH)/libsave.a $(BINPATH)/libaudio.a $(BINPATH)/libgui.a $(BINPATH)/libengine.a $(PLUGINSDEP) $(BINPATH)/libredist.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPATH)/libwiredwahwah.$(DLL): $(addprefix $(BINPATH)/,$(SOURCES_libwiredwahwah:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(BINPATH)/libsave.a $(BINPATH)/libaudio.a $(BINPATH)/libgui.a $(BINPATH)/libengine.a $(PLUGINSDEP) $(BINPATH)/libredist.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
###############################################################################

ifneq "$(MAKECMDGOALS)" "clean"
-include $(BINPATH)/*.d
endif
