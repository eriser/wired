// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef _WIREDDOCUMENT_H_
#define _WIREDDOCUMENT_H_

#include <wx/dynarray.h>

#include "SaveElement.h"

class WiredDocument;

WX_DEFINE_ARRAY_PTR(WiredDocument *, WiredDocumentArray);

#define WIRED_PROJECT_FILE wxT("WiredProjectFile")

class WiredDocument
{
 public:
  /** Constructor.
   * \param docName The name of the document. It should be unique for an object
   * (not for an instance of course...).
   * \param parent The parent WiredDocument. If left to NULL, it will be defaulted 
   * to the SaveCenter.
   * \param noParent For specific use only and should usually not be set to true. Else,
   * the WiredDocument will not be saved with the session. Is used by the SaveCenter
   * and could be used for the settings for example.
   */
  WiredDocument(wxString docName, WiredDocument *parent = NULL,
		bool noParent = false);

  /** Destructor.
   * Removes this from the parent children.
   */
  ~WiredDocument();

  /** Main save function.
   * This function will be called by the SaveCenter when a save of the document
   * is asked.
   * It will only fill the _dataSave array, which will be caught later by getDocData.
   * When implementing a new WiredDocument, you just have to worry about filling the 
   * _dataSave structure.
   * \see Load
   */
  virtual void	Save() = 0;

  /** Main Load function.
   * It will be called by the SaveCenter when a load is asked.<br>
   * You only get with this method the data stored in the project file and 
   * SaveElements containing references to the other xml files.
   * \param data The data stored in the project file.
   */
  virtual void	Load(SaveElementArray data) = 0;

  /** Cleaning function.
   * It will be called before a Load to initialize the WiredDocument with
   * no unneeded children.
   * Every children created during a session should be deleted by this method.
   * In other words, it should restore the state of the direct subtree to what
   * it was at the first instanciation of the WiredDocument.
   * Default values should not be handled by this method, a Load() will occur
   * with them soon after the call to DeleteChildren().
   */
  virtual void	CleanChildren() {}

  /** Properly calls save. */
  void		SaveMe();

  /** Returns the children of the WiredDocument.
   * \return The children array.
   */
  WiredDocumentArray		getChildren();

  /** Adds a child to the _children array.
   * \param children The child to add.
   */
  void				Register(WiredDocument *children);

  /** Removes a child from the _children array.
   * \param child The child to remove.
   */
  void				Unregister(WiredDocument *child);

  void				ChangeParent(WiredDocument *newdad);
  
  /** Returns the data stored in _dataSave.
   * \return The data stored in _dataSave.
   */
  SaveElementsHashMap		getDocData();
  
  /** Returns the data corresponding to a specific entry of the _dataSave hash map.
   * \param the key of the hash map.
   * \return the data stored for this file.
   */
  SaveElementArray		*getDocFile(wxString file);

  /** Returns the name of the WiredDocument.
   * \return The name of the WiredDocument.
   */
  inline wxString		getName() { return _name; }

  /** Returns the id of the WiredDocument.
   * \return the id of the WiredDocument.
   */
  inline int			getId() { return _id; }


  /** The unique id of the WiredDocument. This id is
   * incremented on each new WiredDocument, therefore
   * permitting during the load to recreate the whole 
   * tree in the same order it was created by the user.
   */
  static int		id;

  /** Returns the total number of WiredDocument.
   * \return the total number of WiredDocument.
   */
  static int		getTotalId(){ return id; }

  /** Increases the id. */
  static void		increaseId() { id++; }

 protected:

  /** Saves an external file only.
   * When implementing a WiredDocument, this is handy if you want the SaveCenter
   * to write an external file without saving the whole project.<br>
   * In this state of mind, it also allows to choose an arbitrary filename.
   * \param file The key in the HashMap of the file to save.
   * \param path The path to use when writing the file. If none is specified, 
   * it will be written in the project folder.
   */

  void				SavePatch(wxString file, wxString path);


  /** Gets data from an external xml file.
   * When implementing a WiredDocument, you can the path to those files in the
   * "reference" SaveElements passed to Load().
   * \param filename the path to the external file.
   * \return The data contained in the external file.
   */
  SaveElementArray		AskData(wxString filename);

  /** Adds data that will be saved.
   * When implementing a WiredDocument, one has to instanciate on SaveElement per call
   * to this function, but does not have to worry about deleting it.
   * \param file The relative path to the xml file in which the data will be written.
   * For readability, one can omit the .xml extension. If file is different from
   * WIRED_PROJECT_FILE, the data will be written in an external file.
   * \param data The SaveElement to write.
  */
  void				saveDocData(SaveElement *data, wxString file = WIRED_PROJECT_FILE);
  
  /** Removes all the data previously stored. */
  void				clearDocData();

  /** Removes one element of the HashMap. 
   * \param file The key of the element to erase.
   */
  void				rmDocDataFile(wxString file);

 private:

  /** A pointer to the parent. */
  WiredDocument			*_parent;

  /** The list of the children of this WiredDocument. */
  WiredDocumentArray		_children;

  /** The name of the WiredDocument. */
  wxString			_name;
  
  /** The data to be written by the SaveCenter. */ 
  SaveElementsHashMap		_dataSave;

  /** The WiredDocument's instance of its id. */
  int		_id;
};

#endif /*_WIREDDOCUMENT_H */
