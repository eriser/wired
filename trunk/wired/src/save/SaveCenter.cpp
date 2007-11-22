// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <iostream>
#include "SaveCenter.h"
#include <wx/dir.h>
#include "Settings.h"

SaveCenter		*saveCenter = NULL;
extern	Settings	*WiredSettings;
using namespace std;

SaveCenter::SaveCenter()
  : WiredDocument(wxT("savecenter"), NULL, true)
{
  _id_mark = 0;
  setSaved(false);
}

SaveCenter::~SaveCenter()
{
  if(!getSaved())
    CleanProject();
  std::cerr << "SaveCenter destroyed" << std::endl;
}

//Implemetation of WiredDocument
void	SaveCenter::Save()
{
  //return project specific infos ?
}

void	SaveCenter::Load(SaveElementArray dataLoaded)
{
  //load project specific infos ?
}

long	SaveCenter::RegisterId(long id)
{
  if(this)
  {
    if(id > _id_mark)
      _id_mark = id;
    else
      _id_mark++;
    return (_id_mark);
  }
  else
    return (id);
}

void	SaveCenter::CleanTree()
{
  WiredDocumentArray		toProcess;
  WiredDocumentArray		toMergeInToProcess;
  int				toProcessIt = 0;

  //delete the old project if it has not been saved.
  if(!getSaved())
    CleanProject();

  setSaved(false);

  //Initialization of the WiredDocuments to be processed.
  toProcess = this->getChildren();

  while(toProcessIt != toProcess.GetCount())
    {
#ifdef __DEBUG__
      std::cout << "toProcessIt = " << toProcessIt << std::endl;
      std::cout << "toProcess.GetCount() = " << toProcess.GetCount() << std::endl;
      std::cout << "toProcess[toProcessIt]->GetName() = " << toProcess[toProcessIt]->getName().mb_str() << std::endl;
#endif

      //Clean the WiredDocument
      toProcess[toProcessIt]->CleanChildren();
      //Retrieve its remaining children
      toMergeInToProcess = toProcess[toProcessIt]->getChildren();
      //Merge them into the toProcessArray
      WX_APPEND_ARRAY(toProcess, toMergeInToProcess);

      toProcessIt++;
    }

}

void	SaveCenter::SaveProject()
{
  wxString	fileName;
  WiredXml	*xmlFile = new WiredXml();

  if(!_projectPath.DirExists())
    _projectPath.Mkdir();

  fileName << _projectPath.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME) << wxT("wired.xml");

  xmlFile->CreateDocument(fileName);

  SaveDocument((WiredDocument *)this, xmlFile);

  xmlFile->EndDocumentWriter();
  delete xmlFile;

  //the current project is now a saved one
  setSaved();
}

void	SaveCenter::SaveOneDocument(WiredDocument *doc, wxString file)
{
  WiredXml	*xmlFile = new WiredXml();
  xmlFile->CreateDocument(file);
  SaveDocument(doc, xmlFile);
}

void	SaveCenter::SaveFile(WiredDocument *doc, wxString file, wxString path)
{
  wxFileName	normalizedPath;

  if (path == wxT(""))
    path = file;
  else
    {
      normalizedPath.Assign(path);
      normalizedPath.Normalize();
      path = normalizedPath.GetFullPath();
    }
  if (doc->getDocFile(file) == NULL)
    std::cerr << "[SaveCenter] trying to save a key not found... (file == '" << file.mb_str() << "')" << std::endl;

  WriteFile(path + file, doc->getDocFile(file));
}

