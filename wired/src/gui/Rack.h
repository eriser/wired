// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __RACK_H__
#define __RACK_H__

#include <wx/wx.h>
#include <list>

using namespace			std;

#define UNIT_W			(200)
#define UNIT_H			(100)
#define UNIT_S			(2)

typedef struct s_PlugStartInfo	PlugStartInfo;
class				PluginLoader;
class				Plugin;
class				Channel;
class				ChannelGui;
class				Rack;

class				RackTrack
{
 public:
  RackTrack(Rack *parent, int index);
  ~RackTrack();

  Plugin			*AddRack(PlugStartInfo &startinfo, PluginLoader *p, Plugin *connect_to = 0x0);
  void				DeleteRack(Plugin *plug);
  int				GetYPos();
  void				RemoveRack();
  void				RemoveChannel();
  void				DeleteAllRacks();

  int				Units;
  Rack				*Parent;
  int				Index;
  list<Plugin *>		Racks;
  Channel			*Output;
  ChannelGui*			ChanGui;
  float				**CurrentBuffer;
  
};

class				Rack: public wxScrolledWindow
{
 public:
  Rack(wxWindow* parent, wxWindowID id = -1, 
       const wxPoint& pos = wxDefaultPosition, 
       const wxSize& size = wxDefaultSize);
  ~Rack();
  
  void				AddTrack(PlugStartInfo &startinfo, PluginLoader *p);
  void				AddTrack(Plugin *p);  
  RackTrack			*AddTrack();
  void				AddToSelectedTrack(PlugStartInfo &startinfo, PluginLoader *p);
  void				DeleteRack(Plugin *plug);
  void				DeleteAllRacks();
  RackTrack			*GetRackTrack(Plugin *plug);
  int				GetXPos(int index);
  void				SetScrolling();
  void				ResizeTracks();
  void				SetSelected(Plugin *p);
  void				RemoveFromSelectedTrack();
  void				RemoveTrack();

  
  bool 				DndGetDest(list<RackTrack *>::iterator &k,  list<Plugin *>::iterator &l, int &new_x, int &new_y , Plugin *plug);
  void				DndInsert(list<RackTrack *>::iterator &k,  list<Plugin *>::iterator &l, Plugin *plug);

  //  bool ProcessEvent(wxEvent& event);  

  void				HandleMouseEvent(Plugin *plug, wxMouseEvent *event);
  void				HandleKeyEvent(Plugin *plug, wxKeyEvent *event);
  void				HandlePaintEvent(Plugin *plug, wxPaintEvent *event);
  
  list<RackTrack *>		RackTracks;

  RackTrack			*selectedTrack;
  Plugin			*selectedPlugin;

 protected:  
  DECLARE_EVENT_TABLE()
 
  int				OldX;
  int				OldY;
  wxMenu			*menu;
  bool				WasDragging;
  virtual void			OnPaint(wxPaintEvent &event);
  void				OnHelp(wxMouseEvent &event);
  void				OnClick(wxMouseEvent &event);
};

extern Rack			*RackPanel;
extern int			RackCount;


#endif

