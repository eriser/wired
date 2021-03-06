// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

////
// Current version of API
//
// this number increment only when methods or function of API are modified
//
#define WIRED_CURRENT_VERSION_API (1)


#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <string>
#include <list>
#include <vector>
#include <map>
#include "Plugin.h"
#include "SaveCenter.h"

// methods are mainly in this class
class Plugin;

// called to check name, version,.. of plugin
#define PLUG_INIT	wxT("init")

// called to create one instance
#define PLUG_CREATE	wxT("create")

// called to destroy one instance
#define PLUG_DESTROY	wxT("destroy")

// determine type of plugin
enum ePlugType {
  ePlugTypeUnknown = 0,
  ePlugTypeInstrument,
  ePlugTypeEffect
};

#define WIRED_MAKE_STR(x, y) { x[0] = y[0];  x[1] = y[1];  x[2] = y[2];  x[3] = y[3]; }

/* You need to return this struct to the host when it calls the 'init' function */
typedef struct
{
  // ID of your plugin. Must be *unique* AND ALPHA (not numeric)
  char	 UniqueId[4];

  // ID of an external plugin. Must not be filled if you want to comply with Wired Old API
  unsigned long	UniqueExternalId;

  // Your plugin name
  wxString	Name;

  // Type of plugin (ePlugTypeInstrument for an instrument or ePlugTypeEffect, ..)
  int		Type;

  // Number of units for the width of your plugin (1 unit is 200 pixels)
  int		UnitsX;

  // Number of units for the height of your plugin (1 unit is 100 pixels)
  int		UnitsY;

  // version of API used
  int		Version;
}	        PlugInitInfo;

/* Host give to you this struct to create a new instance of your plugin */
typedef long (*HostInterface)(Plugin *plug, long param, void *value);
typedef struct  s_PlugStartInfo
{
  HostInterface HostCallback;
  wxString	Version;
  wxWindow	*Rack;
  wxPoint	Pos;
  wxSize	Size;
  SaveCenter	*saveCenter;
  WiredDocument	*parent;
}		PlugStartInfo;

#define WIRED_MIDI_EVENT	1

/* Structure that is passed when your plugin receives an event (such as MIDI messages) */
typedef struct
{
  // Event type (i.e WIRED_MIDI_EVENT above)
  long		Type;
  // The number of frames the event will start on next call to Process()
  long		DeltaFrames;
  // The note length (usually 0, look for note off messages to know the end of a note
  long		NoteLength;
  // The MIDI data
  int		MidiData[3];
}		WiredEvent;

