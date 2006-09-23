// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <wx/filename.h>
#include <wx/treectrl.h>
#include "MediaLibrary.h"
#include "MLTree.h"
#include "MLTreeInfos.h"
#include "MainWindow.h"
#include "Sequencer.h"
#include "SequencerGui.h"
#include "Colour.h"
#include "WiredSession.h"
#include "HelpPanel.h"
#include "DownButton.h"
#include "HoldButton.h"
#include "StaticLabel.h"
#include "VUMCtrl.h"
#include "../engine/Settings.h"
#include "../engine/AudioEngine.h"
#include "file.xpm"
#include "audio.xpm"
#include "icon5.xpm"
#include "icon3.xpm"
#include "delete.xpm"


extern WiredSession				*CurrentSession;
extern MediaLibrary				*MediaLibraryPanel;

s_nodeInfo					SetStructInfos(s_nodeInfo infos, wxString label, wxString extention, wxString length)
{
  infos.label = label;
  infos.extention = extention;
  infos.length = length;
  /* add more... */
  return(infos);
}

MLTree::MLTree(wxWindow *MediaLibraryPanel, wxPoint p, wxSize s, long style)
  : wxTreeCtrl(MediaLibraryPanel, MLTree_Selected, p, s, style)
{

  SetIndent(10);
  /* Set the Root node with the project's name in label */
  root = AddRoot(_("Project's name"));
  SetItemBold(root);

  m_reverseSort = 1;

  /* Create Image List */
  wxImageList *images = new wxImageList(16, 16, TRUE);
  AddIcon(images, wxIcon(icon3_xpm));
  AddIcon(images, wxIcon(icon5_xpm));
  AddIcon(images, wxIcon(audio_xpm));
  AddIcon(images, wxIcon(file_xpm));
  AssignImageList(images);
  /* Create basic nodes */
  s_nodeInfo		infos;
  wxTreeItemId		itemTemp;

  infos = SetStructInfos(infos, _("Sounds"), wxT(""), wxT(""));
  itemTemp = AppendItem(root, _("Sound Files"));
  SetItemImage(itemTemp, 0);
  nodes[itemTemp] = infos;

  s_nodeInfo	infos1;
  infos1 = SetStructInfos(infos1, _("MIDI"), wxT(""), wxT(""));
  itemTemp = AppendItem(root, _("MIDI Files"));
  SetItemImage(itemTemp, 0);
  nodes[itemTemp] = infos1;

  s_nodeInfo	infos2;
  infos2 = SetStructInfos(infos2, _("Videos"), wxT(""), wxT(""));
  itemTemp = AppendItem(root, _("Videos Files"));
  SetItemImage(itemTemp, 0);
  nodes[itemTemp] = infos2;

  s_nodeInfo	infos3;
  infos3 = SetStructInfos(infos3, _("Effects"), wxT(""), wxT(""));
  itemTemp = AppendItem(root, _("Effects Files"));
  SetItemImage(itemTemp, 0);
  nodes[itemTemp] = infos3;

  Expand(root);
  LoadKnownExtentions();
  //temp
  // DisplayNodes();

  Connect(ML_ID_MENU_DELETE, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MLTree::OnRemove);
  Connect(ML_ID_MENU_CREATEDIR, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MLTree::OnCreateDir);
  Connect(ML_ID_MENU_INSERT, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MLTree::OnInsert);
  Connect(ML_ID_MENU_EDIT, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MLTree::OnEdit);
  Connect(ML_ID_MENU_PREVIEW, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MLTree::OnPreview);
  Connect(ML_ID_MENU_INFOS, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)&MLTree::DisplayInfos);
}

MLTree::~MLTree()
{
 
}

void				MLTree::AddIcon(wxImageList *images, wxIcon icon)
{
  int sizeInit = icon.GetWidth();

  if (16 == sizeInit)
    images->Add(icon);
  else
    images->Add(wxBitmap(wxBitmap(icon).ConvertToImage().Rescale(16, 16)));
}

// Load extentions known by wired
bool				MLTree::LoadKnownExtentions()
{
  wxTextFile			file(WiredSettings->ConfDir + EXT_FILE);
  wxString			l;
  wxString			*itemdata = NULL;

  filters = wxT("");
  if (file.Open())
    {
      for (l = file.GetFirstLine(); ; l = file.GetNextLine())
	{
	  l.Trim(false);
	  l = l.BeforeFirst('#');
	  if (!l.IsEmpty())
	    {
	      itemdata = new wxString(l.BeforeFirst('\t'));
	      Exts.push_back(*itemdata);
	      filters += *itemdata + wxT(";");
	    }
 	  if (file.Eof())
 	    break;
	}
      file.Close();
    }
  else
    {
      cout << "[MEDIALIBRARY] Could not open ext file" << endl;
    }
  
  //   Display known extentions
  //   for (vector<wxString>::iterator iter = Exts.begin(); iter != Exts.end(); iter++)
  //     {
  
  //       cout << "[MEDIALIBRARY] : " << *iter << endl;
  //     }
  
  return (true);
}

