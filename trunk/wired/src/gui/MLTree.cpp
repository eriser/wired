// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <wx/filename.h>
#include <wx/treectrl.h>
#include "MediaLibrary.h"
#include "MLTree.h"
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
  : wxTreeCtrl(MediaLibraryPanel, -1, p, s, style)
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

  infos = SetStructInfos(infos, _("Sounds"), _(""), _(""));
  itemTemp = AppendItem(root, _("Sound Files"));
  SetItemImage(itemTemp, 0);
  nodes[itemTemp] = infos;

  s_nodeInfo	infos1;
  infos1 = SetStructInfos(infos1, _("MIDI"), _(""), _(""));
  itemTemp = AppendItem(root, _("MIDI Files"));
  SetItemImage(itemTemp, 0);
  nodes[itemTemp] = infos1;

  s_nodeInfo	infos2;
  infos2 = SetStructInfos(infos2, _("Videos"), _(""), _(""));
  itemTemp = AppendItem(root, _("Videos Files"));
  SetItemImage(itemTemp, 0);
  nodes[itemTemp] = infos2;

  s_nodeInfo	infos3;
  infos3 = SetStructInfos(infos3, _("Effects"), _(""), _(""));
  itemTemp = AppendItem(root, _("Effects Files"));
  SetItemImage(itemTemp, 0);
  nodes[itemTemp] = infos3;

  Expand(root);
  LoadKnownExtentions();

  //temp
  DisplayNodes();

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

void				MLTree::OnCreateDir()
{
  cout << "[MEDIALIBRARY] OmCreateDir" << endl;

  s_nodeInfo		infos;
  wxTreeItemId		itemParent;
  wxTreeItemId		itemAdded;


  itemParent = GetSelection();

  infos = SetStructInfos(infos, _("New Directory"), _(""), _(""));
  itemAdded = AppendItem(itemParent, _("New Directory"));
  SetItemImage(itemAdded, 0);
  nodes[itemAdded] = infos;
  Expand(itemParent);
  EditLabel(itemAdded);
}

void				MLTree::OnInsert()
{
  wxCommandEvent		event;

  MediaLibraryPanel->OnInsert(event);
}

void				MLTree::OnEdit()
{
  wxCommandEvent		event;

  MediaLibraryPanel->OnEdit(event);
}

void				MLTree::OnPreview()
{
  wxCommandEvent		event;

  MediaLibraryPanel->OnPreview(event);
}

void				MLTree::DisplayInfos()
{
  cout << "[MEDIALIBRARY] DISPLAYINFOS" << endl;
}

void				MLTree::AddFile(wxTreeItemId ParentNode, wxString FileToAdd, s_nodeInfo infos)
{

  wxTreeItemId			itemToAdd;
  itemToAdd = AppendItem(ParentNode, FileToAdd);
  SetItemImage(itemToAdd, 3);
  nodes[itemToAdd] = infos;
}

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

void				MLTree::DisplayNodes()
{
  map<wxTreeItemId, s_nodeInfo>::iterator it;

  int cnt = 0;
  for (it = nodes.begin(); it != nodes.end(); it++)
    {
      s_nodeInfo temp;
      temp = (*it).second;
      cout << "value : [" << temp.label << "]" << endl;
      cnt++;
    }
  cout << "  total : " << cnt << endl;
}

int				MLTree::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
  map<wxTreeItemId, s_nodeInfo>::iterator it;
  wxString	text1;
  wxString	text2;
  int		slashPos;

  cout << "selected : " << selected.mb_str() << endl;
  for (it = nodes.begin(); it != nodes.end(); it++)
    {
      if ((*it).first == item1)
	{
	  s_nodeInfo temp;
	  temp = (*it).second;
	  if (!selected.Cmp(_("filesize")))
	    text1 = temp.length;
	  else if (!selected.Cmp(_("filename")))
	    {
	      text1 = temp.label;
	      slashPos = text1.Find('/', true);
	      text1 = text1.Mid(slashPos + 1);
	    }
	  else if (!selected.Cmp(_("filetype")))
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
	  if (!selected.Cmp(_("filesize")))
	    text2 = temp.length;
	  else if (!selected.Cmp(_("filename")))
	    {
	      text2 = temp.label;
	      slashPos = text2.Find('/', true);
	      text2 = text2.Mid(slashPos + 1);
	    }
	  else if (!selected.Cmp(_("filetype")))
	    text2 = temp.extention;
	  else
	    text2 = temp.label;
	}
    }
  //  cout << "text1: " << text1.mb_str() << " | text2: " << text2.mb_str() << "comp: " << text1.Cmp(text2) << endl;
  cout << "sort:" << m_reverseSort << endl;

  return text1.Cmp(text2) * m_reverseSort;

}

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

void				MLTree::OnAdd(wxString FileToAdd)
{

  if (!FileToAdd.empty())
    {
      wxFileName	*File = new wxFileName(FileToAdd);

      if (File->FileExists() == true)
	{
	  cout << "[MEDIALIBRARY] File added : " << FileToAdd <<  " Extention is : " << File->GetExt() << endl;
	  for (vector<wxString>::iterator iter = Exts.begin(); iter != Exts.end(); iter++)
	    {
	      if (iter->Contains(File->GetExt()) == true)
		{
		  s_nodeInfo		infos;
		  int			slashPos;

		  infos = SetStructInfos(infos, FileToAdd, File->GetExt(), _(""));
		  slashPos = FileToAdd.Find('/', true);
		  this->AddFile(GetTreeItemIdFromLabel(_("Sounds")), FileToAdd.Mid(slashPos + 1), infos);
		}
	    }
	  DisplayNodes();
	}
    }
}


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
  return (_(""));
}

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

void				MLTree::OnCollapse()
{
  cout << "[MEDIALIBRARY] Expand/Collapse Tree (OnCollapse)" << endl;
  if (IsTreeCollapsed() == true)
    {
      ExpandAll(Tree, GetRootItem(), true, 2);
      SetTreeExpanded();
    }
  else
    {
      wxTreeItemIdValue cookie = &Tree;
      ExpandAll(Tree, GetRootItem(), false, 2);
      EnsureVisible(GetFirstChild(GetRootItem(), cookie));
      SetTreeCollapsed();
    }
}

void				MLTree::OnRightClick(wxMouseEvent& event)
{
  cout << "[MEDIALIBRARY] RightClick" << endl;

  wxMenu* myMenu = new wxMenu();
  myMenu->Append(ML_ID_MENU_INFOS, wxT("Infos"), wxT("Infos"));
  myMenu->Append(ML_ID_MENU_PREVIEW, wxT("Preview"), wxT("Preview"));
  myMenu->Append(ML_ID_MENU_INSERT, wxT("Insert"), wxT("Insert"));
  myMenu->Append(ML_ID_MENU_EDIT, wxT("Edit"), wxT("Edit"));
  myMenu->Append(ML_ID_MENU_DELETE, wxT("Delete"), wxT("Delete"));
  myMenu->AppendSeparator();
  myMenu->Append(ML_ID_MENU_CREATEDIR, wxT("New Directory"), wxT("New Directory"));
  //  myMenu->Append(5112, wxT("New Snippet"), wxT("New Snippet"));
  PopupMenu(myMenu);
  delete myMenu;
}

BEGIN_EVENT_TABLE(MLTree, wxTreeCtrl)
  EVT_RIGHT_UP(MLTree::OnRightClick) 
END_EVENT_TABLE()