// Sequencer event types, used to create MIDI patterns on the host sequencer
typedef struct	s_SeqCreateEvent
{
  // position relative to pattern position
  double	Position;
  // length of message
  double	EndPosition;
  // MIDI Data
  int		MidiMsg[3];
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
    wiredSendMotionEvent,
    wiredSendWheelEvent,
    wiredSendLeftUpEvent,
    wiredSendLeftDownEvent,
    wiredSendRightDownEvent,
    wiredSendKeyEvent,
    wiredSendPaintEvent,
    wiredShowOptionalView,
    wiredCloseOptionalView,
    wiredOpenFileLoader,
    wiredSaveFileLoader,
    wiredShowMidiController,
    wiredAskUpdateGui,

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

/* Parameter class, used for Wired with Xml compatibility.
  The only two methods you must know are SaveValue and LoadValue. */


typedef std::map<wxString, wxString> PluginParams;

class WiredPluginData
{
public:
	WiredPluginData() {;}
	~WiredPluginData() {_Data.clear();}
	WiredPluginData(const WiredPluginData& copy) {*this = copy;}
	WiredPluginData			operator=(const WiredPluginData& right);

	bool			SaveValue(const wxString& Name, char *Value);
	bool			SaveValue(const wxString& Name, wxString Value);
	const char		*LoadValue(const wxString& Name);
	PluginParams	*GetParamsStack();
private:
	PluginParams			_Data;
};

/* This is the Plugin class you should derive from to create your own plugins.
   DO NOT MODIFY THIS FILE, JUST CREATE A DERIVATE CLASS FROM IT, OVERLOADING THE
   VIRTUAL FUNCTIONS YOU NEED. */

class Plugin: public wxWindow
{
 public:
  Plugin(){}
  Plugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo);
  ~Plugin();
  Plugin&	operator=(const Plugin &right)
  {
  	if (this !=& right)
  	{
	  	Name = right.Name;
		StartInfo = right.StartInfo;
		InitInfo = right.InitInfo;
  	}
	return *this;
  }

  /* Called when the plugin is created by the host */
  virtual void	 Init() {}

  /* Called when the host starts to play */
  virtual void	 Play() {}
  /* Called when the host stopped the sequencer */
  virtual void	 Stop() {}

  /* Called by the host when the plugin needs to load existing parameters (such as in a
     .wrd file. 'fd' is the file descriptor of the file placed at the correct position,
     where the plugin should read 'size' bytes of data. */
  virtual void	 Load(int fd, long size) {}

  // ...and the xml-compatible one...
  virtual void	 Load(WiredPluginData& Datas){}

  /* Called by the host when the plugin needs to save its parameters. This function
     should return the size of the data that was written in file descriptor 'fd' */
  virtual long	 Save(int fd) { return (0); }

  // ...and the xml-compatible one...
  virtual void	 Save(WiredPluginData& Datas) {}

  /* Called when the buffer size of the host changes (it is also called after
     initialization of the plugin */
  virtual void	 SetBufferSize(long size) {}
  /* Called when the sample rate of the host changes (it is also called after
     initialization of the plugin */
  virtual void	 SetSamplingRate(double rate) {}
  /* Called by the host when the BPM changes */
  virtual void   SetBPM(float bpm) {}
  /* Called by the host when the signature changes */
  virtual void   SetSignature(int numerator, int denominator) {}

  /* This is were you do your processing. 'input' is the data you have to process (or
   not if you are making an instrument) and 'output' whre you should place the
   processed or generated. 'input' and 'output' contains two buffers (for stereo) which
   contains 'sample_length' elements */
  virtual void	 Process(float **input, float **output, long sample_length)
    {
      long i;
      for (i = 0; i < sample_length; i++)
	{
	  output[0][i] = input[0][i];
	  output[1][i] = input[1][i];
	}
    }

  /* Called when the plugin receives an event such as MIDI */
  virtual void	 ProcessEvent(WiredEvent &event) {}

  /* Called by the host to know if the plugin has an optional view or not */
  virtual bool	 HasView() { return false; }
  /* Called by the host to create the optional view */
  virtual wxWindow *CreateView(wxWindow *zone, wxPoint &pos, wxSize &size) { return 0x0; }
  /* Called when the optional view needs to be destroyed */
  virtual void	 DestroyView() {}

  /* Is plugin supporting audio data to be sent to it ? */
  virtual bool	 IsAudio() = 0;
  /* Is plugin supporting MIDI data to be sent to it ? */
  virtual bool   IsMidi() = 0;

  /* Ask the host application to call Update() whenever the main thread can process gui calls  */
  virtual void	 AskUpdate();

  /* Called by the host that the plugin can update its graphical controls */
  virtual void	 Update() {}

  /* Called when host needs to show the plugin's help */
  virtual wxString GetHelpString() { return _("No help provided for this plugin"); }

  /* Is the Help window being shown ? */
  virtual void SetHelpMode(bool On) { }

  /* Returns the default name for the plugin */
  virtual wxString DefaultName() { return _("Rack"); }

  /* Returns a 32x16 bitmap used for displaying the connected to track plugin */
  virtual wxBitmap *GetBitmap() = 0;

  /* Used to know if a keyboard event occured. No need to overload */
  /* Tells the host that a key event occured */
  virtual void	OnKeyEvent(wxKeyEvent &event);

  /* Used to know if a mouse event occured. No need to overload */
  /* Tells the host that a mouse event occured */
  virtual void  OnMotionEvent(wxMouseEvent &event);
  /* Used to know if a mouse event occured. No need to overload */
  /* Tells the host that a mouse event occured */
  virtual void  OnWheelEvent(wxMouseEvent &event);
  /* Used to know if a mouse event occured. No need to overload */
  /* Tells the host that a mouse event occured */
  virtual void  OnLeftUpEvent(wxMouseEvent &event);
  /* Used to know if a mouse event occured. No need to overload */
  /* Tells the host that a mouse event occured */
  virtual void  OnLeftDownEvent(wxMouseEvent &event);
  /* Used to know if a mouse event occured. No need to overload */
  /* Tells the host that a mouse event occured */
  virtual void  OnRightDownEvent(wxMouseEvent &event);

  /* Used to know if a paint event occured. No need to overload */
  /* Tells the host that the plugin needs to be paint */
  virtual void  OnPaintEvent(wxPaintEvent &event);

  // Time events
  /* Returns current BPM (tempo) */
  float		GetBpm();
  /* Returns the signature numerator */
  int		GetSigNumerator();
  /* Returns the signature denominator */
  int		GetSigDenominator();
  /* Returns the position in sample in the sequencer */
  unsigned long	GetSamplePos();
  /* Returns the number of samples per bar */
  double	GetSamplesPerBar();
  /* Returns the number of bars per sample */
  double	GetBarsPerSample();
  /* Returns the position in Bars of the sequencer */
  double	GetBarPos();

  /*
  // Parameters
  void		SetNumberOfParameters(int value);
  wxString	GetParameterName(int index);
  */

  // User interface events

  /* Send help wxString to the Wired help window */
  void SendHelp(wxString str);
  /* Shows MIDI controller change window, if returns true,
     MidiData will be filled by the MIDI data received (you need to pass a valid pointer
     to a int[3] variable */
  bool ShowMidiController(int *MidiData[3]);

  /* Shows plugin's optional view */
  void ShowOptionalView();
  /* Closes plugin's optional view */
  void CloseOptionalView();

  /* Opens the Wired file loader with given title, extensions, and if it should read
     AKAI audio cds/files or not. Returns the selected file name or an empty wstring if
     cancelled. If 'exts' is NULL, default audio extensions are used. */
  wxString OpenFileLoader(wxString title,
			     std::vector<wxString> *exts,
			     bool akai = false);
  /* Opens the Wired file loader with given title, extensions, for saving a file.
     Returns the file name or an empty wstring if cancelled */
  wxString SaveFileLoader(wxString title,
			     std::vector<wxString> *exts);

  // Host info

  /* Returns the host product name */
  wxString GetHostProductName();
  /* Returns the host product version */
  wxString GetHostProductVersion();
  /* Returns the host vendor name */
  wxString GetHostVendorName();
  /* Returns the path to the host data directory */
  wxString GetDataDir();

  // Sequencer events
  /* Create a MIDI pattern containing a list of event in the host's sequencer */
  bool CreateMidiPattern(std::list<SeqCreateEvent *> *l);

  // String representing the name of the plugin
  wxString	 Name;

  // Plugin startup and init information
  PlugStartInfo StartInfo;
  PlugInitInfo 	*InitInfo;
};

// MANDATORY C FUNCTIONS

// Called when the host initializes the plugin's shared library (usually at startup)
typedef PlugInitInfo (*init_t)();
// Called when the host needs to create a new instance of the plugin
typedef Plugin *(*create_t)(PlugStartInfo *);
// Called when the host needs to destroy an instance of the plugin
typedef void (*destroy_t)(Plugin *);

#endif
