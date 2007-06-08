// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991
#ifndef _SAVEELEMENT_H_
#define _SAVEELEMENT_H_

#include <wx/string.h>
#include <wx/hashmap.h>
#include <wx/dynarray.h>

class SaveElement;

WX_DEFINE_ARRAY_PTR(SaveElement*, SaveElementArray);
WX_DECLARE_STRING_HASH_MAP(SaveElementArray*, SaveElementsHashMap);
WX_DECLARE_STRING_HASH_MAP(wxString, AttributesHashMap);

class SaveElement
{
 public:
  /** Default constructor. Does Nothing. */
  inline SaveElement() {  }

  /** Constructor with key/value initialization.
   * Used to clean up the code.
   * \param key The key.
   * \param value The value.
   */
  inline SaveElement(wxString key, wxString value)
    { setPair(key, value); }

  inline SaveElement(wxString key, int value)
    { setPair(key, value); }

  inline SaveElement(wxString key, float value)
    { setPair(key, value); }

  inline SaveElement(wxString key, double value)
    { setPair(key, value); }

    /** Copy constructor.
    * Duplicates the copy SaveElement into the new one.
    * \param copy The SaveElement to be copied.
    */
    inline SaveElement(SaveElement &copy)
        : _key(copy.getKey()), _value(copy.getValue()), _attributes(copy.getAttributes())
    {
        SaveElementArray children = copy.getChildren();
        for(unsigned int i = 0; i < children.GetCount(); i++)
            addChildren(new SaveElement(*(children.Item(i))));
    }

    /** Destructor. */
    inline ~SaveElement(){};

  //accessors
  /** Key setter.
   * \param key The new key.
   */
  inline void		setKey(wxString key) { _key = key; }

  /** Value setter.
   * \param value The new value.
   */
  inline void		setValue(wxString value)
    { _value = value; }

  inline void		setValue(int value)
    { wxString s; s << value; setValue(s); }

  inline void		setValue(float value)
    { wxString s; s << value; setValue(s); }

  inline void		setValue(double value)
    { wxString s; s << value; setValue(s); }

  /** Pair setter.
   * \param key The new key.
   * \param value The new value.
   */
  inline void		setPair(wxString key, wxString value)
    { setKey(key); setValue(value); }

  inline void		setPair(wxString key, int  value)
    { setKey(key); setValue(value); }

  inline void		setPair(wxString key, float value)
    { setKey(key); setValue(value); }

  inline void		setPair(wxString key, double value)
    { setKey(key); setValue(value); }

  /** Adds an attribute.
   * \param key the key of the attribute.
   * \param value the value of the attribute.
   */
  inline void		addAttribute(wxString key, wxString value)
    { _attributes[key] = value; }

  inline void		addAttribute(wxString key, int value)
    { wxString s; s << value; _attributes[key] = s; }

  inline void		addAttribute(wxString key, float value)
    { wxString s; s << value; _attributes[key] = s; }

  inline void		addAttribute(wxString key, double value)
    { wxString s; s << value; _attributes[key] = s; }

  /** Adds a child.
   * \param children The SaveElement to be added as a child.
   */
  inline void		addChildren(SaveElement *children)
    { _children.Add(children); }

  /** Returns if the SaveElement has children
   * \return true if there are any, else false.
   */
  inline bool		hasChildren()
    { return ( _children.GetCount() > 0 ); }

  /** Key getter.
   * \return The key of the SaveElement.
   */
  inline wxString	getKey() { return _key; }

  /** Value getter.
   * \return The value of the SaveElement.
   */
  inline wxString	getValue() { return _value; }

  inline int		getValueInt()
    { long ret; _value.ToLong(&ret); return (int)ret; }

  inline float		getValueFloat()
    { double ret; _value.ToDouble(&ret); return (float)ret; }

  inline double		getValueDouble()
    { double ret; _value.ToDouble(&ret); return ret; }

  /** Children getter.
   * \return The clhildren of the SaveElement.
   */
  inline SaveElementArray	getChildren() { return _children; }

  /** Attribute getter.
   * \param key The key of the attribute to get.
   * \return The value of the attribute designated by key.
   */
  inline wxString	getAttribute(wxString key)
    {
      if (_attributes.find(key) != _attributes.end())
	return _attributes[key];
      else
	return wxT("");
    }

  inline int		getAttributeInt(wxString key)
    {
      if (_attributes.find(key) != _attributes.end())
	{ long ret; _attributes[key].ToLong(&ret); return (int)ret; }
      else
	return 0;
    }

  inline float		getAttributeFloat(wxString key)
    {
      if (_attributes.find(key) != _attributes.end())
	{ double ret; _attributes[key].ToDouble(&ret); return (float)ret; }
      else
	return 0.f;
    }

  inline double		getAttributeDouble(wxString key)
    {
      if (_attributes.find(key) != _attributes.end())
	{ double ret; _attributes[key].ToDouble(&ret); return ret; }
      else
	return 0.f;
    }

  /** Returns if an attribute with such key exists.
   * \param The key to test.
   * \return true if it exists, else false.
   */
  inline bool		attributeExists(wxString key)
    {  return ( _attributes.find(key) != _attributes.end() ); }

  /** Erase all attributes. */
  inline void		clearAttributes() { _attributes.clear(); }
  /** Clears the whole SaveElement. */
  inline void		clear()
    { _key.Clear(); _value.Clear(); clearAttributes(); _children.clear(); }

  /** Attributes getter.
   * \return The whole attribute HashMap.
   */
  inline AttributesHashMap	getAttributes() { return _attributes; }

 private:
  wxString			_key;
  wxString			_value;
  AttributesHashMap		_attributes;
  SaveElementArray		_children;
};

#endif /*_SAVEELEMENT_H_ */
