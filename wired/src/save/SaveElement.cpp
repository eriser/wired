// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991
#include "SaveElement.h"

SaveElement::SaveElement(WiredObject *Object)
{
  _object = Object;
}

SaveElement::SaveElement(WiredObject *Object, wxString Key, wxString Value)
{
  _object = Object;
  _key = key;
  _value = value;
}

void		SaveElement::setKey(wxString key)
{
  _key = key;
}

void		SaveElement::setValue(wxString value)
{
  _value = value;
}

void		SaveElement::setPair(wxString key, wxString value)
{
  _key = key;
  _value = value;
}

void		SaveElement::addAttribute(wxString key, wxString value)
{
  _attributes[key] = value;
}

wxString	SaveElement::getKey()
{
  return _key;
}

wxString	SaveElement::getValue()
{
  return _value;
}

wxString	SaveElement::getAttribute(wxString key)
{
  if (_attributes.find(key) != _attributes.end())
    return _attributes[key];
  else
    return wxT("");
}

bool		SaveElement::attributeExists(wxString key)
{
  return ( _attributes[key] != _attributes.end() );
}

void		SaveElement::clearAttributes()
{
  _attributes.clear();
}

AttributesHashMap	SaveElement::getAttributes();
{
  return _attributes;
}
