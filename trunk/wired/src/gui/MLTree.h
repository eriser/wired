// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#ifndef __MLTREE_H__
#define __MLTREE_H__

#include <vector>
#include <map>
#include <wx/treectrl.h>
#include <WiredDocument.h>
#include "../xml/WiredSessionXml.h"

using std::string;
using std::vector;
using std::map;

#define EXT_FILE		wxT("wired_exts.conf")
#define SAVE_TREE_FILE		wxT("MediaLibrary/MLTree");

/**
 * The size of ML icon (square)
 */

#define ICON_SIZE		(16)

/**
 * The s_nodeInfo struct contains various informations about files
*/
struct				s_nodeInfo
{
  wxString			label;
  wxString			extention;
  wxString			length;
};

/**
 * The MLTree class handle the tree of the MediaLibrary. It is derived from wxTreeCtrl
 */
class				MLTree : public wxTreeCtrl, public WiredDocument
{
 public:
/**
 * Main constructor for class MLTree
*/
  MLTree(wxWindow *dad, wxPoint p, wxSize s, long style);
/**
 * Main destructor for class MLTree
*/
  ~MLTree();

  void			LoadPatch(wxString filename);
  
  void			OnSave(wxString filename);

 protected:
  friend class			MediaLibrary;
  // friend class			WiredSessionXml;
  friend class			MLTreeInfos;

/**
 * wxTreeCtrl to display the tree and the nodes
*/
  wxTreeCtrl			*Tree;
/**
 * Boolean used to know if the media library nodes are collapsed or not
*/
  bool				collapsed;
/**
 * wxTreeItemId root is the first node of the Tree. It contains the project
 * name.
*/
  wxTreeItemId			root;
/**
 * a wxString used by filters
*/
  wxString			filters;
/**
 * A vector of wxStrings containing all the known sounds extentions
*/
  vector<wxString>		Exts;
/**
 * A Map of wxTreeItemId and their corresponding struct which contains 
 * various informations related to the item.
*/
  map<wxTreeItemId, s_nodeInfo>	nodes;
/**
 * A wxPoint which contains the position of the mouse when using right click
*/
  wxPoint			mouse_pos;
/**
 * A wxString that holds the name of the selected node
*/
  wxString			selected;
/**
 * A wxTreeItemId used for the drag and drop feature
*/
  wxTreeItemId		        item_begin;
/**
 * A wxTreeItemId used for the drag and drop feature
*/
  wxTreeItemId		        item_to_drag;
/**
 * A wxArrayTreeItemsIds which contains multiple selection items ids
*/
  wxArrayTreeItemIds		selection;
/**
 * An int holding the lenght number of the current selection
*/
  int				selection_length;
/**
 * Parse the tree and write xml file
 * \param XmlSession a WiredSessionXml*
 * \param parent a wxTreeItemId
 * \return void
*/
  void				SaveTree(WiredSessionXml *XmlSession, wxTreeItemId parent);
  void				LoadTree(WiredSessionXml *XmlSession);
/**
 * The method used to serialize the MediaLibrary Content (calls SaveTree)
 * \return void
*/
  void				SaveML();

  void				LoadML();
  void				OnSuppr(wxKeyEvent &event);

  /**
   * Wired Document implementation
  */
  void			Save();

  void			SaveTreeSC(wxTreeItemId parent, SaveElement *parentData);

  void			Load(SaveElementArray data);
  