void	SaveCenter::SaveDocument(WiredDocument *currentNode, WiredXml *xmlFile)
{
  WiredDocumentArray			childrenOfCurrentNode;
  int					i;
  SaveElementsHashMap			saveElements;
  SaveElementArray			*toWrite;
  SaveElementsHashMap::iterator		saveElementsIt;
  wxString				tmp;

  //Get our children
  childrenOfCurrentNode = currentNode->getChildren();

  //Save Document
  currentNode->SaveMe();

  //get my SaveElements
  saveElements = currentNode->getDocData();

  //write our SaveElements...
  //...start with our name...
  xmlFile->StartElement(WIRED_TAG_WIREDDOC);
  xmlFile->WriteAttribute(wxT("name"), currentNode->getName());
  tmp.Clear();
  tmp << currentNode->getId();
  xmlFile->WriteAttribute(wxT("id"), tmp);

  //Write references
  AddReferences(saveElements, xmlFile);

  //get elements to write in the conf file
  if(saveElements.count(WIRED_PROJECT_FILE))
    {
      toWrite = saveElements[WIRED_PROJECT_FILE];

      //...then write the elements
      for (i = 0; i < toWrite->GetCount(); i++)
	WriteElement(toWrite->Item(i), xmlFile);
    }
  //Write the other files
  for (saveElementsIt = saveElements.begin();
       saveElementsIt != saveElements.end();
       saveElementsIt++)
    if(saveElementsIt->first != WIRED_PROJECT_FILE)
      WriteFile(saveElementsIt->first, saveElementsIt->second);

  //call recursively on our children
  for (i = 0; i < childrenOfCurrentNode.GetCount(); i++)
    SaveDocument(childrenOfCurrentNode[i], xmlFile);

  //...finish by closing things
  xmlFile->EndElement();
}

void	SaveCenter::AddReferences(SaveElementsHashMap &saveElements,
				  WiredXml *xmlFile)
{
  SaveElementsHashMap::iterator	saveElementsIt;
  SaveElement			*ref = new SaveElement();
  wxFileName			absolutePath;
  //wxFileName			relativePath;

  //wxArrayString			dirs;

  //for each entry of the hash map....
  for (saveElementsIt = saveElements.begin();
	  saveElementsIt != saveElements.end();
	  saveElementsIt++)
	//if it is not the one to write in the project file
	if(saveElementsIt->first != WIRED_PROJECT_FILE)
	{
	  //fill a SaveElement
	  ref->clear();

/*
 *	// crap follows...
 *      //get the path to the file we will create later...
 *      absolutePath.Assign(getProjectPath());
 *
 *      relativePath.Assign(getPathFromRelativeTag(saveElementsIt->first));
 *      dirs = relativePath.GetDirs();
 *
 *      for(int j = 0; j < dirs.GetCount(); j++)
 *        absolutePath.AppendDir(dirs[j]);
 *
 *      //absolutePath.SetName(relativePath.GetName());
 *      //absolutePath.SetExt(relativePath.GetExt());
 *      absolutePath.Assign(relativePath.GetFullPath());
 */

	  absolutePath.Assign(getPathFromRelativeTag(saveElementsIt->first));
	  absolutePath.MakeRelativeTo(getProjectPath().GetPath());

	  cout << "[SAVECENTER] AddReferences() : adding reference to '" << absolutePath.GetFullPath().mb_str() << "'" << endl;
	  ref->setPair(wxT("reference"), absolutePath.GetFullPath());
	  //and write it.
	  WriteElement(ref, xmlFile);
	}

  delete ref;
}

void	SaveCenter::WriteElement(SaveElement *elem, WiredXml *xmlFile)
{
  int				i;
  AttributesHashMap		attributes;
  AttributesHashMap::iterator	attributesIt;
  SaveElementArray		children;

  attributes = elem->getAttributes();

  xmlFile->StartElement(elem->getKey());

  for(attributesIt = attributes.begin();
      attributesIt != attributes.end();
      attributesIt++)
    {
      xmlFile->WriteAttribute(attributesIt->first, attributesIt->second, true);
    }
  xmlFile->WriteString(elem->getValue());

  children = elem->getChildren();

  for(int j = 0; j < children.GetCount(); j++)
      WriteElement(children.Item(j), xmlFile);
  xmlFile->EndElement();

}

