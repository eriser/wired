// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef _SAVECENTER_H_
#define _SAVECENTER_H_

#include "WiredDocument.h"
#include "../xml/WiredXml.h"

//For the DirDialog
#include <wx/dirdlg.h>
#include <wx/filename.h>

//wxArrayString
#include <wx/arrstr.h>

#define WIRED_DEFAULT_PROJECT_NAME wxT("WiredProject")

//define standard XML tags for the project file
#define WIRED_TAG_WIREDDOC wxT("wiredDoc")

typedef struct
{
  wxString		name;
  int			id;
  SaveElementArray	data;
} loadedDocument;

WX_DEFINE_SORTED_ARRAY(loadedDocument *, LoadedDocumentArray);
WX_DEFINE_ARRAY_PTR(SaveElementArray*, SaveElementArrayArray);

WX_DECLARE_STRING_HASH_MAP(SaveElementArrayArray*, SaveElementArrayHashMap);
WX_DECLARE_STRING_HASH_MAP(WiredDocumentArray*, WiredDocumentArrayHashMap);

class SaveCenter : public WiredDocument
{
 public:
  /** Constructor. */
  SaveCenter();

  /** Destructor. */
  ~SaveCenter();

  /** Main Save function, implementation of WiredDocument.
   * The SaveCenter is a WiredDocument. This function is used to save 
   * projectwide infos
   */
  void		Save();
  
  /** Main Load function, implementation of WiredDocument.
   * The SaveCenter is a WiredDocument. This function is used to load 
   * projectwide infos
   */
  void		Load(SaveElementArray data);
  
  /** Cleans the WiredDocument tree.
   * This method is used to restore the initial state of the tree before a load.
   * It calls the DeleteChildren() method of every WiredDocument.
   */
  void		CleanTree();

  /** Returns the project path. */
  wxFileName	getProjectPath();

  /** Sets the project path.
   * As soon as the project path is set, every needed folder is created.
   * \param projectPath the new project path.
   */
  void		setProjectPath(wxFileName projectPath);

  /** Returns the project name. */
  wxString	getProjectName();

  /** Returns the audio directory.
   * Check the _audio member for more explanations.
   * \return The path to the audio directory.
   */
  wxString	getAudioDir();

  /** Sets the audio directory.
   * Check the _audio member for more explanations.
   * \param audioDir The new path to the audio directory.
   */
  void		setAudioDir(wxString audioDir);

  /** Sets the project name
   * \param projectName the new project name.
   */
  void		setProjectName(wxString projectName);
  
  /** Saves the whole project.
   * This function should only be called when clicking on the save menu...
   * Initialize some elements and calls SaveDocument on the SaveCenter.
   * Make sure the project path and name are set.
   */
  void		SaveProject();
  
  /** Only saves the file designated by file of the WiredDocument designated by doc.
   * This method should be used, for example, to save a plugin patch.<br>
   * It only calls Save() on the WiredDocument and then writes the file with 
   * WriteFile. As it allows to write a file that will be stored independently from
   * the project, there is also a filename to specify a path. This path HAS TO be 
   * absolute or it will be made relative to the project root.
   * If no path is specified, the file will be saved as if it was a part of the 
   * project.
   * \param doc The WiredDocument containing the file we want to save.
   * \param file The name of the file we want to save
   * \param path The path to the file to write.
   */
  void		SaveFile(WiredDocument *doc, wxString file, wxString path = wxT(""));

  /** Main function to call when loading a saved project.
   * It loads the project designated by getProjectPath().
   */
  void			LoadProject();

  /** Gets data from an external file.
   * \param filename the path to the external file.
   * \return The data contained in the external file.
   */
  SaveElementArray	LoadFile(wxString filename);

  /** Dumps the whole WiredDocument objects tree. */
  void			DumpWiredDocumentTree();
  
  /** Dumps the name of a WiredDocument object and calls itself 
   * recursively over its children. 
   * \param currentNode The current WiredDocument.
   * \param depth The current depth, will be the number of spaces for indentation.
   */
  void			DumpWiredDocumentSubTree(WiredDocument *currentNode,
						 int depth);

