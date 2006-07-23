// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __WIREDSESSION_H__
#define __WIREDSESSION_H__

#include <string>
using namespace std;

#include <wx/file.h>

#define WIRED_FILE_EXT	".wrd"
#define WIRED_MAGIC	"WIRE"
/*
____________________________________
|				   |
|	       HEADER		   |
|__________________________________|
|				   |
|	    PLUGINS INFO	   |
|__________________________________|
|				   |
|	   SEQUENCER INFO	   |
|__________________________________|
|				   |
|	     TRACKS INFO	   |
|__________________________________|
|				   |
|	     MIXER INFO		   |
|__________________________________|
|				   |
|      WINDOWS & GRAPHICS INFO	   |
|__________________________________|

*/

typedef struct  s_Header
{
  char		Magic[4]; // 'W','I','R','E'
  long		AudioDirLen;
  long		NumberOfRackTracks;  
} t_Header;

typedef struct  s_RackTrack
{
  long		NumberOfPlugins;
} t_RackTrack;


typedef struct  s_Plugin
{
  char		Id[4];
  long		NameLen;
  wxChar		*Name;
  long		DataLen;
} t_Plugin;

typedef struct  s_Sequencer
{
  float		BPM;
  int		SigNumerator;
  int		SigDenominator;
  double	CurrentPos;
  double	BeginLoopPos;
  double	EndLoopPos;
  double	EndPos;
  long		NumberOfTracks;
  bool		Loop;
  bool		Click;
} t_Sequencer;

typedef struct  s_Track
{
  char		Type;		// 0: Audio, 1: Midi
  bool		Mute;
  bool		Record;
  long		DeviceId;
  long		PluginId;
  long		NameLen;
  wxChar		*Name;
  long		NumberOfPatterns;
} t_Track;

typedef struct  s_Pattern
{
  double	Position;
  double	EndPosition;
  long		NameLen;
  wxChar		*Name;
} t_Pattern;

typedef struct  s_AudioPattern
{
  long		StartWavePos;
  long		EndWavePos;
  long		FilenameLen;
  wxChar		*Filename;
} t_AudioPattern;

typedef struct  s_MidiPattern
{
  unsigned short PPQN;
  long		 NumberOfEvents;
} t_MidiPattern;

typedef struct  s_MidiEvent
{
  double	Position;
  double	EndPosition;
  int		Msg[3];
} t_MidiEvent;

class WiredSession
{
 public:
  WiredSession(wxString filename, wxString audiodir = wxT(""));
  ~WiredSession();

  bool		Load();
  bool		Save();

  wxString	FileName;
  wxString	AudioDir;
 protected:
  wxFile		confFile;
};

#endif
