// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991
#include "WiredDocument.h"

//Only used for debug
#include <iostream>

#include "SaveCenter.h"

extern SaveCenter	*saveCenter;

int WiredDocument::id = 0;

WiredDocument::WiredDocument(wxString name, WiredDocument *parent, bool noParent)
{
  increaseId();
  _id = id;

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

void		WiredDocument::saveDocData(SaveElement *data, wxString file)
{
  SaveElementsHashMap::iterator	it;

  if(!_dataSave.count(file))
      _dataSave[file] = new SaveElementArray();

  //Should check if key already exists and delete it before overwriting it.
  _dataSave[file]->Add(data);

}

void		WiredDocument::clearDocData()
{
  SaveElementsHashMap::iterator	dataSaveIt;

  while(!_dataSave.empty())
    {
      dataSaveIt = _dataSave.begin();
      if(dataSaveIt->second)
	rmDocDataFile(dataSaveIt->first);
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

SaveElementArray	WiredDocument::AskData(wxString filename)
{
  return saveCenter->LoadFile(filename);
}

void			WiredDocument::SavePatch(wxString file, wxString path)
{
  saveCenter->SaveFile(this, file, path);
}
