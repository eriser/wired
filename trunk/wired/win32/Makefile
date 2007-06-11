# -----------------------------------------------------------------------------------
# Wired anternative Makefile written by Daniel Moura <oxe AT oxesoft.com> in May-2007
# -----------------------------------------------------------------------------------

SRCPATH    := ../src

ifeq ($(PREFIX),)
BINPATH := ./bin
else
BINPATH := $(PREFIX)
endif
BINPLUG := $(BINPATH)/plugins

ifeq ($(findstring MINGW32,$(shell uname)),MINGW32)
OBJPATH    := .obj-win32
DLL        := dll
EXE        := .exe
WXLIBS     := -lwx_msw-2.8 -mthreads
WXCFLAGS   := -DWXUSINGDLL -D__WXMSW__ -mthreads
XMLLIBS    := -lxml2
XMLCFLAGS  :=
PLATLIBS   := -mconsole -lwinmm -lwsock32 -Wl,-u,_WinMain@16
RESFILE    := $(OBJPATH)/resource.o
PLUGINSDEP := $(BINPATH)/libWiredWidgets.a
CONFPATH   := .
else
OBJPATH    := .obj-linux
DLL        := so
EXE        :=
WXLIBS     := $(shell wx-config --libs)
WXCFLAGS   := $(shell wx-config --cflags)
XMLLIBS    := $(shell xml2-config --libs)
XMLCFLAGS  := $(shell xml2-config --cflags)
PLATLIBS   :=
RESFILE    :=
PLUGINSDEP := $(OBJPATH)/libWiredWidgets.so
CONFPATH   := $(SRCPATH)/conf
endif

WIREDLIBS                    := $(OBJPATH)/libgui.a                   \
                                $(OBJPATH)/libwireddssi.a             \
                                $(OBJPATH)/libsave.a                  \
                                $(OBJPATH)/libsequencer.a             \
                                $(OBJPATH)/libaudio.a                 \
                                $(OBJPATH)/libeditmidi.a              \
                                $(OBJPATH)/libengine.a                \
                                $(OBJPATH)/libfileloader.a            \
                                $(OBJPATH)/libmidi.a                  \
                                $(OBJPATH)/libmixer.a                 \
                                $(OBJPATH)/libplugins.a               \
                                $(OBJPATH)/libredist.a                \
                                $(OBJPATH)/libundo.a                  \
                                $(OBJPATH)/libwiredxml.a              \
                                $(OBJPATH)/libwiredsamplerate.a       \
                                $(OBJPATH)/libwiredcodec.a            \
                                $(OBJPATH)/libwiredvideo.a            \
                                $(BINPATH)/libWiredAkai.$(DLL)        \
                                $(BINPATH)/libWiredWidgets.$(DLL)
WIREDPLUG                    := $(BINPLUG)/libWiredBeatBox.$(DLL)     \
                                $(BINPLUG)/libWiredCrusher.$(DLL)     \
                                $(BINPLUG)/libWiredDelay.$(DLL)       \
                                $(BINPLUG)/libWiredLoopSampler.$(DLL) \
                                $(BINPLUG)/libWiredSampler.$(DLL)     \
                                $(BINPLUG)/libWiredReverb.$(DLL)      \
                                $(BINPLUG)/libWiredChorus.$(DLL)      \
                                $(BINPLUG)/libWiredWahwah.$(DLL)
SOURCES_libsave              := SaveCenter.cpp                \
                                WiredDocument.cpp
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
                                FloatingFrame.cpp
SOURCES_libsequencer         := Sequencer.cpp                 \
                                Track.cpp                     \
                                SeqTrackPattern.cpp
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
SOURCES_libfileloader        := TreeItemData.cpp              \
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
                                WiredExternalPluginGui.cpp
SOURCES_libWiredAkai         := akai.cpp                      \
                                list.cpp
SOURCES_libWiredWidgets      := ChoiceButton.cpp              \
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
SOURCES_libWiredBeatBox      := Polyphony.cpp                 \
                                BeatBox.cpp                   \
                                BeatButton.cpp                \
                                BeatBoxChannel.cpp            \
                                BeatBoxView.cpp               \
                                BeatNote.cpp
SOURCES_libWiredCrusher      := CrusherPlug.cpp
SOURCES_libWiredDelay        := DelayPlug.cpp
SOURCES_libWiredLoopSampler  := LoopSampler.cpp               \
                                LoopSamplerView.cpp           \
                                WaveLoop.cpp                  \
                                RulerLoop.cpp                 \
                                Slice.cpp                     \
                                SliceGui.cpp                  \
                                BeatDialog.cpp                \
                                LoopCursor.cpp
SOURCES_libWiredSampler  :=     AkaiSampler.cpp               \
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
SOURCES_libWiredReverb       := Stk.cpp                       \
                                Effect.cpp                    \
                                Filter.cpp                    \
                                PRCRev.cpp                    \
                                NRev.cpp                      \
                                JCRev.cpp                     \
                                Delay.cpp                     \
                                ReverbPlug.cpp
