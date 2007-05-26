// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include	"WiredCodec.h"
#include	<config.h>

#include <wx/dir.h>

WiredCodec::WiredCodec()
{
  _WiredPath = wxString(wxT(LIB_DIR));
  _CurrentUniqueID = 0;
  path = 0;
}

WiredCodec::~WiredCodec()
{
	list<t_WLib>::const_iterator	iter;
	
	for (iter = _WLib.begin(); iter != _WLib.end(); iter++)
	#ifdef WIN32
		cout << "dlopen" << endl;
	#else
		dlclose((*iter).handle);
	#endif
	if (path)
	  delete path;
}

WiredCodec		WiredCodec::operator=(const WiredCodec& right)
{
	if (this !=& right)
	{
		_WLib = right._WLib;
		_WiredPath = right._WiredPath;
		_WiredSo = right._WiredSo;
		codecToUse = right.codecToUse;
		_CurrentUniqueID = right._CurrentUniqueID;
		path = right.path;
		_DecodeExtList = right._DecodeExtList;
		_EncodeExtList = right._EncodeExtList;
	}
	return *this;
}

int	WiredCodec::EndDecode()
{
  list<t_WLib>::const_iterator		iterTWLib;
  unsigned long						id;
    
  if (codecToUse.find(wxString(path, *wxConvCurrent)) != codecToUse.end())
    id = codecToUse[wxString(path, *wxConvCurrent)];
  for (iterTWLib = _WLib.begin(); iterTWLib != _WLib.end(); iterTWLib++)
    {
      if ((*iterTWLib).Codec->GetUniqueId() == id)
	{
	  (*iterTWLib).Codec->EndDecode();
	  break;
	}
    }
  delete path;
  path = NULL;
  return 0;
}

void WiredCodec::DumpCodec()
{
  list<t_WLib>::iterator	i;
  list<t_LibInfo>::iterator	j;
  
  for (i = _WLib.begin(); i != _WLib.end(); i++)
    {
      cout << "ID bebe:" << (*i).Codec->GetUniqueId() << endl;
      cout << "\tAdresse codec:" << (*i).Codec << endl;
      for (j = (*i).Info.begin(); j != (*i).Info.end(); j++)
	{
	  cout << "\t-----" << endl;
	  cout << "\tCodecMask:" << (*j).CodecMask << endl;
	  cout << "\tExtension:" << (*j).Extension << endl;
	  cout << "\tNote:" << (*j).Note << endl;
	}
    }
}

bool WiredCodec::CanConvert(const wxString &filename, int Decode)
{
  list<t_WLib>::const_iterator		iterTWLib;
  list<t_LibInfo>::const_iterator	iterTLibInfo;
  bool					result = false;
  int					Note = 0;

  std::cout << "Can decode file == {" << filename.mb_str() << "}" << std::endl;
  WiredCodecMutex.Lock();
  for (iterTWLib = _WLib.begin(); iterTWLib != _WLib.end(); iterTWLib++)
    for (iterTLibInfo = (*iterTWLib).Info.begin();  iterTLibInfo != (*iterTWLib).Info.end(); iterTLibInfo++)
      if ((*iterTLibInfo).CodecMask & Decode)
	if ((*iterTWLib).Codec->CanConvert(filename, Decode) == true)
	  if ((*iterTLibInfo).Note > Note)
	    {
	      codecToUse[filename] = (*iterTWLib).Codec->GetUniqueId();
	      result = true;
	      Note = (*iterTLibInfo).Note;
	    }
  WiredCodecMutex.Unlock();
  return result;
}

void WiredCodec::Init()
{
  WiredCodecMutex.Lock();
  InitWLib();
  WLoadLib();
  WiredCodecMutex.Unlock();
}

int WiredCodec::CheckExtension(const wxString& str, const list<wxString>& ExtList)
{
  list<wxString>::const_iterator	iter;
  
  for (iter = ExtList.begin(); iter != ExtList.end(); iter++)
    if (!(*iter).compare(str))
      return 0;
  return 1;
}

list<wxString> WiredCodec::GetExtension(int Decode)
{  
  if (Decode & DECODE)
    return _DecodeExtList;
  return _EncodeExtList;
}

t_WLib WiredCodec::FindBestCodec(wxString extension)
{
  list<t_WLib>::const_iterator		iterTWLib;
  list<t_LibInfo>::const_iterator	iterTLibInfo;
  short					note;
  t_WLib				TheLib;

  note = 0;
  for (iterTWLib = _WLib.begin(); iterTWLib != _WLib.end(); iterTWLib++)
    for (iterTLibInfo = (*iterTWLib).Info.begin();  iterTLibInfo != (*iterTWLib).Info.end(); iterTLibInfo++)
      if (!extension.compare((*iterTLibInfo).Extension))
	if ((*iterTLibInfo).Note > note)
	  {
	    note = (*iterTLibInfo).Note;
	    TheLib = (*iterTWLib);
	  }
  return TheLib;
}

