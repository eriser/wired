#ifndef __ASPLUGPANEL_H__
#define __ASPLUGPANEL_H__

#include <wx/wx.h>
#include "ASPlugin.h"
#include "DownButton.h"
#include "Plugin.h"
#include <vector>

using namespace std;

#define OPT_TOOLBAR_HEIGHT	(17)
#define OPT_TOOLBAR_BORDER	(2)

#define OPT_LIST_TOOL_UP	"ihm/opt/opt_list_up.png"
#define OPT_LIST_TOOL_DOWN	"ihm/opt/opt_list_down.png"
#define OPT_DETACH_TOOL_UP	"ihm/opt/opt_detach_up.png"
#define OPT_DETACH_TOOL_DOWN	"ihm/opt/opt_detach_down.png"
#define OPT_CLOSE_TOOL_UP	"ihm/opt/opt_close_up.png"
#define OPT_CLOSE_TOOL_DOWN	"ihm/opt/opt_close_down.png"

#define OPT_TOOL_ID_START	(8042)

enum
  {
    ID_TOOL_OTHER_OPTIONPANEL = 11142,
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

class ASPlug;
class ASPlugPanel;

class ASPlugFrame: public wxFrame
{
 protected:
  ASPlug      *Plug;

  void				OnClose(wxCloseEvent &event);

 public:
  ASPlugFrame(ASPlug *t, wxString s) :
    wxFrame(NULL, -1, s, wxDefaultPosition, wxSize(400, 200)),
    Plug(t) 
    {
      Connect(GetId(), wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)(wxEventFunction)
	      (wxCloseEventFunction) &ASPlugFrame::OnClose);
    }
  ~ASPlugFrame() {}
};

class ASPlug
{
 public:
  ASPlug(class ASPlugPanel *aspp, wxString name, int type, wxWindow *panel);
  ~ASPlug();

  void				Attach();
  void				Detach();

  wxString			Name;
  int				  Type;
  wxWindow		*Panel;
  bool				IsDetached;
  ASPlugFrame	*Frame;
  ASPlugin *Plugin;
  void				OnClose(wxCloseEvent &event);
  class ASPlugPanel *aspp;
};

class ASPlugPanel: public wxPanel
{
 public:
  ASPlugPanel(wxWindow *parent, const wxPoint &pos, const wxSize &size, long style, Plugin *p);
  ~ASPlugPanel();

  void				AddPlug(ASPlugin *p);
  void				ShowPlug(ASPlug *t);
  void				ShowPlugin(ASPlugin *p);
  void				ShowLastPlug();

  void				RemovePlugin(ASPlugin *p);
  void				DeletePlugs();

 protected:
  wxPanel			*ToolbarPanel;
  wxStaticText			*Title;
  Plugin      *p;
  wxBoxSizer			*TopSizer;
  DownButton			*DetachPlugBtn;
  vector<ASPlug *>		PlugsList;
  ASPlug        *CurrentPlug;
  void				OnDetachPlugClick(wxCommandEvent &event);

  DECLARE_EVENT_TABLE()
};


#endif
