// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#ifndef __RACK_H__
#define __RACK_H__

#include <wx/wx.h>
#include <wx/file.h>
#include <list>

using namespace									std;

#define 	UNIT_W								(200)
#define 	UNIT_H								(100)
#define 	UNIT_S								(2)
#define 	SCROLL_PIX							(50)

class											PluginLoader;
class											Plugin;
class											Rack;
class											RackTrack;
class											ChannelGui;
#ifndef __TRACK_H__
	class											Channel;
#endif


typedef 	struct s_PlugStartInfo				PlugStartInfo;
typedef		list<RackTrack *>					t_ListRackTrack;
typedef		list<Plugin *>::const_iterator		t_ListPluginIterator;

typedef struct	s_RackTrackPlugin
{
	RackTrack*	rackTrack;
	Plugin*		plugin;
}				t_RackTrackPlugin;

class					RackTrack
{
 public:
	RackTrack(Rack *parent, int index);
	RackTrack(const RackTrack& copy){*this = copy;};
	~RackTrack();

	Plugin*				AddRack(PlugStartInfo &startinfo, PluginLoader *p, 
								Plugin *connect_to = 0x0);
	void				DeleteRack(Plugin *plug);
	int					GetYPos();
	void				RemoveRack();
	void				RemoveChannel();
	void				DeleteAllRacks();
	void				SetSelected(Plugin *plugin);
	int				NbRacks(){return Racks.size();};
	RackTrack			operator=(const RackTrack& right);

	void				Dump();													// Debug - Shows member variables
	void				DumpPlugins();								// Pas du tout objet, mais plus simple pour l'instant (ca permet de ne pas changer 
																				// l'API du plugin qui va de tte maniere l'etre pour les DSSI)
	Plugin*				SelectedPlugin;
	int					Units;
	Rack*				Parent;
	int					Index;
	list<Plugin *>		Racks;
	Channel*			Output;
	ChannelGui*			ChanGui;
	float**				CurrentBuffer;
};

class				Rack: public wxScrolledWindow
{
 public:
	Rack(wxWindow* parent, wxWindowID id = -1, 
    	 const wxPoint& pos = wxDefaultPosition, 
	     const wxSize& size = wxDefaultSize);
	Rack(const Rack& copy){*this = copy;};
	~Rack();

	Plugin*				AddTrack(PlugStartInfo &startinfo, PluginLoader *p);	// Kept for compatibility but shouldn't be used anymore
	void				AddTrack(Plugin *p);
	t_RackTrackPlugin*	AddRackAndChannel(PlugStartInfo &startinfo, 			// Should be used instead of AddTrack
											PluginLoader *p);					// Adds a rack and a channel (channel == Mixer object in Mixer window)
	RackTrack*			AddTrack();
	Plugin*				AddToSelectedTrack(PlugStartInfo &startinfo, 			// Kept for compatibility but shouldn't be used anymore
											PluginLoader *p);
	Plugin*				AddSelectedRackAndChannel(PlugStartInfo &startinfo, 	// Should be used instead of AddToSelectedTrack
													PluginLoader *p);
																				// Adds a rack and a channel (== Mixer object in Mixer window) 
																				// and selects the rack
	void				DeleteRack(Plugin *plug);
	void				DeleteAllRacks();
	RackTrack*			GetRackTrack(Plugin *plug);
	int					GetXPos(int index);
	void				SetScrolling();											// Applies scrolling on all racks
	void				ResizeTracks();											// Resizes racks positions from index with scrolling
	void				SetSelected(Plugin *p);									// Seems to select a rack
	void				RemoveFromSelectedTrack();								// Kept for compatibility but shouldn't be used anymore
	bool				RemoveSelectedRackAndChannel();							// Should be used instead of RemoveFromSelectedTrack
																				// Removes the selected rack and the attached channel
	void				RemoveTrack();											// Kept for compatibility but shouldn't be used anymore
	bool				RemoveTrack(int index);									// Should be used instead of RemoveTrack
																				// Removes a rack and a channel from index
																				// RemoveRack() ? Not RemoveAllRackTracks() ?
	bool				RemoveTrack(const RackTrack* rackTrack);				// Should be used instead of RemoveTrack

	void				Dump();													// Debug - Shows member variables

	void				AddPlugToMenu();										// Adds subMenuItems (Instruments or effects in Add menuItem)
	//  bool ProcessEvent(wxEvent& event);  

	void				HandleMouseEvent(Plugin *plug, wxMouseEvent *event);	// Handles all mouse events
	void				HandleKeyEvent(Plugin *plug, wxKeyEvent *event);		// Handles key events
	void				HandlePaintEvent(Plugin *plug, wxPaintEvent *event);	// Handles all paint events like onPaint ???
																				// Draws the selection rectangle

	Rack				operator=(const Rack& right);
	  
	t_ListRackTrack		RackTracks;

	RackTrack*			selectedTrack;
	Plugin*				selectedPlugin;

 protected:  
 
	int					OldX;
	int					OldY;
	int					new_x;
	int					new_y;		
	int					fd_size;

	bool				is_cut;
	Plugin*				copy_plug;
	wxFile				tmpFile;
	wxMenu*				menu;
	wxMenu*				submenu;
	wxMenu*				instr_menu;
	wxMenu*				effects_menu;
	wxString			filePath;
	bool				WasDragging;
	
	virtual void		OnPaint(wxPaintEvent &event);							// Not used ...
	void				OnHelp(wxMouseEvent &event);							// Event : Help handling
	void				OnClick(wxMouseEvent &event);							// Event : Click on rack : Sets unselect rack and plugin
	void				OnDeleteClick();										// Event : onContextMenuClick("Delete") ; new methode
	void				OnCutClick();											// Event : From contextMenu, Cuts a rack
	void				OnCopyClick();											// Event : From contextMenu, Copy a rack
	void				OnPasteClick();											// Event : From contextMenu, Pastes a rack
	void				OnPluginParamChange(wxMouseEvent &event);				// Event : Calls AddChangeParamsEffectAction while a plugin's param is changed
	bool 				DndGetDest(t_ListRackTrack::iterator &k, 
									list<Plugin *>::iterator &l, int &new_x, 
									int &new_y , Plugin *plug);
	void				DndInsert(t_ListRackTrack::iterator &k, 
									list<Plugin *>::iterator &l, Plugin *plug);
	void				UpdateUnitXSize();

private:
	
	void				RemoveRackAndChannel(t_ListRackTrack::const_iterator	// Removes a rack and a channel
												iter);
																				// Be carefull : Freezes if delete rack from contextMenu
	void				InitContextMenu();										// Initializes contextMenu
	void				ConnectPluginChangeParamEventHandler(RackTrack *rackTrack);
	DECLARE_EVENT_TABLE()
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

extern Rack			*RackPanel;
extern int			RackCount;


#endif