int WiredCodec::Encode(float **pcm, wxString OutExtension)
{
  list<t_WLib>::const_iterator	iterTWLib;
  t_WLib			lib;
  
  WiredCodecMutex.Lock();
  lib = FindBestCodec(OutExtension);
  if (!lib.Codec->encode(pcm))
    {
      cout << "[WIRED_ENCODE] Can't encode flux to pcm" << endl;
      WiredCodecMutex.Unlock();
      return 0;
    }
  WiredCodecMutex.Unlock();
  return 1;
}

unsigned long WiredCodec::Decode(const wxString &filename, t_Pcm *pcm, unsigned long length)
{
  list<t_WLib>::const_iterator		iterTWLib;
  unsigned long				id;
  unsigned long				retLenght = 0;
  
  WiredCodecMutex.Lock();
  if (codecToUse.find(filename) != codecToUse.end())
    id = codecToUse[filename];
  else
    {
      pcm->pcm = NULL;
      WiredCodecMutex.Unlock();
      return retLenght;
    }
  if (!path)
    {
      path = new char(filename.size() * sizeof(*path) + 1);
      
      strncpy(path, (const char *)filename.mb_str(*wxConvCurrent), filename.size() + 1);
    }
  for (iterTWLib = _WLib.begin(); iterTWLib != _WLib.end(); iterTWLib++)
    {
      if ((*iterTWLib).Codec->GetUniqueId() == id)
	{
	  retLenght = (*iterTWLib).Codec->decode(path, pcm, length);
	  break;
	}
    }
  WiredCodecMutex.Unlock();
  return retLenght;
}

//
// search for every .so file in the _WiredPath dir
// and push it in the _WiredSo list.
//

void			WiredCodec::InitWLib()
{
  wxDir			dir(_WiredPath);
  wxString		filename;
  bool			cont;
  
  if (dir.IsOpened())
    {
      cont = dir.GetFirst(&filename, wxT(""), wxDIR_FILES);
      while (cont)
	{
	  if (filename.AfterLast('.') == wxT("so"))
	    _WiredSo.push_back(filename);
	  cont = dir.GetNext(&filename);
	}
    }
}

void WiredCodec::WLoadLib()
{
  list<wxString>::const_iterator	iter;
  
  for (iter = _WiredSo.begin(); iter != _WiredSo.end(); iter++)
    WLibLoader(_WiredPath + wxString(wxT("/"), *wxConvCurrent) + *iter);
}

void	WiredCodec::FeelExtension(list<t_LibInfo> Info)
{
  list<t_LibInfo>::const_iterator	iterLibInfo;
  
  for (iterLibInfo = Info.begin(); iterLibInfo != Info.end(); iterLibInfo++)
    {
      if ((*iterLibInfo).CodecMask & DECODE)
	if (!_DecodeExtList.empty())
	  {
	    if (CheckExtension((*iterLibInfo).Extension, _DecodeExtList))
	      _DecodeExtList.push_back((*iterLibInfo).Extension);
	  }
	else
	  _DecodeExtList.push_back((*iterLibInfo).Extension);
      if ((*iterLibInfo).CodecMask & ENCODE)
	if (!_EncodeExtList.empty())
	  {
	    if (CheckExtension((*iterLibInfo).Extension, _EncodeExtList))
	      _EncodeExtList.push_back((*iterLibInfo).Extension);
	  }
	else
	  _EncodeExtList.push_back((*iterLibInfo).Extension);
    }
}

void WiredCodec::WLibLoader(const wxString& filename)
{
  list<t_LibInfo>::const_iterator	iter;
  void					*handle;
  WiredCodecConstruct			construct;
  t_WLib				WLibTemp;
  list<t_LibInfo>			Infos;
  bool					shouldDeleteNewLib = true;
  
  #ifdef WIN32
  handle = 0; cout << "dlopen" << endl;
  #else
  handle = dlopen(filename.mb_str(*wxConvCurrent), RTLD_LAZY);
  #endif

  if (!handle)
    return;
  #ifdef WIN32
  cout << "dlsym" << endl;
  #else
  construct = (WiredCodecConstruct) dlsym(handle, WLIBCONSTRUCT);
  #endif
  if (!construct)
    {
      #ifdef WIN32
      cout << "dlclose" << endl;
      #else
      dlclose(handle);
      #endif
      return;
    }
  WiredApiCodec *NewLib = construct();
  NewLib->init(Infos);
  NewLib->SetuniqueId(_CurrentUniqueID++);
  for (iter = Infos.begin(); iter != Infos.end(); iter++)
    if ((*iter).CodecMask & EXIST)
      {
	WLibTemp.handle = handle;
	WLibTemp.Info = Infos;
	WLibTemp.Codec = NewLib;
	FeelExtension(WLibTemp.Info);
	_WLib.push_back(WLibTemp);
	shouldDeleteNewLib = false;
	break;
      }
  if (shouldDeleteNewLib == true)
    {
      delete NewLib;
      #ifdef WIN32
      cout << "dlclose" << endl;
      #else
      dlclose(handle);
      #endif
    }
}
