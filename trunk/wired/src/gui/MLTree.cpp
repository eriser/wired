// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <wx/filename.h>
#include <wx/wx.h>
#include <wx/file.h>
#include <wx/treectrl.h>
#include <wx/defs.h>
//#include "../xml/WiredSessionXml.h"
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
#include "folder.xpm"
#include "folder-open.xpm"
#include "delete.xpm"
#include <SaveCenter.h>

extern WiredSession	*CurrentSession;
extern MediaLibrary	*MediaLibraryPanel;
extern SaveCenter	*saveCenter;
WiredSessionXml		*CurrXmlSession = NULL;

s_nodeInfo		SetStructInfos(s_nodeInfo infos, wxString label, wxString extention, wxString length)
{
  infos.label = label;
  infos.extention = extention;
  infos.length = length;
  /* add more... */
  return(infos);
}

MLTree::MLTree(wxWindow *MediaLibraryPanel, wxPoint p, wxSize s, long style)
  : wxTreeCtrl(MediaLibraryPanel, MLTree_Selected, p, s, style),
    WiredDocument(wxT("MLTree"))
{
  SetIndent(10);
  /* Set the Root node with the project name in label */
  root = AddRoot(_("Project name"));
  SetItemBold(root);

  /* Create Image List */
  wxImageList *images = new wxImageList(ICON_SIZE, ICON_SIZE, TRUE);
  AddIcon(images, wxIcon(folder_xpm));
  AddIcon(images, wxIcon(folder_open_xpm));
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

void		MLTree::SaveTreeSC(wxTreeItemId parent, SaveElement *parentElem)
{
  wxTreeItemIdValue	cookie;
  s_nodeInfo		infos;
  wxTreeItemId		item = GetFirstChild(parent, cookie);
  wxTreeItemId		item_last = GetLastChild(parent);
  SaveElement		*currSaveElem = NULL;

  while (item.IsOk())
    {
      wxString text = GetItemText(item);
      infos = GetTreeItemStructFromId(item);

      currSaveElem = new SaveElement();
      parentElem->addChildren(currSaveElem);

      if (ItemHasChildren(item))
	{
	  currSaveElem->setKey(wxT("folder"));
	  currSaveElem->addAttribute(wxT("name"), text);
	  if(IsExpanded(item))
	      currSaveElem->addAttribute(wxT("is_expanded"), wxT("true"));
	  else
	    currSaveElem->addAttribute(wxT("is_expanded"), wxT("false"));

	  //call recursively on our children
	  SaveTreeSC(item, currSaveElem);
	}
      else // no children
	{
	  if (infos.extention.Cmp(wxT("")))
	    {
	      currSaveElem->setKey(wxT("file"));
	      currSaveElem->addAttribute(wxT("name"), text);
	      currSaveElem->addAttribute(wxT("infos_label"), infos.label);
	      currSaveElem->addAttribute(wxT("infos_length"), infos.length);
	      currSaveElem->addAttribute(wxT("infos_ext"), infos.extention);
	    }
	  else
	    {
	      currSaveElem->setKey(wxT("folder"));
	      currSaveElem->addAttribute(wxT("name"), text);
	      currSaveElem->addAttribute(wxT("is_expanded"), wxT("false"));
	    }
	}
      item = GetNextChild(parent, cookie);
    }
}

void				MLTree::OnSave(wxString filename)
{
  SavePatch(wxT("MediaLibrary/MLTree"), filename);
}

void				MLTree::Save()
{
  SaveElement	*rootTreeSaveElem = new SaveElement();

  rootTreeSaveElem->setKey(wxT("root"));

  SaveTreeSC(GetRootItem(), rootTreeSaveElem);

  saveDocData(rootTreeSaveElem, wxT("MediaLibrary/MLTree"));
}

void				MLTree::LoadPatch(wxString filename)
{
  //Adapted Copy Paste of Load, didn't want to bother trying to make things clean...
  std::cerr << "[MLTree] Load" << std::endl;

  wxString		patchName;
  SaveElementArray	treeData;
  SaveElement		*rootSaveElem;

  DeleteAllItems();

  patchName = filename.AfterLast('/');
  patchName = patchName.BeforeLast('.');

  root = AddRoot(patchName);
  SetItemBold(root);

  treeData = AskData(filename);
  if(treeData.GetCount() > 0)
    {
      rootSaveElem = treeData.Item(0);
      while(rootSaveElem->getKey() != wxT("root") && rootSaveElem->hasChildren())
	rootSaveElem = rootSaveElem->getChildren().Item(0);

      if(rootSaveElem->getKey() == wxT("root"))
	LoadItem(GetRootItem(), rootSaveElem);
    }
  Expand(root);

}

void				MLTree::Load(SaveElementArray data)
{
  //We assume that data contains only one SaveElement : The root node.
  //else, something has failed in the save or the file is corrupted...

  std::cerr << "[MLTree] Load" << std::endl;

  SaveElement		*ref;
  SaveElementArray	treeData;
  wxString		treeFile;
  SaveElement		*rootSaveElem;

  DeleteAllItems();

  root = AddRoot(saveCenter->getProjectName());
  SetItemBold(root);

  ref = data.Item(0);
  treeFile = ref->getValue();

  treeData = AskData(treeFile);
  if(treeData.GetCount() > 0)
    {
      rootSaveElem = treeData.Item(0);
      while(rootSaveElem->getKey() != wxT("root") && rootSaveElem->hasChildren())
	rootSaveElem = rootSaveElem->getChildren().Item(0);

      if(rootSaveElem->getKey() == wxT("root"))
	LoadItem(GetRootItem(), rootSaveElem);
    }
  Expand(root);
}

void				MLTree::LoadItem(wxTreeItemId parent,
						 SaveElement *parentData)
{
  SaveElementArray	saveElemChildren;
  int			i;
  SaveElement		*currSaveElem;
  s_nodeInfo		infos;
  bool			expand;
  wxTreeItemId		next;

  saveElemChildren = parentData->getChildren();

  for(i = 0; i < saveElemChildren.GetCount(); i++)
    {
      currSaveElem = saveElemChildren.Item(i);
      if(currSaveElem->getKey() == wxT("folder"))
	{
	  infos = SetStructInfos(infos,
				 currSaveElem->getAttribute(wxT("name")),
				 wxT(""), wxT(""));
	  if(currSaveElem->getAttribute(wxT("is_expanded")) == wxT("true"))
	    expand = true;
	  else
	    expand = false;

	  next = AddFile(parent, infos.label, infos, expand);
	  if(expand)
	    SetItemImage(next, 1);
	  else
	    SetItemImage(next, 0);

	  LoadItem(next, currSaveElem);
	}
      else if(currSaveElem->getKey() == wxT("file"))
	{
	  infos = SetStructInfos(infos,
				 currSaveElem->getAttribute(wxT("infos_label")),
				 currSaveElem->getAttribute(wxT("infos_ext")),
				 currSaveElem->getAttribute(wxT("infos_length")));
	  AddFile(parent, currSaveElem->getAttribute(wxT("name")), infos, false);
	  SetItemImage(next, 3);
	}
      if(parentData->getAttribute(wxT("is_expanded")) == wxT("true"))
	 Expand(parent);
    }

}

void			MLTree::SaveTree(WiredSessionXml *XmlSession, wxTreeItemId parent)
{
  wxTreeItemIdValue	cookie;
  s_nodeInfo		infos;
  wxTreeItemId		item = GetFirstChild(parent, cookie);

 //  if (parent == GetRootItem())
//     {
//       XmlSession->MyStartElement(XmlSession, _("root"));
//       XmlSession->MyWriteAttribute(XmlSession, _("name"), GetItemText(parent));
//       XmlSession->MyEndElement(XmlSession);
//     }
  while (item.IsOk())
    {
      wxString text = GetItemText(item);
      infos = GetTreeItemStructFromId(item);

      if (ItemHasChildren(item))
	{
	  XmlSession->MyStartElement(XmlSession, _("folder"));
	  XmlSession->MyWriteAttribute(XmlSession, _("name"), text);
	  SaveTree(XmlSession, item);
	  XmlSession->MyEndElement(XmlSession);
	}
      else // no children
	{
	  if (infos.extention.Cmp(wxT("")))
	    {
	      XmlSession->MyStartElement(XmlSession, _("file"));
	      XmlSession->MyWriteAttribute(XmlSession, _("name"), text);
	      XmlSession->MyWriteAttribute(XmlSession, _("ext"), infos.extention);
	      XmlSession->MyEndElement(XmlSession);
	    }
	  else
	    {
	      XmlSession->MyStartElement(XmlSession, _("folder"));
	      XmlSession->MyWriteAttribute(XmlSession, _("name"), text);
	      XmlSession->MyEndElement(XmlSession);
	    }
	}
      item = GetNextChild(parent, cookie);
    }
}

void				MLTree::LoadTree(WiredSessionXml *XmlSession)
{
  wxString	Buffer;

  cout << "LoadTree...." << endl;
  //  XmlSession->MyGet
  XmlSession->Dumpfile(STR_XMLFILE_ML);
//   while (XmlSession->MyRead(XmlSession) == true)
//     {
//       Buffer = XmlSession->MyGetNodeName(XmlSession);
//       cout << "NAME : " << Buffer.mb_str() << endl;
//     }
}

void				MLTree::SaveML()
{
  CurrXmlSession = new WiredSessionXml(wxString(wxT(""), *wxConvCurrent));
  CurrXmlSession->InitSaveML();
  SaveTree(CurrXmlSession, GetRootItem());
  CurrXmlSession->EndSaveML();
}

void				MLTree::LoadML()
{
  CurrXmlSession = new WiredSessionXml(wxString(wxT(""), *wxConvCurrent));
  //CurrXmlSession->InitLoadML(STR_XMLFILE_ML);
  LoadTree(CurrXmlSession);
  //  CurrXmlSession->EndSaveML();
}

void				MLTree::AddIcon(wxImageList *images, wxIcon icon)
{
  int wInit = icon.GetWidth();
  int hInit = icon.GetWidth();

  if (wInit == ICON_SIZE && hInit == ICON_SIZE)
    images->Add(icon);
  else
    images->Add(wxBitmap(wxBitmap(icon).ConvertToImage().Rescale(ICON_SIZE, ICON_SIZE)));
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
  // cout << "[MEDIALIBRARY] DISPLAYINFOS" << endl;
  s_nodeInfo			info;

  info = GetTreeItemStructFromId(GetSelection());
  mouse_pos.y += 50;
  mouse_pos.x += 30;
  MLTreeInfos	*infos = new MLTreeInfos(MediaLibraryPanel, wxPoint(2, mouse_pos.y), wxSize(197, 100), wxCAPTION | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxTHICK_FRAME, info);
}

// When adding a file
wxTreeItemId			MLTree::AddFile(wxTreeItemId ParentNode, wxString FileToAdd, s_nodeInfo infos, bool expand)
{

  wxTreeItemId			itemToAdd;
  itemToAdd = AppendItem(ParentNode, FileToAdd);
  if (expand == true)
    Expand(ParentNode);
  SetItemImage(itemToAdd, 3);
  nodes[itemToAdd] = infos;

  return itemToAdd;
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
  // cout << "text1: " << text1.mb_str() << " | text2: " << text2.mb_str() << "comp: " << text1.Cmp(text2) << endl;

  return text1.Cmp(text2);

}

// Sort existing nodes
void				MLTree::SortNodes(wxString MLselected)
{
  map<wxTreeItemId, s_nodeInfo>::iterator it;
  wxTreeItemId temp;

  selected = MLselected;
  /*if (m_reverseSort == 1)
    m_reverseSort = -1;
  else
  m_reverseSort = 1;*/
  for (it = nodes.begin(); it != nodes.end(); it++)
    {
      temp = (*it).first;
      SortChildren(temp);
    }
  SaveML();
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
	  wxFile *FileInfos = new wxFile(FileToAdd);
	  wxString length_str;
	  int popo = FileInfos->Length();
	  length_str << popo;

	  for (vector<wxString>::iterator iter = Exts.begin(); iter != Exts.end(); iter++)
	    if (iter->Contains(File->GetExt().Lower()) == true)
	      {
		s_nodeInfo		infos;
		int			slashPos;
		wxTreeItemId		selection;

		infos = SetStructInfos(infos, FileToAdd, File->GetExt(), length_str);
		slashPos = FileToAdd.Find('/', true);
		selection = GetSelection();
		if (selection.IsOk() == true && selection != GetRootItem())
		  this->AddFile(selection, FileToAdd.Mid(slashPos + 1), infos, true);
		else
		  this->AddFile(GetTreeItemIdFromLabel(_("Sounds")), FileToAdd.Mid(slashPos + 1), infos, true);
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
  LoadML();
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
       MediaLibraryPanel->TopToolbar->EnableTool(4, true);
       MediaLibraryPanel->BottomToolbar->EnableTool(5, true);
     }
   else if (GetItemParent(item) != GetRootItem() && item != GetRootItem())
     {
       MediaLibraryPanel->TopToolbar->EnableTool(1, true);
       MediaLibraryPanel->TopToolbar->EnableTool(2, true);
       MediaLibraryPanel->TopToolbar->EnableTool(4, false);
       MediaLibraryPanel->BottomToolbar->EnableTool(5, false);
     }
   else
     {
       MediaLibraryPanel->TopToolbar->EnableTool(1, true);
       MediaLibraryPanel->TopToolbar->EnableTool(2, false);
       MediaLibraryPanel->TopToolbar->EnableTool(4, false);
       //       MediaLibraryPanel->BottomToolbar->EnableTool(5, false);
     }

}

void				MLTree::BeginDrag(wxTreeEvent &event)
{
  event.Allow();
  item_to_drag = event.GetItem();
  selection_length = GetSelections(selection);
}

wxTreeItemId                    MLTree::Copy(wxTreeItemId item)
{
  wxTreeItemId		dad_item;
  s_nodeInfo		infos;
  wxString		selfile;
  wxTreeItemId		itemAdded;
  int			slashPos;


  wxFileName	        *File = new wxFileName(selfile);


  infos = GetTreeItemStructFromId(item_to_drag);
  if (infos.label != wxT(""))
    {
      if (infos.extention.Cmp(wxT("")))
	{
	  selfile = infos.label;
	  slashPos = selfile.Find('/', true);
	  AddFile(item, selfile.Mid(slashPos + 1), infos, false);
	  return item;
	}
      else
	{
	  itemAdded = AppendItem(item, GetItemText(item_to_drag));
	  SetItemImage(itemAdded, 0);
	  nodes[itemAdded] = infos;
	  return itemAdded;
	}
    }

}
void                          MLTree::DragAndDrop(wxTreeItemId item)
{
 wxTreeItemIdValue cookie = &Tree;
 wxTreeItemId		item_to_drag_save;
 wxTreeItemId		item_save;


 item = Copy(item);
 if (item_to_drag == item_begin)
   return;
 for (item_to_drag = GetFirstChild(item_to_drag, cookie); item_to_drag.IsOk(); item_to_drag = GetFirstChild(item_to_drag, cookie))
   {
     if (item_to_drag == item_begin)
       return;
     item_save = item;
     item = Copy(item);
     item_to_drag_save = item_to_drag;
     for (item_to_drag = GetNextSibling(item_to_drag); item_to_drag.IsOk(); item_to_drag = GetNextSibling(item_to_drag))
       {
	 if (item_to_drag == item_begin)
	    return;
	 DragAndDrop(item_save);
       }

     item_to_drag = item_to_drag_save;
   }
}
void				MLTree::EndDrag(wxTreeEvent &event)
{
  wxTreeItemId		item;
  int			i;

  item = event.GetItem();
  item_begin = item;
  if (item.IsOk() && item != item_to_drag && item != GetRootItem() && GetItemParent(item) != item_to_drag && !GetTreeItemStructFromId(item).extention.Cmp(wxT("")))
    {
      DragAndDrop(item);
      OnRemove();
      Expand(item_begin);
    }
}
void				MLTree::OnLeftClick(wxMouseEvent &event)
{
  int		x;
  int		y;
  wxString     	selfile;
  s_nodeInfo   	infos;

  infos = GetTreeItemStructFromId(item_to_drag);
  if (infos.label != wxT(""))
    {
      if (infos.extention.Cmp(wxT("")))
	{
	  selfile = infos.label;
	  x = event.GetPosition().x;
	  y = event.GetPosition().y;
	  ClientToScreen(&x, &y);
	  SeqPanel->Drop(x, y, selfile);
	}
      SeqPanel->HideAllPatterns(event);
    }
  event.Skip();
}

void				MLTree::OnSuppr(wxKeyEvent &event)
{

//   if (wxGetKeyState())
//     {
//       cout << "yeeeee" << endl;
//     }
  int key = event.GetKeyCode();
  //cout << key << endl;
  if (key == WXK_DELETE)
    {
      OnRemove();
      cout << "removing" << endl;
    }
}

BEGIN_EVENT_TABLE(MLTree, wxTreeCtrl)
  EVT_RIGHT_UP(MLTree::OnRightClick)
  EVT_TREE_SEL_CHANGED(MLTree_Selected, MLTree::OnSelChange)
  EVT_TREE_BEGIN_DRAG(MLTree_Selected, MLTree::BeginDrag)
  EVT_TREE_END_DRAG(MLTree_Selected, MLTree::EndDrag)
  EVT_LEFT_UP(MLTree::OnLeftClick)
  EVT_KEY_DOWN(MLTree::OnSuppr)
END_EVENT_TABLE()
