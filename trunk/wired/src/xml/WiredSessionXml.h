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
#define DTD_FILENAME "WiredSession.dtd"
#define XML_EXTENSION ".xml"
#define DTD_EXTENSION ".dtd"
#define STR_TRUE "true"
#define STR_FALSE "false"
#define STR_AUDIO "Audio"
#define STR_MIDI "MIDI"


// Defines under are nodes ID, referenced in WiredSession.dtd

#define	STR_ROOT_NODE_NAME "Wired-Project"

#define STR_WORKING_DIR	"WorkingDir"

#define	NB_SEQUENCEUR_PARAM 7
#define STR_SEQUENCEUR "Sequencer"
#define 	STR_LOOP "Loop"
#define 	STR_CLICK "Click"
#define 	STR_BPM "BPM"
#define 	STR_SIG_NUM "SigNumerator"
#define 	STR_SIG_DEN "SigDenominator"
#define 	STR_BEGIN_LOOP "BeginLoop"
#define 	STR_END_LOOP "EndLoop"

#define	NB_AUDIO_TRACK_PARAM 3
#define	NB_MIDI_TRACK_PARAM 4
#define STR_TRACK "Track"
#define 	STR_TRACK_TYPE "TrackType"
#define		STR_MUTED "Muted"
#define 	STR_RECORDING "Recording"

#define	STR_PLUGIN "Plugin"
#define		STR_PLUGIN_ID "PluginId"
#define		STR_PLUGIN_DATA "PluginData"
#define		STR_PLUGIN_DATA_PARAM_NAME "ParamName"
#define		STR_PLUGIN_DATA_PARAM_VALUE "ParamValue"

#define STR_PATTERN "Pattern"

#define STR_PATTERN_DATA "PatternData"
#define 	STR_PATTERN_AUDIO_DATA "PatternAudioData"
#define 	STR_PATTERN_MIDI_DATA "PatternMIDIData"
#define		STR_PPQN "PPQN"
#define 	STR_MIDI_MESSAGE "MIDIMessage"

#define	STR_WIDTH "Width"
#define	STR_HEIGHT "Height"

#define STR_NAME "Name"
#define	STR_START_POS "StartPos"
#define STR_CURRENT_POS "CurrentPos"
#define STR_END_POS "EndPos"
#define STR_DEVIDE_ID "DeviceId"
#define STR_FILENAME "FileName"



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
	std::string		Name;
	int				TrackNumber;
} t_PatternXml;

typedef struct  s_AudioPatternXml
{
	long			StartWavePos;
	long			EndWavePos;
	std::string		FileName;
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

typedef std::map<std::string, char *>::iterator PluginParamsIter;

extern int errno;

typedef struct	s_PluginXml
{
	std::string		Id;
	std::string		Name;
	int				Width;
	int				Height;
	WiredPluginData	Data;
} t_PluginXml;

class WiredSessionXml : public WiredXml
{
public:
	WiredSessionXml() {;}
	WiredSessionXml(const std::string& FileName, const std::string& AudioDir = "") {_WorkingDir = AudioDir, _DocumentFileName = FileName;}
	~WiredSessionXml();
	WiredSessionXml(const WiredSessionXml& copy);
	WiredSessionXml			operator=(const WiredSessionXml& right);

	WiredSessionXml			Clone();
	bool					Load(const std::string& FileName = "");
	bool					Save();
	bool					CreateFile();
	void					Dumpfile(const std::string& FileName);
	const std::string&		GetAudioDir();
private:
	bool					SaveSeq(); // Saving Sequenceur infos to XML File
	bool					SaveTrack(Track* TrackInfo); // Saving Track infos to XML File
	bool					SavePattern(Pattern* PatternInfo, bool AudioTrack); //Saving Pattern infos to XML File
	bool					SavePatternAudioData(AudioPattern* PatternInfo); //Saving Audio Pattern Data to XML File
	bool					SavePatternMIDIData(MidiPattern* PatternInfo); //Saving MIDI Pattern Data to XML File
	bool					SaveTrackPlugins(Track* TrackInfo);
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

	std::string				_WorkingDir;
};

#endif
