#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <wx/wx.h>
#include <string> 
#include <list> 

class Plugin;

typedef long (*HostInterface)(Plugin *plug, long param, void *value);

#define PLUG_INIT	"init"
#define PLUG_DESTROY	"destroy"
#define PLUG_CREATE	"create"
 
#define PLUG_IS_INSTR	0x0
#define PLUG_IS_EFFECT	0x1

#define WIRED_MAKE_STR(x, y) { x[0] = y[0];  x[1] = y[1];  x[2] = y[2];  x[3] = y[3]; }

typedef struct  
{
  char	 UniqueId[4];	// Must be *unique*
  std::string Name;
  int	 Type;
  int	 UnitsX;
  int	 UnitsY;
}	        PlugInitInfo;


typedef struct  
{
  HostInterface HostCallback;
  float		Version;
  wxWindow	*Rack;
  wxPoint	Pos;
  wxSize	Size;
}		PlugStartInfo;

#define WIRED_MIDI_EVENT	1

typedef struct  
{
  long		Type;	
  long		DeltaFrames;
  long		NoteLength;
  int		MidiData[3];
}		WiredEvent;

// Sequencer event types
typedef struct
{
  double	Position;	// position relative to pattern position
  double	EndPosition;	// length of note
  int		MidiMsg[3];	// MIDI Data
}		SeqCreateEvent;

// Parameters code list
enum
  {
    // Product events
    wiredHostProductName = 0,
    wiredHostProductVersion,
    wiredHostVendorName,
    wiredGetDataDir,

    // User interface events
    wiredSendHelp,
    wiredSendMouseEvent,
    wiredSendKeyEvent,
    wiredSendClickEvent,
    wiredShowOptionalView,
    wiredCloseOptionalView,

    // Time events
    wiredGetBpm,
    wiredGetSigNumerator,
    wiredGetSigDenominator,
    wiredGetSamplePos,
    wiredGetBarPos,
    wiredGetSamplesPerBar,
    wiredGetBarsPerSample,

    // Sequencer events
    wiredCreateMidiPattern
  };

class Plugin: public wxWindow
{
 public:
  Plugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~Plugin();

  virtual void	 Init() {}

  virtual void	 Play() {}
  virtual void	 Stop() {}

  virtual void	 Load(int fd, long size) {}
  virtual long	 Save(int fd) { return (0); }

  virtual void	 SetBufferSize(long size) {}
  virtual void	 SetSamplingRate(double rate) {}

  virtual void	 Process(float **input, float **output, long sample_length) 
    {
      long i;
      for (i = 0; i < sample_length; i++)
	{
	  output[0][i] = input[0][i];
	  output[1][i] = input[1][i];
	}
    }

  virtual void	 ProcessEvent(WiredEvent &event) {}

  virtual bool	 HasView() { return false; }
  virtual wxWindow *CreateView(wxWindow *zone, wxPoint &pos, wxSize &size) { return 0x0; } 
  virtual void	 DestroyView() {}
  
  virtual bool	 IsAudio() = 0;
  virtual bool   IsMidi() = 0;

  virtual std::string GetHelpString() { return "No help provided for this plugin"; }

  virtual void SetHelpMode(bool On) { }

  virtual std::string DefaultName() { return "Rack"; }

  virtual void	OnKeyEvent(wxKeyEvent &event);
  virtual void  OnMouseEvent(wxMouseEvent &event);

  virtual wxBitmap *GetBitmap() = 0;
  
  // Time events
  float		GetBpm();
  int		GetSigNumerator();
  int		GetSigDenominator();
  unsigned long	GetSamplePos();
  double	GetSamplesPerBar();
  double	GetBarsPerSample();
  double	GetBarPos();

  /*
  // Parameters
  void		SetNumberOfParameters(int value);
  string	GetParameterName(int index);
  */
  
  // User interface events
  void SendHelp(std::string str);
  void SendMouseEvent(wxMouseEvent &event);
  void SendKeyEvent(wxKeyEvent &event);
  void SendClickEvent(wxMouseEvent &event);

  void ShowOptionalView();
  void CloseOptionalView();
  
  // Host info
  std::string GetHostProductName();
  float  GetHostProductVersion();
  std::string GetHostVendorName();
  std::string GetDataDir();

  // Sequencer events
  bool CreateMidiPattern(std::list<SeqCreateEvent *> *l);

  std::string	 Name;

  PlugStartInfo StartInfo;
  PlugInitInfo 	*InitInfo;
};

typedef PlugInitInfo (*init_t)();
typedef Plugin *(*create_t)(PlugStartInfo *);
typedef void (*destroy_t)(Plugin *);

#endif
