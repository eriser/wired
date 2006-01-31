#include	"WiredCodec.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

WiredCodec::WiredCodec()
{
  _WiredPath = CODEC_PATH;
  _CurrentUniqueID = 0;
  path = 0;
}

WiredCodec::~WiredCodec()
{
	list<t_WLib>::const_iterator	iter;
	
	for (iter = _WLib.begin(); iter != _WLib.end(); iter++)
		dlclose((*iter).handle);
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
    
  if (codecToUse.find(path) != codecToUse.end())
    id = codecToUse[path];
  for (iterTWLib = _WLib.begin(); iterTWLib != _WLib.end(); iterTWLib++)
    {
      if ((*iterTWLib).Codec->GetUniqueId() == id)
	{
	  (*iterTWLib).Codec->EndDecode();
	  break;
	}
    }
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

bool WiredCodec::CanConvert(const string &filename, int Decode)
{
  list<t_WLib>::const_iterator		iterTWLib;
  list<t_LibInfo>::const_iterator	iterTLibInfo;
  bool					result = false;
  int					Note = 0;

  std::cout << "Can decode file == {" << filename.c_str() << "}" << std::endl;
  WiredCodecMutex.Lock();
  for (iterTWLib = _WLib.begin(); iterTWLib != _WLib.end(); iterTWLib++)
    for (iterTLibInfo = (*iterTWLib).Info.begin();  iterTLibInfo != (*iterTWLib).Info.end(); iterTLibInfo++)
      if ((*iterTLibInfo).CodecMask & Decode)
	if ((*iterTWLib).Codec->CanConvert((char*)filename.c_str(), Decode) == true)
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

int WiredCodec::CheckExtension(const string& str, const list<string>& ExtList)
{
  list<string>::const_iterator	iter;
  
  for (iter = ExtList.begin(); iter != ExtList.end(); iter++)
    if (!(*iter).compare(str))
      return 0;
  return 1;
}

list<string> WiredCodec::GetExtension(int Decode)
{  
  if (Decode & DECODE)
    return _DecodeExtList;
  return _EncodeExtList;
}

t_WLib WiredCodec::FindBestCodec(string extension)
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

int WiredCodec::Encode(float **pcm, string OutExtension)
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

unsigned long WiredCodec::Decode(const string &filename, t_Pcm *pcm, unsigned long length)
{
  list<t_WLib>::const_iterator		iterTWLib;
  unsigned long						id;
  unsigned long						retLenght = 0;
  
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
      path = new char(filename.size() * sizeof(char) + 1);
      strncpy(path, filename.c_str(), filename.size());
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

void WiredCodec::InitWLib()
{
  DIR		 *dir;
  struct dirent	*sdir;
  string	pathname;
  
  if (dir = opendir(_WiredPath.c_str()))
    {
      while (sdir = readdir(dir))
	{
	  pathname = sdir->d_name;
	  if (pathname.find(".so", pathname.length() - 3) != -1)
	    _WiredSo.push_back(pathname);
	}
      closedir(dir);
    }
}

void WiredCodec::WLoadLib()
{
  list<string>::const_iterator	iter;
  
  for (iter = _WiredSo.begin(); iter != _WiredSo.end(); iter++)
    WLibLoader(_WiredPath + string("/") + *iter);
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

void WiredCodec::WLibLoader(const string& filename)
{
  list<t_LibInfo>::const_iterator	iter;
  void					*handle;
  WiredCodecConstruct			construct;
  t_WLib				WLibTemp;
  list<t_LibInfo>			Infos;
  bool					shouldDeleteNewLib = true;
  
  handle = dlopen(filename.c_str(), RTLD_LAZY);
  if (!handle)
    return;
  construct = (WiredCodecConstruct) dlsym(handle, WLIBCONSTRUCT);
  if (!construct)
    {
      dlclose(handle);
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
      dlclose(handle);
    }
}
