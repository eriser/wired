// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __OPTIONPANEL_H__
#define __OPTIONPANEL_H__

#include <wx/wx.h>
#include <wx/string.h>
#include "WaveFile.h"
#include "EditMidi.h"
#include "MixerGui.h"
#include "DownButton.h"
#include "AudioPattern.h"
#include "MidiPattern.h"
#include "HelpPanel.h"
#include "Plugin.h"
#include "EditMidi.h"

#define OPT_TOOLBAR_HEIGHT	(17)
#define OPT_TOOLBAR_BORDER	(2)

#define OPT_LIST_TOOL_UP	"ihm/opt/opt_list_up.png"
#define OPT_LIST_TOOL_DOWN	"ihm/opt/opt_list_down.png"
#define OPT_DETACH_TOOL_UP	"ihm/opt/opt_detach_up.png"
#define OPT_DETACH_TOOL_DOWN	"ihm/opt/opt_detach_down.png"
#define OPT_CLOSE_TOOL_UP	"ihm/opt/opt_close_up.png"
#define OPT_CLOSE_TOOL_DOWN	"ihm/opt/opt_close_down.png"

#define OPT_TOOL_ID_START	(4042)

enum
  {
    ID_TOOL_OTHER_OPTIONPANEL = 1142,
    ID_TOOL_AUDIO_OPTIONPANEL,
    ID_TOOL_MIDI_OPTIONPANEL,
    ID_TOOL_MIXER_OPTIONPANEL,
    ID_TOOL_HELP_OPTIONPANEL,
    ID_TOOL_LIST_OPTIONPANEL,
    ID_TOOL_DETACH_OPTIONPANEL,
    ID_TOOL_CLOSE_OPTIONPANEL,
    ID_TOOL_MOVE_OPTIONPANEL,
    ID_TOOL_EDIT_OPTIONPANEL,
    ID_TOOL_DEL_OPTIONPANEL
  };

class WiredTool;

class WiredFrame: public wxFrame
{
 protected:
  WiredTool			*Tool;

  void				OnClose(wxCloseEvent &event);
  void				OnToolMove(wxCommandEvent &);
  void				OnToolDel(wxCommandEvent &);
  void				OnToolEdit(wxCommandEvent &);

 public:
  WiredFrame(WiredTool *t, wxString s) :
    wxFrame(0x0, -1, s, wxDefaultPosition, wxSize(400, 200)),
    Tool(t) 
    {
      em = NULL;
      Connect(GetId(), wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)(wxEventFunction)
	      (wxCloseEventFunction) &WiredFrame::OnClose);
    }
  EditMidi			*em;
  ~WiredFrame() {}
  DECLARE_EVENT_TABLE();
};

class WiredTool
{
 public:
  WiredTool(wxString name, int type, wxWindow *panel);
  ~WiredTool();

  void				Attach();
  void				Detach();

  wxString			Name;
  int				Type;
  wxWindow			*Panel;

  bool				IsDetached;
  WiredFrame			*Frame;

  void				*Data;

  void				OnClose(wxCloseEvent &event);
};

class OptionPanel : public wxPanel
{
 public:
  OptionPanel(wxWindow *parent, const wxPoint &pos, const wxSize &size, long style);
  ~OptionPanel();

  void				AddAudioTool(AudioPattern *p);
  void				AddMidiTool(MidiPattern *p);
  void				AddPlugTool(Plugin *p);

  void				ShowTool(WiredTool *t);
  void				ShowWave(AudioPattern *p);
  void				ShowMidi(MidiPattern *p);
  void				ShowPlug(Plugin *p);
  void				ShowLastTool();

  void				ClosePlug(Plugin *p);
  void				DeleteTools();

 protected:
  wxPanel			*ToolbarPanel;
  wxStaticText			*Title;

  wxBoxSizer			*TopSizer;

  DownButton			*ListToolBtn;
  DownButton			*DetachToolBtn;
  DownButton			*CloseToolBtn;

  vector<WiredTool *>		ToolsList;
  WiredTool			*CurrentTool;
  WiredTool			*MixerTool;
  WiredTool			*HelpTool;

  void				OnListToolClick(wxCommandEvent &event);
  void				OnDetachToolClick(wxCommandEvent &event);
  void				OnCloseToolClick(wxCommandEvent &event);
  void				OnSelectTool(wxCommandEvent &event);

  DECLARE_EVENT_TABLE()
};

extern OptionPanel		*OptPanel;

#endif