wxFileName	SaveCenter::getPathFromRelativeTag(wxString tag)
{
  wxFileName	ret;

  /*
   *while(tag.Find('/') != -1)
   *  {
   *    ret.AppendDir(tag.BeforeFirst('/'));
   *    tag = tag.AfterFirst('/');
   *  }
   *tag = tag.BeforeFirst('.');
   *ret.SetName(tag);
   *ret.SetExt(wxT("xml"));
   */

  if (tag.Right(4) != wxString(wxT(".xml")))
  {
	tag += wxT(".xml");
  }
  ret.Assign(tag);
  if (!ret.IsOk())
	cerr << "Problem saving '" << tag.mb_str() << "'" << endl;
  return ret;
}

void		SaveCenter::WriteFile(wxString givenFileName,
				      SaveElementArray *elements)
{
  wxFileName	filename;
  wxFileName	relativePath;
  wxArrayString	dirs;
  wxString	rootTag;

  WiredXml	*xmlFile = new WiredXml();
  int		i;

  if (!wxFileName::DirExists(givenFileName) &&
 // EndsWith was created in >= 2.8 and
 // wxCHECK_VERSION test if we're >= 2.8.0
#if wxCHECK_VERSION(2, 8, 0)
      !givenFileName.EndsWith(wxT("/"))     &&
      !givenFileName.EndsWith(wxT("\\")))
#else
      givenFileName.Right(1) != wxString(wxT("/")) &&
      givenFileName.Right(1) != wxString(wxT("\\")))
#endif
      filename.Assign(givenFileName);
  else
    {
      filename.Assign(getProjectPath());
      relativePath = getPathFromRelativeTag(givenFileName);
      dirs = relativePath.GetDirs();

      for(int j = 0; j < dirs.GetCount(); j++)
	filename.AppendDir(dirs[j]);

      filename.SetName(relativePath.GetName());
      filename.SetExt(relativePath.GetExt());

      filename.MakeAbsolute();
    }
  std::cerr << "[SaveCenter] filename to write : " << filename.GetFullPath().mb_str() << std::endl;

  if(!wxFileName::DirExists(filename.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME)))
    wxFileName::Mkdir(filename.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME),
		      0777, wxPATH_MKDIR_FULL);

  xmlFile->CreateDocument(filename.GetFullPath());

  rootTag = filename.GetName();

  xmlFile->StartElement(rootTag);

  if (elements)
    for (i = 0; i < elements->GetCount(); i++)
      WriteElement(elements->Item(i), xmlFile);
  else
    std::cerr << "[SaveCenter] No elements to write !" << std::endl;

  xmlFile->EndElement();
  delete xmlFile;
}

//Accessors
wxFileName	SaveCenter::getProjectPath()
{
  if (this == NULL)
    return (wxGetCwd());
  return (_projectPath);
}

wxString	SaveCenter::getAudioDir()
{
  return _audioDir.GetLongPath();
}

void		SaveCenter::setAudioDir(wxString audioDir)
{
  _audioDir = audioDir;
}

void		SaveCenter::setProjectPath(wxFileName projectPath)
{
  if(!projectPath.IsOk())
    {
      _projectPath.AssignDir(wxGetCwd());
      _projectName = GetDefaultProjectName(projectPath);
      _projectPath.AppendDir(_projectName);
    }
  else
    _projectPath.AssignDir(projectPath.GetFullPath());

  _projectName = GetProjectNameFromProjectPath(_projectPath);

  _audioDir = _projectPath;
  _audioDir.AppendDir(wxT("audio"));

  if(!_projectPath.DirExists())
    _projectPath.Mkdir();

  if(!_audioDir.DirExists())
    _audioDir.Mkdir();
}

wxString	SaveCenter::getProjectName()
{
  return _projectName;
}

void		SaveCenter::setProjectName(wxString projectName)
{
  _projectName = projectName;

  _projectPath.RemoveLastDir();
  _projectPath.AppendDir(_projectName);
}