void				MLTree::SetTreeExpanded()
{
  collapsed = false;
}

void				MLTree::SetTreeCollapsed()
{
  collapsed = true;
}

bool				MLTree::IsTreeCollapsed()
{
  return (collapsed);
}

// When creating a directory
void				MLTree::OnCreateDir()
{
  //  cout << "[MEDIALIBRARY] OmCreateDir" << endl;

  s_nodeInfo		infos;
  wxTreeItemId		itemParent;
  wxTreeItemId		itemAdded;


  itemParent = GetSelection();

  infos = SetStructInfos(infos, _("New Directory"), wxT(""), wxT(""));
  itemAdded = AppendItem(itemParent, _("New Directory"));
  SetItemImage(itemAdded, 0);
  nodes[itemAdded] = infos;
  Expand(itemParent);
  EditLabel(itemAdded);
}

// When inserting a file
void				MLTree::OnInsert()
{
  wxCommandEvent		event;

  MediaLibraryPanel->OnInsert(event);
}

// When editing a file
void				MLTree::OnEdit()
{
  wxCommandEvent		event;

  MediaLibraryPanel->OnEdit(event);
}

// When previewing a file
void				MLTree::OnPreview()
{
  wxCommandEvent		event;

  MediaLibraryPanel->OnPreview(event);
}

// Display infos corresponding to a node
void				MLTree::DisplayInfos()
{
  //  cout << "[MEDIALIBRARY] DISPLAYINFOS" << endl;
  cout << "[MEDIALIBRARY] DISPLAYINFOS" << endl;
  
  mouse_pos.y += 50;
  mouse_pos.x += 30;
  MLTreeInfos	*infos = new MLTreeInfos(MediaLibraryPanel, mouse_pos, wxSize(300,150), wxCAPTION | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxTHICK_FRAME);
}

// When adding a file
void				MLTree::AddFile(wxTreeItemId ParentNode, wxString FileToAdd, s_nodeInfo infos)
{

  wxTreeItemId			itemToAdd;
  itemToAdd = AppendItem(ParentNode, FileToAdd);
  Expand(ParentNode);
  SetItemImage(itemToAdd, 3);
  nodes[itemToAdd] = infos;
}

// Return an item Id from a label node
wxTreeItemId			MLTree::GetTreeItemIdFromLabel(wxString label)
{
  wxTreeItemId			ItemToReturn;
  map<wxTreeItemId, s_nodeInfo>::iterator theIterator;

  for (theIterator = nodes.begin(); theIterator != nodes.end(); theIterator++ )
    {
      s_nodeInfo temp;
      temp = (*theIterator).second;
      if (temp.label.Cmp(label) == 0)
	{
	  ItemToReturn = (*theIterator).first;
	}
    }
  return (ItemToReturn);
}

// DEBUG display nodes names
void				MLTree::DisplayNodes()
{
  map<wxTreeItemId, s_nodeInfo>::iterator it;

  int cnt = 0;
  for (it = nodes.begin(); it != nodes.end(); it++)
    {
      s_nodeInfo temp;
      temp = (*it).second;
      //      cout << "value : [" << temp.label << "]" << endl;
      cnt++;
    }
  //  cout << "  total : " << cnt << endl;
}