SOURCES_libWiredChorus       := ChorusPlug.cpp                \
                                Effect.cpp                    \
                                WvLoop.cpp                    \
                                WvIn.cpp                      \
                                WvOut.cpp                     \
                                DelayL.cpp                    \
                                Delay.cpp                     \
                                Stk.cpp                       \
                                Filter.cpp                    \
                                Chorus.cpp
SOURCES_libWiredWahwah       := WahwahPlug.cpp

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
              $(SRCPATH)/sequencer                     \
              $(SRCPATH)/xml                           \
              $(SRCPATH)/fileloader                    \
              $(SRCPATH)/engine                        \
              $(SRCPATH)/midi                          \
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

VPATH      := $(BINPATH) $(INCPATHS)

.PHONY: all plugins clean

all: $(BINPATH) $(OBJPATH) $(BINPATH)/wired$(EXE) plugins

$(BINPATH)/wired$(EXE): $(WIREDLIBS) $(RESFILE)
	@echo "Linking $@"
ifeq ($(DLL),dll)
	@gcc -s -o $@ $(+:.$(DLL)=.a) -lstdc++ -lsamplerate -lSoundTouch -lsndfile -lportaudio -lportmidi            $(PLATLIBS) $(XMLLIBS) $(WXLIBS)
else
	@gcc -s -o $@ $+              -lstdc++ -lsamplerate -lSoundTouch -lsndfile -lportaudio -lportmidi -lporttime $(PLATLIBS) $(XMLLIBS) $(WXLIBS)
endif

plugins: $(OBJPATH) $(WIREDPLUG)

$(OBJPATH):
	@mkdir --parents $(OBJPATH)

