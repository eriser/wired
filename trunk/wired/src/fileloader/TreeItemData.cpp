// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#include "TreeItemData.h"

TreeItemData::TreeItemData(wxString _path) : wxTreeItemData()
{
	path = _path;
}

TreeItemData::~TreeItemData()
{
}

wxString TreeItemData::GetPath()
{
	return path;
}
