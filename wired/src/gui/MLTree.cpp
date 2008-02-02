// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <vector>
#include <wx/filename.h>
#include <wx/wx.h>
#include <wx/file.h>
#include <wx/treectrl.h>
#include <wx/defs.h>
#include "MLTree.h"
#include "MediaLibrary.h"
#include "MLTreeInfos.h"
#include "MainWindow.h"
#include "Sequencer.h"
#include "SequencerGui.h"
#include "Colour.h"
#include "HelpPanel.h"
#include "DownButton.h"
#include "HoldButton.h"
#include "StaticLabel.h"
#include "Settings.h"
#include "AudioEngine.h"
#include "file.xpm"
#include "audio.xpm"
#include "folder.xpm"
#include "folder-open.xpm"
#include "delete.xpm"
#include "SaveCenter.h"
#include "MLTraverser.h"
#include "debug.h"

#ifdef DEBUG_MLTREE
#define LOG { wxFileName __filename__(__FILE__); cout << __filename__.GetFullName() << " : "  << __LINE__ << " : " << __FUNCTION__  << endl; }
#else
#define LOG
#endif

using namespace std;

extern SaveCenter	*saveCenter;

extern const wxEventType EVT_DROP;

void s_nodeInfo::Set(const wxString &label, const wxString &extension, const wxString &length)
{
  LOG;
  _label = label;
  _extension = extension;
  _length = length;
}

void s_nodeInfo::Set(const wxString &label, const wxString &extension, const wxFileOffset &length)
{
  LOG;
  _label = label;
  _extension = extension;
  _length << length;
}

MLTree::MLTree(wxWindow *MediaLibraryPanel, wxPoint p, wxSize s, long style)
  : wxTreeCtrl(MediaLibraryPanel, MLTree_Selected, p, s, style), WiredDocument(wxT("MLTree"), NULL)
{
  LOG;
  SetIndent(10);
  /* Set the Root node with the project's name in label */
  _root = AddRoot(saveCenter->getProjectName());
  SetItemBold(_root);

  /* Create Image List */
  wxImageList *images = new wxImageList(ICON_SIZE, ICON_SIZE, TRUE);
  AddIcon(images, wxIcon(folder_xpm));
  AddIcon(images, wxIcon(folder_open_xpm));
  AddIcon(images, wxIcon(audio_xpm));
  AddIcon(images, wxIcon(file_xpm));
  AssignImageList(images);

  /* Create basic nodes */
  s_nodeInfo infos(LOCAL_NODE, wxT(""), wxT(""));
  wxTreeItemId itemTemp = AppendItem(_root, LOCAL_NODE_NAME);
  SetItemImage(itemTemp, 0);
  _nodes[itemTemp] = infos;

  s_nodeInfo infos1(PROJECT_NODE, wxT(""), wxT(""));
  itemTemp = AppendItem(_root, PROJECT_NODE_NAME);
  SetItemImage(itemTemp, 0);
  _nodes[itemTemp] = infos1;

  LoadKnownExtensions();
  //temp
  // DisplayNodes();

  Connect(ML_ID_MENU_DELETE, wxEVT_COMMAND_MENU_SELECTED,
    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) &MLTree::OnRemove);
  Connect(ML_ID_MENU_CREATEDIR, wxEVT_COMMAND_MENU_SELECTED,
    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) &MLTree::OnCreateDir);
  Connect(ML_ID_MENU_INSERT, wxEVT_COMMAND_MENU_SELECTED,
    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) &MLTree::OnInsert);
  Connect(ML_ID_MENU_EDIT, wxEVT_COMMAND_MENU_SELECTED,
    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) &MLTree::OnEdit);
  Connect(ML_ID_MENU_PREVIEW, wxEVT_COMMAND_MENU_SELECTED,
    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) &MLTree::OnPreview);
  Connect(ML_ID_MENU_INFOS, wxEVT_COMMAND_MENU_SELECTED,
    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) &MLTree::DisplayInfos);
  Connect(ML_ID_MENU_ADDDIR, wxEVT_COMMAND_MENU_SELECTED,
    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) &MLTree::ImportDir);

  LoadLocalTree();
}

MLTree::~MLTree()
{
  LOG;
  SaveLocalTree();
}