// Compare two existing items
int				MLTree::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
  map<wxTreeItemId, s_nodeInfo>::iterator it;
  wxString	text1;
  wxString	text2;
  int		slashPos;

  //  cout << "selected : " << selected.mb_str() << endl;
  for (it = nodes.begin(); it != nodes.end(); it++)
    {
      if ((*it).first == item1)
	{
	  s_nodeInfo temp;
	  temp = (*it).second;
	  if (!selected.Cmp(wxT("filesize")))
	    text1 = temp.length;
	  else if (!selected.Cmp(wxT("filename")))
	    {
	      text1 = temp.label;
	      slashPos = text1.Find('/', true);
	      text1 = text1.Mid(slashPos + 1);
	    }
	  else if (!selected.Cmp(wxT("filetype")))
	    text1 = temp.extention;
	  else
	    text1 = temp.label;
	}
    }
  for (it = nodes.begin(); it != nodes.end(); it++)
    {
      if ((*it).first == item2)
	{
	  s_nodeInfo temp;
	  temp = (*it).second;
	  if (!selected.Cmp(wxT("filesize")))
	    text2 = temp.length;
	  else if (!selected.Cmp(wxT("filename")))
	    {
	      text2 = temp.label;
	      slashPos = text2.Find('/', true);
	      text2 = text2.Mid(slashPos + 1);
	    }
	  else if (!selected.Cmp(wxT("filetype")))
	    text2 = temp.extention;
	  else
	    text2 = temp.label;
	}
    }
  //  cout << "text1: " << text1.mb_str() << " | text2: " << text2.mb_str() << "comp: " << text1.Cmp(text2) << endl;
  //  cout << "sort:" << m_reverseSort << endl;

  return text1.Cmp(text2) * m_reverseSort;

}

// Sort existing nodes
void				MLTree::SortNodes(wxString MLselected)
{
  map<wxTreeItemId, s_nodeInfo>::iterator it;
  wxTreeItemId temp;

  selected = MLselected;
  if (m_reverseSort == 1)
    m_reverseSort = -1;
  else
    m_reverseSort = 1;
  for (it = nodes.begin(); it != nodes.end(); it++)
    {
      temp = (*it).first;
      SortChildren(temp);
    }
}

// Return the struct associated to a node, from an ID
s_nodeInfo			MLTree::GetTreeItemStructFromId(wxTreeItemId ItemToFind)
{
  s_nodeInfo			structToReturn;
  map<wxTreeItemId, s_nodeInfo>::iterator theIterator;

  for (theIterator = nodes.begin(); theIterator != nodes.end(); theIterator++)
    {
      if ((*theIterator).first == ItemToFind)
	{
	  s_nodeInfo		temp;

	  temp = (*theIterator).second;
	  return (temp);
	}
    }
  return (structToReturn);
}

// When adding a file/directory
void				MLTree::OnAdd(wxString FileToAdd)
{

  if (!FileToAdd.empty())
    {
      wxFileName	*File = new wxFileName(FileToAdd);

      if (File->FileExists() == true)
	{
	  //	  cout << "[MEDIALIBRARY] File added : " << FileToAdd <<  " Extention is : " << File->GetExt() << endl;
	  for (vector<wxString>::iterator iter = Exts.begin(); iter != Exts.end(); iter++)
	    {
	      if (iter->Contains(File->GetExt()) == true)
		{
		  s_nodeInfo		infos;
		  int			slashPos;
		  wxTreeItemId		selection;

		  infos = SetStructInfos(infos, FileToAdd, File->GetExt(), wxT(""));
		  slashPos = FileToAdd.Find('/', true);
		  selection = GetSelection();

		  if (selection.IsOk() == true && selection != GetRootItem())
		    {
		      this->AddFile(selection, FileToAdd.Mid(slashPos + 1), infos);
		    }
		  else
		    {
		      this->AddFile(GetTreeItemIdFromLabel(_("Sounds")), FileToAdd.Mid(slashPos + 1), infos);
		    }
		}
	    }
	  DisplayNodes();
	}
    }
}

// Return selection label
wxString			MLTree::getSelection(int flag)
{
  wxArrayTreeItemIds		selection;
  int				selection_length;
  int				i;
  wxTreeItemId			ItemToReturn;
  map<wxTreeItemId, s_nodeInfo>::iterator it;


  selection_length = GetSelections(selection);
  for (i = 0; i < selection_length; i++)
    if (GetItemParent(selection[i]) != GetRootItem() && selection[i] != GetRootItem())
      if (!flag)
	{
	  return (GetItemText(selection[i]));
	}
      else
	{
	  s_nodeInfo		temp;
 
	  temp = GetTreeItemStructFromId(selection[i]);
	  return (temp.label);
	}
  return (wxT(""));
}

// When removing an element
void				MLTree::OnRemove()
{
  wxArrayTreeItemIds		selection;
  int				selection_length;
  int				i;

  selection_length = GetSelections(selection);
  for (i = 0; i < selection_length; i++)
    {
      if (GetItemParent(selection[i]) != GetRootItem() && selection[i] != GetRootItem())
	{
	  // wxTreeItemId temp = GetItemText(selection[i]);
	  nodes.erase(selection[i]);
	  DeleteChildren(selection[i]);
	  Delete(selection[i]);
	}
    }
  DisplayNodes();
}

