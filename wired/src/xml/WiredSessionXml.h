#ifndef __WIREDSESSIONSXML_H__
#define __WIREDSESSIONSXML_H__

#include "WiredXml.h"

#include <iostream>

#include <string>
#include <map>
#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>
#include <libxml/valid.h>
//#include <libxml/tree.h>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <wx/filename.h>

#include <errno.h>

#include "../plugins/PluginLoader.h"
#include "../redist/Plugin.h"
#include "../sequencer/Track.h"
#include "../sequencer/Sequencer.h"
#include "../gui/SeqTrack.h"
#include "../gui/SeqTrackPattern.h"
#include "../gui/Pattern.h"
#include "../gui/AudioPattern.h"
#include "../gui/SequencerGui.h"
#include "../gui/MidiPattern.h"
#include "../gui/Rack.h"
#include "../midi/midi.h"
#include "../gui/Transport.h"
#include "../engine/AudioCenter.h"

//For Windows future ...

//#ifdefined	WIN32
//#include <io.h>
//#include <direct.h>
//#pragma comment(lib, "libxml2.lib")
//#define FLAGS_OPEN_RDONLY _O_RDONLY
//#define FLAGS_OPEN_CW _O_CREAT | _O_TRUNC | _O_WRONLY , _S_IRUSR | _S_IWUSR
//#else
#include <unistd.h>
#define FLAGS_OPEN_RDONLY O_RDONLY
#define FLAGS_OPEN_CW  O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR
//#endif

#define INVALID_FD -1
#define DTD_FILENAME wxT("WiredSession.dtd")
#define XML_EXTENSION wxT(".xml")
#define DTD_EXTENSION wxT(".dtd")
#define STR_TRUE wxT("true")
#define STR_FALSE wxT("false")
#define STR_AUDIO wxT("Audio")
#define STR_MIDI wxT("MIDI")


// Defines under are nodes ID, referenced in WiredSession.dtd

#define	STR_ROOT_NODE_NAME wxT("Wired-Project")

#define STR_WORKING_DIR	wxT("WorkingDir")

#define	NB_SEQUENCEUR_PARAM 7
#define STR_SEQUENCEUR wxT("Sequencer")
#define 	STR_LOOP wxT("Loop")
#define 	STR_CLICK wxT("Click")
#define 	STR_BPM wxT("BPM")
#define 	STR_SIG_NUM wxT("SigNumerator")
#define 	STR_SIG_DEN wxT("SigDenominator")
#define 	STR_BEGIN_LOOP wxT("BeginLoop")
#define 	STR_END_LOOP wxT("EndLoop")

#define	NB_AUDIO_TRACK_PARAM 3
#define	NB_MIDI_TRACK_PARAM 4
#define STR_TRACK wxT("Track")
#define 	STR_TRACK_TYPE wxT("TrackType")
#define		STR_MUTED wxT("Muted")
#define 	STR_RECORDING wxT("Recording")

#define	STR_PLUGIN wxT("Plugin")
#define		STR_PLUGIN_ID wxT("PluginId")
#define		STR_PLUGIN_DATA wxT("PluginData")
#define		STR_PLUGIN_DATA_PARAM_NAME wxT("ParamName")
#define		STR_PLUGIN_DATA_PARAM_VALUE wxT("ParamValue")

#define STR_PATTERN wxT("Pattern")

#define STR_PATTERN_DATA wxT("PatternData")
#define 	STR_PATTERN_AUDIO_DATA wxT("PatternAudioData")
#define 	STR_PATTERN_MIDI_DATA wxT("PatternMIDIData")
#define		STR_PPQN wxT("PPQN")
#define 	STR_MIDI_MESSAGE wxT("MIDIMessage")

#define	STR_WIDTH wxT("Width")
#define	STR_HEIGHT wxT("Height")

#define STR_NAME wxT("Name")
#define	STR_START_POS wxT("StartPos")
#define STR_CURRENT_POS wxT("CurrentPos")
#define STR_END_POS wxT("EndPos")
#define STR_DEVIDE_ID wxT("DeviceId")
#define STR_FILENAME wxT("FileName")



typedef std::vector<Track *>::iterator 		TrackIter;
typedef std::vector<Pattern *>::iterator 	PatternIter;
typedef std::vector<MidiEvent *>::iterator 	MidiEventIter;
typedef std::list<RackTrack *>::iterator	RackIter;
typedef std::list<Plugin *>::iterator		PluginIter;

typedef struct  s_PatternXml
{
	double			Position;
	double			EndPosition;
	long			NameLen;
	wxString		Name;
	int				TrackNumber;
} t_PatternXml;

typedef struct  s_AudioPatternXml
{
	long			StartWavePos;
	long			EndWavePos;
	wxString		FileName;
} t_AudioPatternXml;

//TODO Uncomment this block when WiredSession class has been removed

//typedef struct  s_MidiEventXml
//{
//	double	Position;
//	double	EndPosition;
//	int		Msg[3];
//} t_MidiEvent;

typedef struct  s_MidiPatternXml
{
	unsigned short 				PPQN;
	std::vector<MidiEvent *>	Events;
} t_MidiPatternXml;

typedef std::map<wxString, wxString>::iterator PluginParamsIter;

extern int errno;

typedef struct	s_PluginXml
{
	wxString		Id;
	wxString		Name;
	int				Width;
	int				Height;
	WiredPluginData	Data;
} t_PluginXml;

class WiredSessionXml : public WiredXml
{
public:
	WiredSessionXml() {;}
	WiredSessionXml(const wxString& FileName, const wxString& AudioDir = wxString(wxT(""), *wxConvCurrent)) {_WorkingDir = AudioDir, _DocumentFileName = FileName;}
	~WiredSessionXml();
	WiredSessionXml(const WiredSessionXml& copy);
	WiredSessionXml			operator=(const WiredSessionXml& right);

	WiredSessionXml			Clone();
	bool					Load(const wxString& FileName = wxString(wxT(""), *wxConvCurrent));
	bool					Save();
	bool					CreateFile();
	void					Dumpfile(const wxString& FileName);
	wxString&      			GetAudioDir();
private:
	bool					SaveSeq(); // Saving Sequenceur infos to XML File
	bool					SaveTrack(Track* TrackInfo); // Saving Track infos to XML File
	bool					SavePattern(Pattern* PatternInfo, bool AudioTrack); //Saving Pattern infos to XML File
	bool					SavePatternAudioData(AudioPattern* PatternInfo); //Saving Audio Pattern Data to XML File
	bool					SavePatternMIDIData(MidiPattern* PatternInfo); //Saving MIDI Pattern Data to XML File
	bool					SaveTrackPlugins(Track* TrackInfo);
	bool					SaveFreePlugins(); // for plugins not attached to a track
	bool					IsPluginConnected(Plugin *Plug);
	bool					SavePlugin(Plugin* PluginInfo);
	void					LoadWorkingDir();
	void					LoadSeq();
	void					LoadTrack(int Number);
	void					LoadTrackPlugin(Track* TrackInfo, t_PluginXml *PluginInfo);
	void					LoadPlugin(Track* TrackInfo);
	void					LoadPluginData(t_PluginXml *Params);
	void					LoadPattern(Track *AddedTrack, int TrackNumber);
	void					LoadPatternAudio(Track *AddedTrack, t_PatternXml *InfoPattern);
	void					LoadPatternMIDI(Track *AddedTrack, t_PatternXml *InfoPattern);
	bool					ParseWiredSession();

	wxString   				_WorkingDir;
};

#endif
