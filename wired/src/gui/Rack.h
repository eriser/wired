// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __RACK_H__
#define __RACK_H__

#include <wx/wx.h>
#include <wx/file.h>
#include <wx/cmdproc.h>
#include <list>
#include "WiredDocument.h"
#include "RackTrack.h"

#define		UNIT_W		(200)
#define 	UNIT_H		(100)
#define 	UNIT_S		(2)
#define 	SCROLL_PIX	(50)

class		PluginLoader;
class		Plugin;
class		Rack;
class		RackTrack;
class		ChannelGui;
#ifndef __TRACK_H__
class		Channel;
#endif

typedef struct s_PlugStartInfo			PlugStartInfo;
typedef	std::list<RackTrack *>			t_ListRackTrack;
typedef	std::list<Plugin *>::const_iterator	t_ListPluginIterator;

/********************   class CreateRackAction   ********************/
class				CreateRackAction : public wxCommand
{
private:
  PluginLoader			*mPluginLoader;
  PlugStartInfo			*mStartInfo;
  Plugin*			_created;

public:
  CreateRackAction(wxString&, PlugStartInfo*, PluginLoader*);
  ~CreateRackAction() {};
  bool				Do();
  bool				Undo();
};

/********************   class DeleteRackAction   ********************/
class				DeleteRackAction : public wxCommand
{
private:
  Plugin*			_deleted;

public:
  DeleteRackAction(wxString, Plugin*);
  ~DeleteRackAction();
  bool				Do();
  bool				Undo();
};


class		Rack: public wxScrolledWindow, WiredDocument
{
 public:
  Rack(wxWindow* parent, wxWindowID id = -1,
       const wxPoint& pos = wxDefaultPosition,
       const wxSize& size = wxDefaultSize);
  ~Rack();

  // WiredDocument implementation
  void			Save();
  void			Load(SaveElementArray data);
  void			CleanChildren();

  Plugin*		AddNewRack(PlugStartInfo &startinfo, PluginLoader *p);
  bool			AddLoadedRack(Plugin *p);
  Plugin*		AddToSelectedTrack(PlugStartInfo &startinfo,
					   PluginLoader *p);

  bool			DeleteRack(Plugin *plug, bool eraseit = true);
  void			DeleteSelectedRack();
  void			DeleteAllTracks();

  // Find a plugin from its name
  Plugin*		FindPlugin(wxString name);

  // Get RackTrack from a plugin
  RackTrack*		GetRackTrack(Plugin *plug);

  int			GetXPos(int index);

  // Applies scrolling on all racks
  void			SetScrolling();

  // Resizes racks positions from index with scrolling
  void			ResizeTracks();

  // Seems to select a rack
  void			SetSelected(Plugin *p);

  // Select a RackTrack from its number
  void			SelectTrackFromNumber(int no);

  // Should be used instead of RemoveFromSelectedTrack
  bool			RemoveSelectedRackTrack();

  // Removes the last rack track
  void			RemoveLastRackTrack();

  // Removes a rack from its index
  bool			RemoveRackTrack(unsigned int index);

  // Removes a rack from its ptr
  bool			RemoveRackTrack(const RackTrack* rackTrack);

  // Debug - Shows member variables
  void			Dump();

  // Adds subMenuItems (Instruments or effects in Add menuItem)
  void			AddPlugToMenu();

  //  bool ProcessEvent(wxEvent& event);
  void          OnMotion(wxMouseEvent &event);

  // Handles all mouse events from plugins
  void			HandleMotionEvent(Plugin *plug, wxMouseEvent& event);
  void			HandleWheelEvent(Plugin *plug, wxMouseEvent& event);
  void			HandleLeftUpEvent(Plugin *plug, wxMouseEvent& event);
  void			HandleLeftDownEvent(Plugin *plug, wxMouseEvent& event);
  void			HandleRightDownEvent(Plugin *plug, wxMouseEvent& event);

  // Handles key events
  void			HandleKeyEvent(Plugin *plug, wxKeyEvent& event);
  // Handles all paint events like onPaint ???
  void			HandlePaintEvent(Plugin *plug, wxPaintEvent& event);
  // Draws the selection rectangle

  void			SetAudioConfig(long bufferSize, double samplingRate);

  t_ListRackTrack	RackTracks;

  RackTrack*		selectedTrack;
  Plugin*		selectedPlugin;

 protected:

  int			OldX;
  int			OldY;
  int			fd_size;

  bool			is_cut;
  Plugin*		copy_plug;
  wxFile		tmpFile;
  wxMenu*		menu;
  wxMenu*		submenu;
  wxMenu*		instr_menu;
  wxMenu*		effects_menu;
  wxString		filePath;
  bool			WasDragging;

  // Event : Help handling
  void			OnHelp(wxMouseEvent &event);
  // Event : Click on rack : Sets unselect rack and plugin
  void			OnClick(wxMouseEvent &event);
  // Event : onContextMenuClick("Delete") ; new methode
  void			OnDeleteClick();
  // Event : From contextMenu, Cuts a rack
  void			OnCutClick();
  // Event : From contextMenu, Copy a rack
  void			OnCopyClick();
  // Event : From contextMenu, Pastes a rack
  void			OnPasteClick();
  // Event : Calls AddChangeParamsEffectAction while a plugin's param is changed
  void			OnPluginParamChange(wxMouseEvent &event);
  bool 			DndGetDest(t_ListRackTrack::iterator &k,
				   std::list<Plugin *>::iterator &l, int &new_x,
				   int &new_y , Plugin *plug);
  void			DndInsert(t_ListRackTrack::iterator &k,
				  std::list<Plugin *>::iterator &l, Plugin *plug);
  void			UpdateUnitXSize();

 private:

  // Create a empty track and a channel
  RackTrack*		CreateRackTrack();

  // Removes a rack and a channel
  void			RemoveRackAndChannel(t_ListRackTrack::const_iterator
						     iter);

  // Be careful : Freezes if delete rack from contextMenu
  // Initializes contextMenu
  void			InitContextMenu();
  DECLARE_EVENT_TABLE();
};

    // IDS
enum
  {
    ID_MENU_ADD = 20000,
    ID_MENU_CUT,
    ID_MENU_COPY,
    ID_MENU_PASTE,
    ID_MENU_DELETE,
    ID_INSTR_MENU,
    ID_EFFECTS_MENU
  };

extern Rack		*RackPanel;
extern int		RackCount;


#endif