  void			LoadItem(wxTreeItemId parent, SaveElement *parentData);

/**
 * The function IsTreeCollapsed is used to know if the nodes are expanded or
 * not.
 * \return a boolean set to true if the Tree is collapsed, else return false.
*/
  bool				IsTreeCollapsed();

/**
 * The SetTreeCollapsed function is a seter used when clicking the expand or
 * collapse button. It sets the collapsed variable to true.
 * \return void
*/
  void				SetTreeCollapsed();

/**
 * The SetTreeExpanded function is a seter used when clicking the expand or
 * collapse button. It sets the collapsed variable to false.
 * \return void
*/
  void				SetTreeExpanded();

/**
 * The GetTreeItemIdFromLabel function is used to get the id of a node using 
 * it's label.
 * \param label a wxString containing the name of a node
 * \return a wxTreeItemId corresponding to the selectioned node
*/
  wxTreeItemId			GetTreeItemIdFromLabel(wxString label);

/**
 * The GetTreeItemStructFromId function is used to get the struct s_node_info
 * associated to a node item using it's Id.
 * \param ItemToFind a wxTreeItemId corresponding to the desired node
 * \return a s_node_info struct correponding to the desired node
*/
  s_nodeInfo			GetTreeItemStructFromId(wxTreeItemId ItemToFind);

/**
 * The getSelection return the label of the selected node
 * \param flag, an int set to 1 or 0 to determine if the label is taken from
 * the struct or from the tree
 * \return a wxString containing the label of the selected node
*/
  wxString			getSelection(int flag);

/**
 * The AddFile function adds a new node 
 * \param ParentNode a wxTreeItemId corresponding to the parent node of the
 * new node.
 * \param wxString FileToAdd the full path to the file to add
 * \param infosx a s_nodeInfo struct used to hold informations about the file
 * \param expand, a boolean used to expand or not the new node
 * \return the added node.
*/
  wxTreeItemId			AddFile(wxTreeItemId ParentNode, wxString FileToAdd, s_nodeInfo infos, bool expand);

/**
 * The ExpandAll function sets all the existing nodes to the state expanded 
 * \param Tree, a wxTreeCtrl which is the tree to expand
 * \param id, a wxTreeItemId which is used for recursivity
 * \param shouldExpand, a boolean used to expand or not the current node
 * \param toLevel, an int which represent the level of expansion
 * \return void
*/
  void				ExpandAll(wxTreeCtrl *Tree, const wxTreeItemId& id, bool shouldExpand, int toLevel);

/**
 * The OnCollapse function is called when launching the Expand or Collapse 
 * action.
 * \return void
*/
  void				OnCollapse();

/**
 * The LoadKnownExtentions function load the known extentions file types.
 * \return void
*/
  bool				LoadKnownExtentions();

/**
 * The AddIcon function add a visual icon to a node.
 * \param images a wxImageList holding the available images
 * \param icon a wxIcon
 * \return void
*/
  void				AddIcon(wxImageList *images, wxIcon icon);

/**
 * The OnAdd function is called by launching the Add File action, using the
 * dedicated button. The file is added to the selected node or automaticaly 
 * sorted if no node is selected
 * \param FileToAdd a wxString which is the full path to the file to add
 * \return void
*/
  void				OnAdd(wxString FileToAdd);

/**
 * The OnRemove function remove the selected nodes
 * \return void
*/
  void				OnRemove();

/**
 * The OnCreateDir function is called when creating a new directory. It 
 * create a new directory node.
 * \return void
*/
  void				OnCreateDir();

/**
 * The OnInsert function is called when inserting the selected item into a
 * track
 * \return void
*/
  void				OnInsert();

/**
 * The OnEdit function is called when launching the Edit function. It loads
 * the file into the wired wav editor
 * \return void
*/
  void				OnEdit();

/**
 * The OnPreview function is called when lauching the preview action. It 
 * starts playing the selected file
 * \return void
*/
  void				OnPreview();

/**
 * The DisplayInfos function is called when lauching the File Infos action.
 * It open a tool tip containing various informations about the selected node
 * \return void
*/
  void				DisplayInfos();

/**
 * The OnRightClick function is called when right clicking a node.
 * \param event a wxMouseEvent& because this is the normal wx event handling
 * \return void
*/
  void				OnRightClick(wxMouseEvent &event);

/**
 * The OnContextMenu is not documented yet.
 * \param event a wxMouseEvent& because this is the normal wx event handling
 * \return void
*/
  void				OnContextMenu(wxMouseEvent &WXUNUSED(event));

/**
 * The OnSelChange is called every time the user select a node from the tree
 * \param event a wxTreeEvent& because this is the normal wx event handling
 * \return void
*/
  void				OnSelChange(wxTreeEvent &event);

/**
 * The BeginDrag function is called when the user starts to drag and drop a
 * node containing a file
 * \param event a wxTreeEvent& because this is the normal wx event handling
 * \return void
*/
  void				BeginDrag(wxTreeEvent &event);

/**
 * The EndDrag function is called when the user finishes to drag and drop a
 * node containing a file
 * \param event a wxTreeEvent& because this is the normal wx event handling
 * \return void
*/
  void				EndDrag(wxTreeEvent &event);

/**
 * The DisplayNodes is here for debug only
 * \return void
*/
  void				DisplayNodes();

/**
 * The SortNodes is used to sort the node depending on the user's choice
 * \param selected, a wxString containing the name of the selected node
 * \return void
*/
  void				SortNodes(wxString selected);

/**
 * The OnCompareItems is used to compare two items
 * \param item1, the first wxTreeItemId&
 * \param item2, the second wxTreeItemId&
 * \return an int set to 0 if true
*/
  int				OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);

/**
 * The Copy function returns a copy of a node
 * \param item, the wxTreeItemId to copy
 * \return a wxTreeItemId
*/
  wxTreeItemId                  Copy(wxTreeItemId item);

/**
 * The DragAndDrop function is here for the drag and drop feature
 * \param item, the wxTreeItemId representing the node to drag
 * \return void
*/
  void                          DragAndDrop(wxTreeItemId item);

/**
 * The OnLeftClick is called when left clicking a node
 * \param event, a wxMouseEvent& because this is the normal event handling
 * \return void
*/
  void				OnLeftClick(wxMouseEvent &event);

/**
 * The declaration of the event table
*/
  DECLARE_EVENT_TABLE()
};

/**
 * enums for event handling
*/
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
   MLTree_RightClick = ML_ID_EFFECTS_MENU + 1,
   MLTree_Menu,
   MLTree_Selected
};

#endif