  /** Checks if the path is the path to a valid project.
   * A path is a valid project path if it contains a wired.xml file.
   * TODO : Add deeper checks.
   * \param path The path to the folder to check.
   */
  bool			IsProject(wxFileName path);

 private:
   /** Writes an element in the xmlfile.
    * \param elem the element to write.
    * \param xmlFile the WiredXml object to write in.
    */
  void		WriteElement(SaveElement *elem, WiredXml *xmlFile);

  /** Saves a WiredDocument.
   * This function calls itself recursively, so the whole WiredDocument tree is
   * parsed.
   * If the confFile is NULL, no project file will be written (only data...)
   * \param doc the 
   */ 
  void		SaveDocument(WiredDocument *doc, WiredXml *xmlFile);

  /** Writes a SaveElementArray in a separate file.
   * The filename is relative to the project root path.
   * \param filename the path to the file to write.
   * \param elements the SaveElementArray to write.
   */
  void		WriteFile(wxString filename, SaveElementArray *elements);


  /** Adds the references to external files in the project file.
   * \param saveElements the SaveElements HashMap of the current WiredDocument.
   * \param xmlFile the WiredXml object to write in.
   */
  void		AddReferences(SaveElementsHashMap &saveElements, 
			      WiredXml *xmlFile);

  /** Returns an unused project name, based on the content of the current working
   * directory.
   * \param cwd The currentWorkingDirectory.
   * \return A project name.
   */
  wxString	GetDefaultProjectName(wxFileName cwd);

  /** Returns the project name, based on the project path.
   * The project name is the name of the last directory of the path.
   * \param path The path to the current working directory.
   * \return The project name.
   */
  wxString	GetProjectNameFromProjectPath(wxFileName path);

  /** Returns a path to an xml file, relative to the project root, 
   * based on a wxString.
   * \param tag The wxString to build the path from.
   * \return The relative path.
   */
  wxFileName	getPathFromRelativeTag(wxString tag);

  /** Returns true if the project has been saved at least once, else false */
  bool		getSaved();

  /** Sets the saved state of the session. */
  void		setSaved(bool saved = true);

  /** Removes the files in the project folder.
   * Should be used only if the project hasn't been saved.
   */
  void		CleanProject();

 private:

  /** Dumps a SaveElementArrayHashMap.
   * As this structure is quite complicated, this function is mainly used for debug.
   * \param dataLoaded The SaveElementArrayHashMap to dump.
   */
  void			DumpSaveElementArrayHashMap(SaveElementArrayHashMap dataLoaded);

  /** Dumps a WiredDocumentArrayHashMap.
   * As this structure is quite complicated, this function is mainly used for debug.
   * \param toProcess The WireDocumentArrayHashMap to dump.
   */
  void			DumpWiredDocumentArrayHashMap(WiredDocumentArrayHashMap toProcess);

  /** Redistributes a SaveElementArrayHashMap to the right WiredDocuments.
   * This method is the second part of the project loading.
   * \param dataLoaded The data to redistribute.
   */
  void			RedistributeHash(LoadedDocumentArray dataLoaded);

  /** The path to the project. */
  wxFileName		_projectPath;

  /** The name of the project. 
   * It is only stored to avoid recalculating it each time.*/
  wxString		_projectName;

  /** The path to the audio directory. 
   * This one would rather be in the settings. It is here to smooth 
   * the suppression of WiredSessionXml.
   */ 
  wxFileName		_audioDir;

  /** Stores if the project has been saved at least once. */
  bool			_saved;

};

extern SaveCenter*	saveCenter;

/** Used to sort the LoadedDocumentArray
 * \param doc1 the first document.
 * \param doc2 the second document.
 * \return 1 if doc1>doc2, -1 if doc1<doc2, 0 else
 */
int		SortDataLoaded(loadedDocument *doc1, loadedDocument *doc2);


#endif /*_SAVECENTER_H_ */