//This method really looks like LoadProject... maybe we could do something...
SaveElementArray	SaveCenter::LoadLocalFile(wxString filename)
{
  SaveElementArray	ret;
  wxString		path;
  wxFileName		absoluteFilename;

  absoluteFilename.Assign(filename);
  absoluteFilename.Normalize(wxPATH_NORM_ALL, WiredSettings->HomeDir);

  return LoadFile(absoluteFilename.GetFullPath(), true, absoluteFilename.GetName());
}
SaveElementArray	SaveCenter::LoadProjectFile(wxString filename)
{
  return LoadFile(filename, false);
}
SaveElementArray	SaveCenter::LoadFile(wxString filename, bool fullpath, wxString rootTag)
{
  WiredXml		*xmlFile = new WiredXml();
  //wxString		rootTag;
  SaveElementArray	ret;
  SaveElementArray	history;
  wxString		s;

  wxFileName		absoluteFilename, relativeFilename;
  wxArrayString		dirs;

  wxString		nodeName;
  int			nodeType;

  SaveElement		*currSaveElem;


  if (fullpath)
  {
    if(!xmlFile->OpenDocument(filename))
      cerr << "LoadFile:could not load file:" << absoluteFilename.GetFullPath().mb_str() << ":" << endl;
  }
  else
  {
    absoluteFilename.Assign(filename);
    absoluteFilename.Normalize(wxPATH_NORM_ALL, getProjectPath().GetFullPath());

    if(!xmlFile->OpenDocument(absoluteFilename.GetFullPath()))
      cerr << "LoadFile:could not load file:" << absoluteFilename.GetFullPath().mb_str() << ":" << endl;

    rootTag = absoluteFilename.GetName();
  }

  //data management
  while(xmlFile->Read())
    {
      nodeType = xmlFile->GetNodeType();
      if(nodeType == XML_READER_TYPE_ELEMENT)
	{
	  nodeName = xmlFile->GetNodeName();
	  if(nodeName != rootTag)
	    {
	      currSaveElem = new SaveElement();

	      if(xmlFile->GetDepth() <= 1)
		ret.Add(currSaveElem);
	      else
		history.Last()->addChildren(currSaveElem);
	      currSaveElem->setKey(nodeName);

	      //attributes handling
	      for(int i = 0; i < xmlFile->GetAttributeCount(); i++)
		currSaveElem->addAttribute(xmlFile->GetAttributeName(i),
					   xmlFile->GetAttributeValue(i));

	      history.Add(currSaveElem);
	    }
	}
      else if(nodeType == XML_READER_TYPE_TEXT)
	{
	  currSaveElem->setValue(xmlFile->GetNodeValue());
	}
      else if(nodeType == XML_READER_TYPE_END_ELEMENT)
	{
	  if(history.GetCount() > 0)
	    history.Remove(history.Last());
	}
    }
  return ret;
}


