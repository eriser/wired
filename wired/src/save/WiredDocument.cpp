// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991
#include "WiredDocument.h"

//Only used for debug
#include <iostream>

#include <SaveCenter.h>
#include "Settings.h"

extern SaveCenter	*saveCenter;
extern Settings		*WiredSettings;

using namespace std;
long WiredDocument::id = 0;

WiredDocument::WiredDocument(wxString name, WiredDocument *parent, bool noParent)
{
  // making a unique id for each document
  increaseId();
  _id = id;
  // registering this id to avoid duplicates
  // if we aren't in a plugin, id == _id
  // but still, this way we keep track of the growing id
  _id = saveCenter->RegisterId(id);

  if (!parent && !noParent)
    parent = saveCenter;

  if (parent == NULL && !noParent)
  {
    std::cerr << "[WiredDocument] No parent for " << name.mb_str() << " !!" << std::endl;
    std::cerr << "[WiredDocument] Creating a new one... !?!" << std::endl;
    saveCenter = new SaveCenter();
    parent = saveCenter;
  }
  
  _parent = parent;
  if(parent)
    _parent->Register(this);

  _name = name;
}

WiredDocument::~WiredDocument()
{
  if(_parent)
    _parent->Unregister(this);
}

void		WiredDocument::Register(WiredDocument *child)
{
  _children.Add(child);
}

void		WiredDocument::Unregister(WiredDocument *child)
{
  _children.Remove(child);
}

void		WiredDocument::ChangeParent(WiredDocument *newdad)
{
  if (_parent)
    _parent->Unregister(this);
  if (newdad)
    {
      _parent = newdad;
      newdad->Register(this);
    }
}

WiredDocumentArray	WiredDocument::getChildren()
{
  return _children;
}

void		WiredDocument::SaveMe()
{
  clearDocData();

  Save();
}

bool		WiredDocument::saveDocData(SaveElement *data, wxString file)
{
  SaveElementsHashMap::iterator	it;

#ifdef __DEBUG__
  if(file.Cmp(WIRED_PROJECT_FILE))
      cout << ">>> writing file :'" << file.mb_str() << "'" << endl;
#endif
  if(!_dataSave.count(file))
    _dataSave[file] = new SaveElementArray();

  //Should check if key already exists and delete it before overwriting it.
  _dataSave[file]->Add(data);
  return true;
}

void		WiredDocument::clearDocData()
{
  SaveElementsHashMap::iterator	dataSaveIt;
  
  dataSaveIt = _dataSave.begin();
  while(!_dataSave.empty())
    {
      if(dataSaveIt->second)
	rmDocDataFile(dataSaveIt->first);
      dataSaveIt++;
    }
 
  //to be sure...
  _dataSave.clear();
}

void		WiredDocument::rmDocDataFile(wxString file)
{
   int	i;
   
   if(_dataSave.find(file) != _dataSave.end())
     while(!_dataSave[file]->IsEmpty())
       {
	 if(_dataSave[file]->Item(0))
	   delete (_dataSave[file]->Item(0));
	 
	 _dataSave[file]->RemoveAt(0);
       }
   
   _dataSave.erase(file);

}

SaveElementsHashMap	WiredDocument::getDocData()
{
  return _dataSave;
}

SaveElementArray	*WiredDocument::getDocFile(wxString file)
{
  return _dataSave[file];
}

SaveElementArray	WiredDocument::AskData(wxString filename, bool local)
{
  if (local)
    return saveCenter->LoadLocalFile(filename);
  else
    return saveCenter->LoadProjectFile(filename);
}

void			WiredDocument::SavePatch(wxString file, wxString path)
{
  saveCenter->SaveFile(this, file, path);
}

void			WiredDocument::SavePatch(wxFileName filename)
{
  SavePatch(filename.GetName(), filename.GetPath());
}
