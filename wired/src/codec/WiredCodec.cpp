#include	"WiredCodec.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

WiredCodec::WiredCodec()
{
  _WiredPath = CODEC_PATH;
}

WiredCodec::WiredCodec(const string& DataPath)
{
  _WiredPath = DataPath + CODEC_PATH;
}

WiredCodec::~WiredCodec()
{
  
}

void WiredCodec::Init()
{
  InitWLib();
  WLoadLib();
}

int WiredCodec::CheckExtension(const string& str)
{
  list<string>::iterator	i;

  for (i = _ExtList.begin(); i != _ExtList.end(); i++)
    if (!(*i).compare(str))
      return 0;
  return 1;
}

list<string> WiredCodec::GetExtension()
{
  list<t_WLib>::iterator		i;
  list<t_LibInfo>::iterator		j;

  for (i = _WLib.begin(); i != _WLib.end(); i++)
    for (j = (*i).Info.begin();  j != (*i).Info.end(); j++)
      if (!_ExtList.empty())
	{
	  if (CheckExtension((*j).Extension))
	    _ExtList.push_back((*j).Extension);
	}
      else
	_ExtList.push_back((*j).Extension);
  return _ExtList;
}

t_WLib WiredCodec::FindBestCodec(string extension)
{
  list<t_WLib>::iterator	i;
  list<t_LibInfo>::iterator	j;
  short				note;
  t_WLib			TheLib;

  note = 0;
  for (i = _WLib.begin(); i != _WLib.end(); i++)
    for (j = (*i).Info.begin();  j != (*i).Info.end(); j++)
      if (!extension.compare((*j).Extension))
	if ((*j).Note > note)
	  {
	    note = (*j).Note;
	    TheLib = (*i);
	  }
  return TheLib;
}

int WiredCodec::Encode(float **pcm, string OutExtension)
{
  list<t_WLib>::iterator	i;
  t_WLib			lib;

  lib = FindBestCodec(OutExtension);
  if (!lib.Codec->encode(pcm))
    {
      cout << "[WIRED_ENCODE] Can't encode flux to pcm" << endl;
      return 0;
    }
  return 1;
}

t_Pcm WiredCodec::Decode(const string filename)
{
  int				fd;
  list<t_WLib>::iterator	i;
  char				buf[BUF_SIZE];
  float				**pcm;
  int				size;
  int				pcm_size;
  t_Pcm				mypcm;

  for (i = _WLib.begin(); i != _WLib.end(); i++)
    if ((*i).Codec->decode(filename, &mypcm))
      break;
  return mypcm;
}

void WiredCodec::InitWLib()
{
  DIR		 *dir;
  struct dirent	*sdir;
  string	pathname;

  if (dir = opendir(_WiredPath.c_str()))
    if (sdir = readdir(dir))
      {
	while (sdir)
	  {
	    pathname = sdir->d_name;
	    if (pathname.find(".so", pathname.length() - 3) != -1)
	      _WiredSo.push_back(pathname);
	    sdir = readdir(dir);
	  }
	closedir(dir);
      }
}

void WiredCodec::WLoadLib()
{
  int c1;

  /*pas oublier de changer pour le path*/
  for (c1 = 0; c1 < _WiredSo.size(); c1++)
    WLibLoader(_WiredPath + string("/") + _WiredSo.at(c1));
}

void WiredCodec::WLibLoader(const string& filename)
{
  list<t_LibInfo>::iterator	i;
  void				*handle;
  WiredCodecConstruct		construct;
  bool				find;

  t_WLib			WLibTemp;
  list<t_LibInfo>		Infos;

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

  find = false;
  for (i = Infos.begin(); i != Infos.end(); i++)
    if ((*i).CodecMask > EXIST)
      {
	WLibTemp.Info = Infos;
	WLibTemp.Codec = NewLib;
	_WLib.push_back(WLibTemp);
	find = true;
	break;
      }
  if (!find)
    dlclose(handle);
}