void	SaveCenter::LoadProject()
{
  std::cout << "[SaveCenter] LoadProject" << std::endl;

  //infos about the node we are reading
  WiredXml		*xmlFile = new WiredXml();
  wxString		filename;
  wxString		nodeName;
  int			nodeType;
  SaveElement		*currSaveElem = NULL;
  loadedDocument	*currDoc = NULL;
  long			tmp;

  //depth of the last wiredDocument
  int		lastWiredDocDepth;

  //infos about the last SaveElement
  SaveElement	*previousSaveElem = NULL;

  //path of SaveElements to the current one
  SaveElementArray	pathToCurrSaveElem;

  //The whole array of data loaded
  LoadedDocumentArray	dataLoaded(&SortDataLoaded);

  //We start by cleaning the WiredDocument tree to restore its initial state.
  //  CleanTree();

  //open the file
  filename.Clear();
  filename << getProjectPath().GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
  filename << wxT("wired.xml");
  xmlFile->OpenDocument(filename);

  while (xmlFile->Read())
    {
      nodeType = xmlFile->GetNodeType();

      //If it is an opening tag
      if (nodeType == XML_READER_TYPE_ELEMENT)
	{
	  nodeName = xmlFile->GetNodeName();
	  //if it is a WiredDoc
	  if (nodeName == WIRED_TAG_WIREDDOC)
	    {
	      //we store the depth
	      lastWiredDocDepth = xmlFile->GetDepth();

	      //we init a loadedDocument struct
	      currDoc = new loadedDocument();
	      currDoc->name = xmlFile->GetAttribute(wxT("name"));
	      wxString(xmlFile->GetAttribute(wxT("id"))).ToLong(&tmp);
	      currDoc->id = tmp;
	      //and add it to the dataLoaded
	      dataLoaded.Add(currDoc);
	    }
	  else
	    {
	      //we keep in a safe place the previous SaveElement
	      previousSaveElem = currSaveElem;

	      //we create a SaveElement to store the tag.
	      currSaveElem = new SaveElement();

	      //we fill it with the infos we can grab there
	      currSaveElem->setKey(nodeName);
	      for (int i = 0; i < xmlFile->GetAttributeCount(); i++)
		currSaveElem->addAttribute(xmlFile->GetAttributeName(i),
					   xmlFile->GetAttributeValue(i));

	      //if we are a direct son of a WiredDocument
	      if (xmlFile->GetDepth() == lastWiredDocDepth + 1)
		//we add the SaveElement to the currentDocument
		currDoc->data.Add(currSaveElem);
	      else
		{
		  //else we add the SaveElement to the previous SaveElement children
		  previousSaveElem->addChildren(currSaveElem);
		  //and we add it to the history
		  pathToCurrSaveElem.Add(currSaveElem);
		}
	    }
	}
      //if it is the value of the current tag
      else if (nodeType == XML_READER_TYPE_TEXT)
	{
	  //we set it in the current SaveElement
	  currSaveElem->setValue(xmlFile->GetNodeValue());
	}
     //if it is a closing tag
      else if (nodeType == XML_READER_TYPE_END_ELEMENT)
	{
	  if (!pathToCurrSaveElem.IsEmpty())
	    {
	      currSaveElem = pathToCurrSaveElem.Last();
	      pathToCurrSaveElem.Remove(pathToCurrSaveElem.Last());
	    }
	}
    }

   //redistribute the elements loaded
  //In a separated method for readability and for logic
  //check technical documentation for more informations.

  RedistributeHash(dataLoaded);

  //the current project is now a saved one
  setSaved();
}

int		SortDataLoaded(loadedDocument *doc1, loadedDocument *doc2)
{
  if(doc1->id > doc2->id)
    return 1;
  else if(doc2->id > doc1->id)
    return -1;
  else
    return 0;
}

void		SaveCenter::DumpSaveElementArrayHashMap(SaveElementArrayHashMap dataLoaded)
{
  //iterate on the keys of the hash map
  for(SaveElementArrayHashMap::iterator myIt = dataLoaded.begin();
      myIt != dataLoaded.end();
      myIt++)
    //iterate on the elements of the ArrayArray
    for(int z = 0; z < myIt->second->GetCount(); z++)
      //iterate on the elements of the Array
      for(int y = 0; y < myIt->second->Item(z)->GetCount(); y++)
	{
	  std::cerr << "SaveElementArrayHashMap[" << myIt->first.mb_str() << "]";
	  std::cerr << "[" << z << "][" << y << "]->getKey() = ";
	  std::cerr << dataLoaded[myIt->first]->Item(z)->Item(y)->getKey().mb_str() << std::endl;
	}
}

void		SaveCenter::DumpWiredDocumentArrayHashMap(WiredDocumentArrayHashMap toProcess)
{
  for(WiredDocumentArrayHashMap::iterator it = toProcess.begin();
      it != toProcess.end();
      it ++)
    for(int i = 0; i < it->second->GetCount(); i++)
      {
	std::cerr << "WiredDocumentArrayHashMap[" << it->first.mb_str() << "]";
	std::cerr << "[" << i << "]->getName() = ";
	std::cerr << toProcess[it->first]->Item(i)->getName().mb_str() << std::endl;
      }
}