$(BINPATH):
	@mkdir --parents $(BINPATH)
	@mkdir --parents $(BINPATH)/plugins
	@mkdir --parents $(BINPATH)/data/wired
	@mkdir --parents $(BINPATH)/conf
	@cp -r $(SRCPATH)/data/*              $(BINPATH)/data/wired
	@find $(BINPATH)/data/wired -name Makefile.am | xargs rm
	@find $(BINPATH)/data/wired -name .svn | xargs rm -rf
	@cp -r $(CONFPATH)/*.conf             $(BINPATH)/conf

$(RESFILE): resource.rc
	@windres -o $@ $<

$(OBJPATH)/%.o $(OBJPATH)/%.d: %.cpp
	@echo " ... compiling $<"
	@gcc $(addprefix -I,$(INCPATHS)) $(XMLCFLAGS) $(WXCFLAGS) -MMD -O2 -c -o $(@:.d=.o) $< #-Wall

clean:
	@rm -rf $(OBJPATH)
	@echo " ... cleaned!"

###############################################################################
# It does not work (*** target pattern contains no `%'.  Stop.)
# For that reason, I had to replicate the libraries targets. :-(
# .SECONDEXPANSION:
# $(OBJPATH)/%.a: $$(SOURCES_$$(*F):.cpp=.o)
# 	@echo " ... archiving $@"
# 	@ar cru $@ $+
# 	@ranlib $@
###############################################################################
$(OBJPATH)/libsave.a: $(addprefix $(OBJPATH)/,$(SOURCES_libsave:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libgui.a: $(addprefix $(OBJPATH)/,$(SOURCES_libgui:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libsequencer.a: $(addprefix $(OBJPATH)/,$(SOURCES_libsequencer:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libaudio.a: $(addprefix $(OBJPATH)/,$(SOURCES_libaudio:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libeditmidi.a: $(addprefix $(OBJPATH)/,$(SOURCES_libeditmidi:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libengine.a: $(addprefix $(OBJPATH)/,$(SOURCES_libengine:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libfileloader.a: $(addprefix $(OBJPATH)/,$(SOURCES_libfileloader:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libmidi.a: $(addprefix $(OBJPATH)/,$(SOURCES_libmidi:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libmixer.a: $(addprefix $(OBJPATH)/,$(SOURCES_libmixer:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libplugins.a: $(addprefix $(OBJPATH)/,$(SOURCES_libplugins:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libredist.a: $(addprefix $(OBJPATH)/,$(SOURCES_libredist:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libundo.a: $(addprefix $(OBJPATH)/,$(SOURCES_libundo:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libwiredxml.a: $(addprefix $(OBJPATH)/,$(SOURCES_libwiredxml:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libwiredsamplerate.a: $(addprefix $(OBJPATH)/,$(SOURCES_libwiredsamplerate:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libwiredcodec.a: $(addprefix $(OBJPATH)/,$(SOURCES_libwiredcodec:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libwiredvideo.a: $(addprefix $(OBJPATH)/,$(SOURCES_libwiredvideo:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(OBJPATH)/libwireddssi.a: $(addprefix $(OBJPATH)/,$(SOURCES_libwireddssi:.cpp=.o))
	@echo " ... archiving $@"
	@ar cru $@ $+
	@ranlib $@
$(BINPATH)/libWiredAkai.$(DLL): $(addprefix $(OBJPATH)/,$(SOURCES_libWiredAkai:.cpp=.o))
	@echo " ... linking $@"
ifeq ($(DLL),dll)
	@gcc -shared -s -o $@ $+ -lstdc++ $(WXLIBS) -Wl,--out-implib=$(@:.$(DLL)=.a)
else
	@gcc -shared -s -o $@ $+ -lstdc++ $(WXLIBS) -Wl,-soname=$(@F)
endif
$(BINPATH)/libWiredWidgets.$(DLL): $(addprefix $(OBJPATH)/,$(SOURCES_libWiredWidgets:.cpp=.o))
	@echo " ... linking $@"
ifeq ($(DLL),dll)
	@gcc -shared -s -o $@ $+ -lstdc++ $(WXLIBS) -Wl,--out-implib=$(@:.$(DLL)=.a)
else
	@gcc -shared -s -o $@ $+ -lstdc++ $(WXLIBS) -Wl,-soname=$(@F)
endif
$(BINPLUG)/libWiredBeatBox.$(DLL): $(addprefix $(OBJPATH)/,$(SOURCES_libWiredBeatBox:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(OBJPATH)/libsave.a $(OBJPATH)/libaudio.a $(OBJPATH)/libgui.a $(OBJPATH)/libengine.a $(PLUGINSDEP) $(OBJPATH)/libredist.a $(OBJPATH)/libwiredxml.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPLUG)/libWiredCrusher.$(DLL): $(addprefix $(OBJPATH)/,$(SOURCES_libWiredCrusher:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(OBJPATH)/libsave.a $(OBJPATH)/libaudio.a $(OBJPATH)/libgui.a $(OBJPATH)/libengine.a $(PLUGINSDEP) $(OBJPATH)/libredist.a $(OBJPATH)/libwiredxml.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPLUG)/libWiredDelay.$(DLL): $(addprefix $(OBJPATH)/,$(SOURCES_libWiredDelay:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(OBJPATH)/libsave.a $(OBJPATH)/libaudio.a $(OBJPATH)/libgui.a $(OBJPATH)/libengine.a $(PLUGINSDEP) $(OBJPATH)/libredist.a $(OBJPATH)/libwiredxml.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPLUG)/libWiredLoopSampler.$(DLL): $(addprefix $(OBJPATH)/,$(SOURCES_libWiredLoopSampler:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(OBJPATH)/libsave.a $(OBJPATH)/libaudio.a $(OBJPATH)/libgui.a $(OBJPATH)/libengine.a $(PLUGINSDEP) $(OBJPATH)/libredist.a $(OBJPATH)/libwiredxml.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPLUG)/libWiredSampler.$(DLL): $(addprefix $(OBJPATH)/,$(SOURCES_libWiredSampler:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(OBJPATH)/libsave.a $(OBJPATH)/libaudio.a $(OBJPATH)/libgui.a $(OBJPATH)/libengine.a $(PLUGINSDEP) $(OBJPATH)/libredist.a $(OBJPATH)/libwiredxml.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPLUG)/libWiredReverb.$(DLL): $(addprefix $(OBJPATH)/,$(SOURCES_libWiredReverb:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(OBJPATH)/libsave.a $(OBJPATH)/libaudio.a $(OBJPATH)/libgui.a $(OBJPATH)/libengine.a $(PLUGINSDEP) $(OBJPATH)/libredist.a $(OBJPATH)/libwiredxml.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPLUG)/libWiredChorus.$(DLL): $(addprefix $(OBJPATH)/,$(SOURCES_libWiredChorus:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(OBJPATH)/libsave.a $(OBJPATH)/libaudio.a $(OBJPATH)/libgui.a $(OBJPATH)/libengine.a $(PLUGINSDEP) $(OBJPATH)/libredist.a $(OBJPATH)/libwiredxml.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
$(BINPLUG)/libWiredWahwah.$(DLL): $(addprefix $(OBJPATH)/,$(SOURCES_libWiredWahwah:.cpp=.o))
	@echo " ... linking $@"
	@gcc -shared -s -o $@ $+ $(OBJPATH)/libsave.a $(OBJPATH)/libaudio.a $(OBJPATH)/libgui.a $(OBJPATH)/libengine.a $(PLUGINSDEP) $(OBJPATH)/libredist.a $(OBJPATH)/libwiredxml.a -lstdc++ -lsndfile $(XMLLIBS) -lSoundTouch $(WXLIBS)
###############################################################################

ifneq "$(MAKECMDGOALS)" "clean"
-include $(OBJPATH)/*.d
endif