void MLTree::SaveTreeSC(wxTreeItemId parent, SaveElement *parentElem, bool relativePath)
{
  LOG;
  wxTreeItemIdValue	cookie;
  s_nodeInfo		infos;
  wxTreeItemId		item = GetFirstChild(parent, cookie);
  wxTreeItemId		item_last = GetLastChild(parent);
  SaveElement		*currSaveElem = NULL;
  wxString		text;
  wxFileName		path;

  while (item.IsOk())
  {
    text = GetItemText(item);
    infos = GetTreeItemStructFromId(item);

    path.Assign(infos._label);
    if(relativePath)
      path.MakeRelativeTo(saveCenter->getAudioDir());

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
      SaveTreeSC(item, currSaveElem, relativePath);
    }
    else // no children
    {
      if (infos._extension.Cmp(wxT("")))
      {
        currSaveElem->setKey(wxT("file"));
        currSaveElem->addAttribute(wxT("name"), text);
        currSaveElem->addAttribute(wxT("infos_label"), path.GetFullPath());
        currSaveElem->addAttribute(wxT("infos_length"), infos._length);
        currSaveElem->addAttribute(wxT("infos_ext"), infos._extension);
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

void MLTree::OnSave(wxString filename)
{
  LOG;
  wxString	mlpath = wxString(saveCenter->getProjectPath().GetFullPath() + wxT("MediaLibrary/MLTree"));
  cout << "le path de la ml : '" << mlpath.mb_str() << "'" << endl;
  SavePatch(mlpath, filename);
}

void MLTree::Save()
{
  LOG;
  SaveElement	*rootTreeSaveElem = new SaveElement();
  rootTreeSaveElem->setKey(PROJECT_NODE);
  SaveTreeSC(GetTreeItemIdFromLabel(PROJECT_NODE), rootTreeSaveElem, true);
  saveDocData(rootTreeSaveElem, SAVE_TREE_FILE);
}

void MLTree::SaveLocalTree()
{
  LOG;
  SaveElement	*rootTreeSaveElem = new SaveElement();
  rootTreeSaveElem->setKey(LOCAL_NODE);
  SaveTreeSC(GetTreeItemIdFromLabel(LOCAL_NODE), rootTreeSaveElem, false);
  saveDocData(rootTreeSaveElem, LOCAL_TREE_FILE);
  SavePatch(LOCAL_TREE_FILE, WiredSettings->HomeDir);
}

void MLTree::LoadLocalTree()
{
  LOG;
  //Another awful copy paste... we have to do something soon

  SaveElementArray	treeData;
  SaveElement		*rootSaveElem;
  wxTreeItemId		localNode;

  localNode = GetTreeItemIdFromLabel(LOCAL_NODE);

  DeleteChildren(localNode);

  treeData = AskData(LOCAL_TREE_FILE, true);

  if(treeData.GetCount() > 0)
    {
      rootSaveElem = treeData.Item(0);
      while(rootSaveElem->getKey() != LOCAL_NODE && rootSaveElem->hasChildren())
	rootSaveElem = rootSaveElem->getChildren().Item(0);

      if(rootSaveElem->getKey() == LOCAL_NODE)
	LoadItem(localNode, rootSaveElem);
    }

}

void				MLTree::LoadPatch(wxString filename)
{
  LOG;
  //Adapted Copy Paste of Load, didn't want to bother trying to make things clean...
  std::cerr << "[MLTree] Load" << std::endl;

  SaveElementArray	treeData;
  SaveElement		*rootSaveElem;
  wxTreeItemId		projectNode;

  projectNode = GetTreeItemIdFromLabel(PROJECT_NODE);

  DeleteChildren(projectNode);

  treeData = AskData(filename);
  if(treeData.GetCount() > 0)
    {
      rootSaveElem = treeData.Item(0);
      while(rootSaveElem->getKey() != PROJECT_NODE && rootSaveElem->hasChildren())
	rootSaveElem = rootSaveElem->getChildren().Item(0);

      if(rootSaveElem->getKey() == PROJECT_NODE)
	LoadItem(projectNode, rootSaveElem);
    }

}

void MLTree::LoadItem(wxTreeItemId parent, SaveElement *parentData)
{
  LOG;
    SaveElementArray    saveElemChildren;
    SaveElement         *currSaveElem;
    s_nodeInfo          infos;
    bool                expand;
    wxTreeItemId        next;
    wxFileName          path;
    saveElemChildren = parentData->getChildren();
    for (unsigned int i = 0; i < saveElemChildren.GetCount(); i++)
    {
        currSaveElem = saveElemChildren.Item(i);
        if(currSaveElem->getKey() == wxT("folder"))
        {
            infos.Set(currSaveElem->getAttribute(wxT("name")), wxT(""), wxT(""));
            if(currSaveElem->getAttribute(wxT("is_expanded")) == wxT("true"))
                expand = true;
            else
                expand = false;
            next = AddFile(parent, infos._label, infos, expand);
            if (expand)
                SetItemImage(next, 1);
            else
                SetItemImage(next, 0);
            LoadItem(next, currSaveElem);
        }
        else if (currSaveElem->getKey() == wxT("file"))
        {
            path = currSaveElem->getAttribute(wxT("infos_label"));
            if (path.IsRelative())
                path.MakeAbsolute(saveCenter->getAudioDir());
            infos.Set(path.GetFullPath(), currSaveElem->getAttribute(wxT("infos_ext")), currSaveElem->getAttribute(wxT("infos_length")));
            AddFile(parent, currSaveElem->getAttribute(wxT("name")), infos, false);
        }
        if (parentData->getAttribute(wxT("is_expanded")) == wxT("true"))
            Expand(parent);
    }
}

void				MLTree::Load(SaveElementArray data)
{
  LOG;
  //We assume that data contains only one SaveElement : The root node.
  //else, something has failed in the save or the file is corrupted...

  std::cerr << "[MLTree] Load" << std::endl;

  SaveElement		*ref;
  SaveElementArray	treeData;
  wxString		treeFile;
  SaveElement		*rootSaveElem;

  ref = data.Item(0);
  treeFile = ref->getValue();

  treeData = AskData(treeFile);
  if(treeData.GetCount() > 0)
    {
      rootSaveElem = treeData.Item(0);
      while(rootSaveElem->getKey() != PROJECT_NODE && rootSaveElem->hasChildren())
      {
	rootSaveElem = rootSaveElem->getChildren().Item(0);
      }

      if(rootSaveElem->getKey() == PROJECT_NODE)
	LoadItem(GetTreeItemIdFromLabel(PROJECT_NODE), rootSaveElem);
    }
}

/*void			MLTree::SaveTree(WiredSessionXml *XmlSession, wxTreeItemId parent)
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
*/

void				MLTree::AddIcon(wxImageList *images, wxIcon icon)
{
  LOG;
  int wInit = icon.GetWidth();
  int hInit = icon.GetWidth();

  if (wInit == ICON_SIZE && hInit == ICON_SIZE)
    images->Add(icon);
  else
    images->Add(wxBitmap(wxBitmap(icon).ConvertToImage().Rescale(ICON_SIZE, ICON_SIZE)));
}

// Load extensions known by wired
bool MLTree::LoadKnownExtensions()
{
  LOG;
  wxTextFile file(WiredSettings->ConfDir + EXT_FILE);
  wxString   l;
  wxString   *itemdata = NULL;
  _filters = wxT("");
  if (file.Open())
  {
    for (l = file.GetFirstLine(); ; l = file.GetNextLine())
    {
      l.Trim(false);
      l = l.BeforeFirst('#');
      if (!l.IsEmpty())
      {
        itemdata = new wxString(l.BeforeFirst('\t'));
        _Exts.push_back(*itemdata);
        _filters += *itemdata + wxT(";");
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

  //   Display known extensions
  //   for (vector<wxString>::iterator iter = Exts.begin(); iter != Exts.end(); iter++)
  //     {

  //       cout << "[MEDIALIBRARY] : " << *iter << endl;
  //     }
  return (true);
}

void MLTree::SetTreeExpanded()
{
  LOG;
  _collapsed = false;
}

void MLTree::SetTreeCollapsed()
{
  LOG;
  _collapsed = true;
}

bool MLTree::IsTreeCollapsed()
{
  LOG;
  return (_collapsed);
}

// When creating a directory
void MLTree::OnCreateDir()
{
  LOG;
  s_nodeInfo infos(_("New Directory"), wxT(""), wxT(""));
  wxTreeItemId itemParent = GetSelection();
  wxTreeItemId itemAdded = AppendItem(itemParent, _("New Directory"));

  SetItemImage(itemAdded, 0);
  _nodes[itemAdded] = infos;
  Expand(itemParent);
  EditLabel(itemAdded);
}

wxTreeItemId MLTree::OnCreateDirName(wxString dirName, wxTreeItemId itemParent)
{
  LOG;
  s_nodeInfo        infos(dirName, wxT(""), wxT(""));
  wxTreeItemId      itemAdded;
  wxTreeItemIdValue cookie;
  wxTreeItemId      itemSearch = GetFirstChild(itemParent, cookie);

  while (itemSearch.IsOk())
  {
    if (GetItemText(itemSearch).Cmp(dirName) == 0)
      return (itemSearch);
    itemSearch = GetNextChild(itemParent, cookie);
  }
  itemAdded = AppendItem(itemParent, dirName);
  SetItemImage(itemAdded, 0);
  _nodes[itemAdded] = infos;
  Expand(itemParent);
  //EditLabel(itemAdded);
  return (itemAdded);
}

// When inserting a file
void				MLTree::OnInsert()
{
  LOG;
  wxCommandEvent		event;
  MediaLibraryPanel->OnInsert(event);
}

// When editing a file
void				MLTree::OnEdit()
{
  LOG;
  wxCommandEvent		event;

  MediaLibraryPanel->OnEdit(event);
}

// When previewing a file
void				MLTree::OnPreview()
{
  LOG;
  wxCommandEvent		event;

  MediaLibraryPanel->OnPreview(event);
}

// Display infos corresponding to a node
void MLTree::DisplayInfos()
{
  LOG;
  s_nodeInfo info = GetTreeItemStructFromId(GetSelection());
  _mouse_pos.y += 50;
  _mouse_pos.x += 30;
  // BAD Why a new (infos will be lost, delete impossible)
  MLTreeInfos *infos = new MLTreeInfos(MediaLibraryPanel, wxPoint(2, _mouse_pos.y), wxSize(197, 100), wxCAPTION | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxTHICK_FRAME, info);
}

// When adding a file
wxTreeItemId MLTree::AddFile(wxTreeItemId ParentNode, wxString FileToAdd, s_nodeInfo infos, bool expand)
{
  LOG;
  wxTreeItemId itemToAdd;
  if (!ParentNode.IsOk())
    ParentNode = _root;
  for (nodeInfoMap::iterator it = _nodes.begin(); it != _nodes.end(); it++)
    if ((*it).second._label == infos._label && GetItemParent((*it).first) == ParentNode)
      return (itemToAdd);
  itemToAdd = AppendItem(ParentNode, FileToAdd);
  if (expand == true)
    Expand(ParentNode);
  SetItemImage(itemToAdd, 3);
  _nodes[itemToAdd] = infos;
  return (itemToAdd);
}

wxTreeItemId MLTree::AddFileInProject(wxString FileToAdd, bool expand)
{
  LOG;
  wxTreeItemId itemToAdd;
  wxFileName   File(FileToAdd);
  wxFile       FileInfos(FileToAdd);
  s_nodeInfo   infos(FileToAdd, File.GetExt(), FileInfos.Length());
  wxTreeItemId ParentNode = GetTreeItemIdFromLabel(PROJECT_NODE);
  for (nodeInfoMap::iterator it = _nodes.begin(); it != _nodes.end(); it++)
    if ((*it).second._label == infos._label && GetItemParent((*it).first) == ParentNode)
      return (itemToAdd); // BAD Return an unitialized value ???
  itemToAdd = AppendItem(ParentNode, File.GetFullName());
  if (expand == true)
    Expand(ParentNode);
  SetItemImage(itemToAdd, 3);
  _nodes[itemToAdd] = infos;
  return (itemToAdd);
}

wxTreeItemId MLTree::DelFileInProject(wxString FileToDel, bool expand)
{
  LOG;
  wxFileName fileName(FileToDel);
  wxTreeItemId ParentNode = GetTreeItemIdFromLabel(PROJECT_NODE);
  for (nodeInfoMap::iterator it = _nodes.begin(); it != _nodes.end(); it++)
  {
    wxFileName curFileName((*it).second._label);
    if (curFileName.GetFullName() == fileName.GetFullName() && GetItemParent((*it).first) == ParentNode)
    {
      _nodes.erase((*it).first);
      DeleteChildren((*it).first);
      Delete((*it).first);
    }
  }
  DisplayNodes();
}

// Return an item Id from a label node
wxTreeItemId MLTree::GetTreeItemIdFromLabel(wxString label)
{
  LOG;
  wxTreeItemId ItemToReturn;
  for (nodeInfoMap::iterator i = _nodes.begin(); i != _nodes.end(); i++ )
    {
        s_nodeInfo temp;
        temp = (*i).second;
        if (temp._label.Cmp(label) == 0)
            ItemToReturn = (*i).first;
    }
    return (ItemToReturn);
}

// DEBUG display nodes names
void MLTree::DisplayNodes()
{
  LOG;
  //cout << "MLTree::DisplayNodes() : Why am i called ?" << endl;
  int cnt = 0;
  for (nodeInfoMap::iterator it = _nodes.begin(); it != _nodes.end(); it++)
  {
    s_nodeInfo temp;
    temp = (*it).second;
    //      cout << "value : [" << temp.label << "]" << endl;
    cnt++;
  }
  //  cout << "  total : " << cnt << endl;
}

// Compare two existing items
int MLTree::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
  LOG;
  nodeInfoMap::iterator it;
  wxString	text1;
  wxString	text2;
  int		slashPos;

  //  cout << "selected : " << selected.mb_str() << endl;
  for (it = _nodes.begin(); it != _nodes.end(); it++)
  {
    if ((*it).first == item1)
    {
      s_nodeInfo temp;
      temp = (*it).second;
      if (!selected.Cmp(wxT("filesize")))
        text1 = temp._length;
      else if (!selected.Cmp(wxT("filename")))
      {
        text1 = temp._label;
        slashPos = text1.Find('/', true);
        text1 = text1.Mid(slashPos + 1);
      }
      else if (!selected.Cmp(wxT("filetype")))
        text1 = temp._extension;
      else
        text1 = temp._label;
      }
  }
  for (it = _nodes.begin(); it != _nodes.end(); it++)
  {
    if ((*it).first == item2)
    {
      s_nodeInfo temp;
      temp = (*it).second;
      if (!selected.Cmp(wxT("filesize")))
        text2 = temp._length;
      else if (!selected.Cmp(wxT("filename")))
      {
        text2 = temp._label;
        slashPos = text2.Find('/', true);
        text2 = text2.Mid(slashPos + 1);
      }
      else if (!selected.Cmp(wxT("filetype")))
        text2 = temp._extension;
      else
        text2 = temp._label;
    }
  }
  // cout << "text1: " << text1.mb_str() << " | text2: " << text2.mb_str() << "comp: " << text1.Cmp(text2) << endl;
  return (text1.Cmp(text2));
}

// Sort existing nodes
void MLTree::SortNodes(wxString MLselected)
{
  LOG;
  wxTreeItemId temp;
  selected = MLselected;
  /*if (m_reverseSort == 1)
    m_reverseSort = -1;
  else
  m_reverseSort = 1;*/
  for (nodeInfoMap::iterator it = _nodes.begin(); it != _nodes.end(); it++)
  {
    temp = (*it).first;
    SortChildren(temp);
  }

  saveCenter->SaveFile(this, SAVE_TREE_FILE);
}

// Return the struct associated to a node, from an ID
// BAD Return an uninitialized struct
s_nodeInfo MLTree::GetTreeItemStructFromId(wxTreeItemId ItemToFind)
{
  LOG;
  s_nodeInfo structToReturn;
  for (nodeInfoMap::iterator i = _nodes.begin(); i != _nodes.end(); i++)
    if ((*i).first == ItemToFind)
      return ((*i).second);
  // BAD Return an uninitialized variable.
  return (structToReturn);
}

// When adding a file/directory
void MLTree::OnAdd(wxString FileToAdd)
{
  LOG;
  if (!FileToAdd.empty())
  {
    wxFileName File(FileToAdd);
    if (File.FileExists() == true)
    {
      wxFile FileInfos(FileToAdd);
      for (wxStringVector::iterator iter = _Exts.begin(); iter != _Exts.end(); iter++)
        if (iter->Contains(File.GetExt().Lower()) == true)
        {
          s_nodeInfo infos(FileToAdd, File.GetExt(), FileInfos.Length());
          wxTreeItemId selection;
          selection = GetSelection();
          if (selection.IsOk() == true && selection != GetRootItem())
            this->AddFile(selection, File.GetFullName(), infos, true);
          else
            this->AddFile(GetTreeItemIdFromLabel(LOCAL_NODE), File.GetFullName(), infos, true);
        }
        DisplayNodes();
    }
  }
}

void MLTree::OnAddOnNode(wxString FileToAdd, wxTreeItemId selection)
{
  LOG;
  if (!FileToAdd.empty())
  {
    wxFileName File(FileToAdd);
    if (File.FileExists() == true)
    {
      wxFile *FileInfos = new wxFile(FileToAdd);
      for (wxStringVector::iterator iter = _Exts.begin(); iter != _Exts.end(); iter++)
      {
        wxString ExtToCheck;
        wxString ExtList = (*iter);
        while (ExtList.BeforeFirst(';').Cmp(wxT("")) != 0)
        {
          ExtToCheck = ExtList.BeforeFirst(';');
          ExtList = ExtList.AfterFirst(';');
          //Check ext
          if (ExtToCheck.Cmp(wxT("")) != 0 && ExtToCheck.Cmp(File.GetExt().Lower()) == 0)
          {
            s_nodeInfo infos(FileToAdd, File.GetExt(), FileInfos->Length());
            int slashPos;
            //wxTreeItemId		selection;
            slashPos = FileToAdd.Find('/', true);
            //selection = GetSelection();
            if (selection.IsOk() == true && selection != GetRootItem())
              this->AddFile(selection, FileToAdd.Mid(slashPos + 1), infos, true);
            else
              this->AddFile(GetTreeItemIdFromLabel(LOCAL_NODE), FileToAdd.Mid(slashPos + 1), infos, true);
          }
        }
      }
      // we need to delete FileInfos so that the file doesn't stay open
      delete FileInfos;
      DisplayNodes();
    }
  }
}

void MLTree::ImportDir()
{
  LOG;
  wxDirDialog   dlg(this, _("Import directory"), wxGetCwd());
  if (dlg.ShowModal() == wxID_OK)
  {
    wxString seldir = dlg.GetPath();
    this->OnAddDirectory(seldir);
    if (CheckEmptyDir(GetTreeItemIdFromLabel(LOCAL_NODE)) == 0)
      wxMessageBox(wxT("This folder does not contain any allowed file."));
  }
}

int				MLTree::CheckEmptyDir(wxTreeItemId itemParent)
{
  LOG;
	wxTreeItemId							itemSearch;
	wxTreeItemIdValue				cookie;
	int 											nb_item = 0;
	vector<wxTreeItemId>		ItemToDel;

	//Search item to del
	itemSearch = GetFirstChild(itemParent, cookie);
	while (itemSearch.IsOk())
	{
		if (GetItemImage(itemSearch, wxTreeItemIcon_Normal) == 0) { //Is folder
			if (CheckEmptyDir(itemSearch) != 0)
				nb_item++;
			else {
				ItemToDel.push_back(itemSearch);
			}
		}
		else	nb_item++;
		itemSearch = GetNextChild(itemParent, cookie);
	}
	//Del item
   for (vector<wxTreeItemId>::iterator iter = ItemToDel.begin(); iter != ItemToDel.end(); iter++)
		this->Delete(*iter);
	return nb_item;
}



wxTreeItemId	MLTree::getOrCreateNodeFromFName(wxString f, wxTreeItemId myRoot)
{
  LOG;
  if (f.BeforeFirst('/').Cmp(wxT("")) != 0)
  {
    myRoot = OnCreateDirName(f.BeforeFirst('/'), myRoot);
    return (getOrCreateNodeFromFName(f.AfterFirst('/'), myRoot));
  }
  return (myRoot);
}

void				MLTree::OnAddDirectory(wxString DirToAdd)
{
  LOG;
  //DirToAdd.Append("/");
  if (!DirToAdd.empty())
  {
    wxFileName	File(DirToAdd);

    if (File.DirExists())
    {
      wxArrayString		files;
      wxDirTraverserSimple	traverser(files);
      wxDir			Dir(DirToAdd);

      files.Empty();
      Dir.Traverse(traverser);
      if (!files.IsEmpty())
      {
	size_t			nbFiles = files.GetCount();
	wxString		f;
	int			i;
	wxTreeItemId	selection;
	wxTreeItemId	myRoot;
	wxTreeItemId	node;

	selection = GetSelection();
	myRoot = OnCreateDirName(DirToAdd.AfterLast('/'), selection);
	//myNodes.push_back(myRoot);
	for (i = (int)(nbFiles) - 1; i >= 0 ; i--)
	{
	  f = files.Item(i);
	  wxFileName	t(f);
	  t.MakeRelativeTo(DirToAdd);
	  node = getOrCreateNodeFromFName(t.GetPath(), myRoot);
	  OnAddOnNode(f, node);
	}
      }
    }
  }
}

// Return selection label
wxString MLTree::getSelection(int flag)
{
  LOG;
    int selection_length = GetSelections(selection);
    for (int i = 0; i < selection_length; i++)
        if (GetItemParent(selection[i]) != GetRootItem() && selection[i] != GetRootItem())
            if (!flag)
            {
                return (GetItemText(selection[i]));
            }
            else
            {
                s_nodeInfo		temp;
                temp = GetTreeItemStructFromId(selection[i]);
                return (temp._label);
            }
    return (wxT(""));
}

// When removing an element
void				MLTree::OnRemove()
{
  LOG;
  wxArrayTreeItemIds		selection;
  int				selection_length;
  int				i;

  selection_length = GetSelections(selection);
  for (i = 0; i < selection_length; i++)
    {
      if (GetItemParent(selection[i]) != GetRootItem() && selection[i] != GetRootItem())
	{
	  // wxTreeItemId temp = GetItemText(selection[i]);
	  _nodes.erase(selection[i]);
	  DeleteChildren(selection[i]);
	  Delete(selection[i]);
	}
    }
  DisplayNodes();
}

// Expand all existing nodes
void MLTree::ExpandAll(wxTreeCtrl *Tree, const wxTreeItemId& id, bool shouldExpand, int toLevel)
{
  LOG;
    if (toLevel == 0 || !ItemHasChildren(id))
        return;
    bool isExpanded = IsExpanded(id);
    if (shouldExpand && !isExpanded)
        Expand(id);
    else if (!shouldExpand && isExpanded)
        Collapse(id);
    // Tree or _Tree ??? (see rev. 1439)
    wxTreeItemIdValue cookie = &Tree;
    for (wxTreeItemId child = GetFirstChild(id, cookie); child.IsOk(); child = GetNextChild(id, cookie))
        ExpandAll(Tree, child, shouldExpand, toLevel - 1);
  //LoadML();
}

// Collapse all existing nodes
void				MLTree::OnCollapse()
{
  LOG;
  //  cout << "[MEDIALIBRARY] Expand/Collapse Tree (OnCollapse)" << endl;
  // TODO : Find a way to get the max depth of node childs.
  // Currently using 100 as an arbitrary max depth
  if (IsTreeCollapsed() == true)
    {
      ExpandAll(_Tree, GetRootItem(), true, 100);
      SetTreeExpanded();
    }
  else
    {
      wxTreeItemIdValue cookie = &_Tree;
      ExpandAll(_Tree, GetRootItem(), false, 100);
      EnsureVisible(GetFirstChild(GetRootItem(), cookie));
      SetTreeCollapsed();
    }
}

// Display menu on right click
void MLTree::OnRightClick(wxMouseEvent& event)
{
  LOG;
  wxTreeItemId		item;

  _mouse_pos = event.GetPosition();
  item = GetSelection();
  if (item == GetRootItem())
    return ;
  wxMenu myMenu;
  s_nodeInfo infos = GetTreeItemStructFromId(item);
  if (infos._extension.Cmp(wxT("")))
  {
    myMenu.Append(ML_ID_MENU_INFOS, _("Infos"), _("Infos"));
    myMenu.Append(ML_ID_MENU_PREVIEW, _("Preview"), _("Preview"));
    myMenu.Append(ML_ID_MENU_INSERT, _("Insert"), _("Insert"));
#ifndef FEATURE_DISABLE_WAVE_EDITOR
    myMenu.Append(ML_ID_MENU_EDIT, _("Edit"), _("Edit"));
#endif
    myMenu.AppendSeparator();
  }
  else
  {
    myMenu.Append(ML_ID_MENU_CREATEDIR, _("New Directory"), _("New Directory"));
    myMenu.Append(ML_ID_MENU_ADDDIR, _("Import Directory"), _("Import Directory"));
  }
  myMenu.Append(ML_ID_MENU_DELETE, _("Delete"), _("Delete"));
  PopupMenu(&myMenu);
}

void				MLTree::OnSelChange(wxTreeEvent &event)
{
  LOG;
   wxTreeItemId		item;
   s_nodeInfo		infos;

   item = GetSelection();

   infos = GetTreeItemStructFromId(item);
   if (infos._extension.Cmp(wxT("")))
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
  LOG;
  event.Allow();
  item_to_drag = event.GetItem();
  selection_length = GetSelections(selection);
}

wxTreeItemId                    MLTree::Copy(wxTreeItemId item)
{
  LOG;
  wxTreeItemId		dad_item;
  s_nodeInfo		infos;
  wxString		selfile;
  wxTreeItemId		itemAdded;
  int			slashPos;


  wxFileName	        *File = new wxFileName(selfile);


  infos = GetTreeItemStructFromId(item_to_drag);
  if (infos._label != wxT(""))
    {
      if (infos._extension.Cmp(wxT("")))
	{
	  selfile = infos._label;
	  slashPos = selfile.Find('/', true);
	  AddFile(item, selfile.Mid(slashPos + 1), infos, false);
	  return item;
	}
      else
	{
	  itemAdded = AppendItem(item, GetItemText(item_to_drag));
	  SetItemImage(itemAdded, 0);
	  _nodes[itemAdded] = infos;
	  return itemAdded;
	}
    }
}

void MLTree::DragAndDrop(wxTreeItemId item)
{
  LOG;
    // Assign a wxTreeCtrl to a wxTreeItemIdValue ???
    wxTreeItemIdValue cookie = &_Tree;
    wxTreeItemId item_to_drag_save;
    wxTreeItemId item_save;

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

void MLTree::EndDrag(wxTreeEvent &event)
{
  LOG;
    wxTreeItemId item = event.GetItem();
    item_begin = item;
    s_nodeInfo infos;
    for (int i = 0; i < selection_length; i++)
    {
        if (GetItemParent(selection[i]) != GetRootItem() && selection[i] != GetRootItem())
        {
            item_to_drag = selection[i];
            for (nodeInfoMap::iterator it = _nodes.begin(); it != _nodes.end(); it++)
            {
                if ((*it).first == item_to_drag)
                {
                    infos = (*it).second;
                    break;
                }
            }
            if (item.IsOk() &&
                item != item_to_drag &&
                item != GetRootItem() &&
                GetItemParent(item) != item_to_drag &&
                GetItemParent(item_to_drag) != item &&
                !GetTreeItemStructFromId(item)._extension.Cmp(wxT("")))
                {
                    DragAndDrop(item);
                    Expand(item_begin);
                }
        }
    }
    if (item.IsOk() &&
        item != item_to_drag &&
        item != GetRootItem() &&
        GetItemParent(item) != item_to_drag &&
        GetItemParent(item_to_drag) != item &&
        !GetTreeItemStructFromId(item)._extension.Cmp(wxT("")))
            OnRemove();
}



void MLTree::OnLeftClick(wxMouseEvent &event)
{
  LOG;
  int x, y;
  s_nodeInfo infos = GetTreeItemStructFromId(item_to_drag);

  if (infos._label != wxT(""))
  {
	if (infos._extension.Cmp(wxT("")))
	{
	  _Selfile = infos._label;
	  x = event.GetPosition().x;
	  y = event.GetPosition().y;
	  ClientToScreen(&x, &y);
	  wxCommandEvent event(EVT_DROP, ID_EVT_DROP);
	  _Pos.x = x;
	  _Pos.y = y;
	  wxPostEvent(SeqPanel->GetEventHandler(), event);
	}
	SeqPanel->HideAllPatterns(event);
  }
  event.Skip();
}

void				MLTree::OnSuppr(wxKeyEvent &event)
{
  LOG;
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