void		SaveCenter::RedistributeHash(LoadedDocumentArray dataLoaded)
{
  WiredDocumentArray				children;
  wxString					childName;
  wxString					currentName;
  WiredDocumentArrayHashMap::iterator		it;
  WiredDocumentArrayHashMap			toProcess;
  WiredDocument					*currentDoc;
  loadedDocument				*currentLoadedDoc;

  int						i;

  std::cout << "[SaveCenter] RedistributeHash begin" << std::endl;

  //TODO : Make tests about what we are treating and handle cases were it fails.

  //init the whole process with the SaveCenter on top
  currentName = getName();
  toProcess[currentName] = new WiredDocumentArray();
  toProcess[currentName]->Add(this);

  while(!toProcess.empty() && !dataLoaded.empty())
  {
	//Take the first element of dataLoaded
	currentLoadedDoc = dataLoaded[0];
	currentName = currentLoadedDoc->name;

	if (toProcess[currentName])
	{
#ifdef __DEBUG__
	  std::cout << "[SaveCenter] currentName = " << currentName.mb_str() << std::endl;
#endif
	  //Take the first element with the same name of toProcess
	  currentDoc = toProcess[currentName]->Item(0);

	  //load the WiredDocument with data from the dataLoaded
	  currentDoc->Load(currentLoadedDoc->data);

#ifdef __DEBUG__
	  std::cout << "[SaveCenter] loaded" << std::endl;
#endif

	  //Retrieve its children
	  children = currentDoc->getChildren();

	  //Add them to the list of WiredDocuments
	  for(i = 0; i < children.GetCount(); i++)
	  {
		childName = children[i]->getName();
		if(toProcess.find(childName) == toProcess.end() || !toProcess[childName])
		  toProcess[childName] = new WiredDocumentArray();
		toProcess[childName]->Add(children[i]);
	  }

	  //Remove the current WiredDocument from the list to Process
	  toProcess[currentName]->Remove(currentDoc);

	  //Clear the hashmap entry if needed
	  if(toProcess[currentName]->IsEmpty())
	  {
		delete toProcess[currentName];
		toProcess.erase(currentName);
	  }
	}

	//Remove the current loadedDocument from the dataLoaded
	dataLoaded.Remove(currentLoadedDoc);
  }
  std::cout << "[SaveCenter] RedistributeHash finished" << std::endl;
}

wxString	SaveCenter::GetDefaultProjectName(wxFileName cwd)
{
  wxString ret(WIRED_DEFAULT_PROJECT_NAME);
  wxString fullPath;
  int i = 0;

  do
    {
      fullPath.Clear();
      i++;
      fullPath << cwd.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) << ret << i;
    }
  while(wxDirExists(fullPath));

  ret << i;

  return ret;
}

wxString	SaveCenter::GetProjectNameFromProjectPath(wxFileName path)
{
  wxString		ret;
  wxArrayString		dirs;

  dirs = path.GetDirs();

  ret = dirs[dirs.GetCount() - 1];

  return ret;
}

void		SaveCenter::DumpWiredDocumentTree()
{
  DumpWiredDocumentSubTree(this, 0);
}

void		SaveCenter::DumpWiredDocumentSubTree(WiredDocument *currentNode,
						     int depth)
{
  int	i;
  WiredDocumentArray	children;


  for(i = 0; i < depth; i++)
    std::cout << " ";

  std::cout << currentNode->getName().mb_str() << std::endl;

  children = currentNode->getChildren();

  for(i = 0; i < children.size(); i++)
    DumpWiredDocumentSubTree(children[i], depth + 1);

}

bool		SaveCenter::IsProject(wxFileName path)
{
  wxFileName	test;

  test = path;

  test.SetName(wxT("wired"));
  test.SetExt(wxT("xml"));

  if(test.FileExists())
    return true;
  else
    return false;
}

bool		SaveCenter::getSaved()
{
  return _saved;
}

void		SaveCenter::setSaved(bool saved)
{
  _saved = saved;
}

void		SaveCenter::CleanProject()
{
  wxString	audioPath;
  wxArrayString	files;

  audioPath = getAudioDir();

  //empty the directory
  //We can assume the audio directory doesn't have any subdirectory
  //If one wants to remove subdirectories, he should remove them before
  //removing the top one.
  wxDir::GetAllFiles(audioPath, &files);
  for(int i = 0; i < files.GetCount(); i++)
    wxRemoveFile(files[i]);

  wxRmdir(audioPath);

  //add there the deletion of the other folders auto generated.
}
