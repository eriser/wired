// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __MLTREE_H__
#define __MLTREE_H__

#include <wx/wx.h>
#include <vector>
#include <list>
#include <map>

using std::string;
using std::vector;
using std::map;

#define EXT_FILE		wxT("wired_exts.conf")

struct				s_nodeInfo
{
  wxString			label;
  wxString			extention;
  wxString			length;

};

class				MLTree : public wxTreeCtrl
{
 public:
  MLTree(wxWindow *dad, wxPoint p, wxSize s, long style);
  ~MLTree();

 protected:
  friend class			MediaLibrary;

  wxTreeCtrl			*Tree;
  bool				collapsed;
  wxTreeItemId			root;
  wxString			filters;
  vector<wxString>		Exts;
  map<wxTreeItemId, s_nodeInfo>	nodes;
  wxPoint			mouse_pos;
  int				m_reverseSort;
  wxString			selected;
  wxTreeItemId		        item_begin;
  wxTreeItemId		        item_to_drag;
  wxArrayTreeItemIds		selection;
  int				selection_length;
  bool				IsTreeCollapsed();
  void				SetTreeCollapsed();
  void				SetTreeExpanded();
  wxTreeItemId			GetTreeItemIdFromLabel(wxString label);
  s_nodeInfo			GetTreeItemStructFromId(wxTreeItemId ItemToFind);
  wxString			getSelection(int flag);
  void				AddFile(wxTreeItemId ParentNode, wxString FileToAdd, s_nodeInfo infos, bool expand);
  void				ExpandAll(wxTreeCtrl *Tree, const wxTreeItemId& id, bool shouldExpand, int toLevel);
  void				OnCollapse();
  bool				LoadKnownExtentions();
  void				AddIcon(wxImageList *images, wxIcon icon);
  void				OnAdd(wxString FileToAdd);
  void				OnRemove();
  void				OnCreateDir();
  /////
  void				OnInsert();
  void				OnEdit();
  void				OnPreview();
  void				DisplayInfos();
  /////
  void				OnRightClick(wxMouseEvent &event);
  void				OnContextMenu(wxMouseEvent &WXUNUSED(event));
  void				OnSelChange(wxTreeEvent &event);
  void				BeginDrag(wxTreeEvent &event);
  void				EndDrag(wxTreeEvent &event);
  void				DisplayNodes();
  void				SortNodes(wxString selected);
  int				OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
  wxTreeItemId                  Copy(wxTreeItemId item);
  void                          DragAndDrop(wxTreeItemId item);
  DECLARE_EVENT_TABLE()
};

enum
{
  ML_ID_MENU_ADD = 20600,
  //  ML_ID_MENU_CUT,
  //  ML_ID_MENU_COPY,
  //  ML_ID_MENU_PASTE,
  ML_ID_MENU_PREVIEW,
  ML_ID_MENU_INSERT,
  ML_ID_MENU_EDIT,
  ML_ID_MENU_DELETE,
  ML_ID_MENU_INFOS,
  ML_ID_MENU_CREATEDIR,
  ML_ID_INSTR_MENU,
  ML_ID_EFFECTS_MENU
};

enum
{
   MLTree_RightClick = 165238,
   MLTree_Menu,
   MLTree_Selected
};

#endif