// Expand all existing nodes
void				MLTree::ExpandAll(wxTreeCtrl *Tree, const wxTreeItemId& id, bool shouldExpand, int toLevel)
{

  if (toLevel == 0 || !ItemHasChildren(id))
    return;
  bool isExpanded = IsExpanded(id);
  if (shouldExpand && !isExpanded)
    Expand(id);
  else if (!shouldExpand && isExpanded)
    Collapse(id);
  wxTreeItemIdValue cookie = &Tree;
  for (wxTreeItemId child = GetFirstChild(id, cookie); child.IsOk(); child = GetNextChild(id, cookie))
    ExpandAll(Tree, child, shouldExpand, toLevel - 1);
}

// Collapse all existing nodes
void				MLTree::OnCollapse()
{
  //  cout << "[MEDIALIBRARY] Expand/Collapse Tree (OnCollapse)" << endl;
  // TODO : Find a way to get the max depth of node childs. 
  // Currently using 100 as an arbitrary max depth
  if (IsTreeCollapsed() == true)
    {
      ExpandAll(Tree, GetRootItem(), true, 100);
      SetTreeExpanded();
    }
  else
    {
      wxTreeItemIdValue cookie = &Tree;
      ExpandAll(Tree, GetRootItem(), false, 100);
      EnsureVisible(GetFirstChild(GetRootItem(), cookie));
      SetTreeCollapsed();
    }
}

// Display menu on right click
void				MLTree::OnRightClick(wxMouseEvent& event)
{
  //  cout << "[MEDIALIBRARY] RightClick" << endl;
  wxTreeItemId		item;
  wxMenu* myMenu = new wxMenu();
  s_nodeInfo		infos;


  mouse_pos = event.GetPosition();
  item = GetSelection();
  if (item == GetRootItem())
    {
      return ;
    }
  infos = GetTreeItemStructFromId(item);
  if (infos.extention.Cmp(wxT("")))
    {
      myMenu->Append(ML_ID_MENU_INFOS, wxT("Infos"), wxT("Infos"));
      myMenu->Append(ML_ID_MENU_PREVIEW, wxT("Preview"), wxT("Preview"));
      myMenu->Append(ML_ID_MENU_INSERT, wxT("Insert"), wxT("Insert"));
      myMenu->Append(ML_ID_MENU_EDIT, wxT("Edit"), wxT("Edit"));
      myMenu->Append(ML_ID_MENU_DELETE, wxT("Delete"), wxT("Delete"));
      myMenu->AppendSeparator();
    }
  else
    {
      myMenu->Append(ML_ID_MENU_CREATEDIR, wxT("New Directory"), wxT("New Directory"));
    }
  myMenu->Append(ML_ID_MENU_DELETE, wxT("Delete"), wxT("Delete"));
  PopupMenu(myMenu);
  delete myMenu;
}


void				MLTree::OnSelChange(wxTreeEvent &event)
{
   wxTreeItemId		item;
   s_nodeInfo		infos;
  
   item = GetSelection();
  
   infos = GetTreeItemStructFromId(item);
   if (infos.extention.Cmp(wxT("")))
     {
       MediaLibraryPanel->TopToolbar->EnableTool(1, false);
       MediaLibraryPanel->TopToolbar->EnableTool(2, true);
       MediaLibraryPanel->TopToolbar->EnableTool(3, true);
       MediaLibraryPanel->TopToolbar->EnableTool(4, true);
       MediaLibraryPanel->BottomToolbar->EnableTool(5, true);
     }
   else if (GetItemParent(item) != GetRootItem() && item != GetRootItem())
     {
       MediaLibraryPanel->TopToolbar->EnableTool(1, true);
       MediaLibraryPanel->TopToolbar->EnableTool(2, true);
       MediaLibraryPanel->TopToolbar->EnableTool(3, false);
       MediaLibraryPanel->TopToolbar->EnableTool(4, false);
       MediaLibraryPanel->BottomToolbar->EnableTool(5, false);
     }
   else
     {
       MediaLibraryPanel->TopToolbar->EnableTool(1, true);
       MediaLibraryPanel->TopToolbar->EnableTool(2, false);
       MediaLibraryPanel->TopToolbar->EnableTool(3, false);
       MediaLibraryPanel->TopToolbar->EnableTool(4, false);
       MediaLibraryPanel->BottomToolbar->EnableTool(5, false);
     }
  
}
BEGIN_EVENT_TABLE(MLTree, wxTreeCtrl)
  EVT_RIGHT_UP(MLTree::OnRightClick)
  EVT_TREE_SEL_CHANGED(MLTree_Selected, MLTree::OnSelChange)
END_EVENT_TABLE()
