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

bool WiredCodec::CanDecode(const string &filename)
{
  int				fd;
  list<t_WLib>::iterator	i;
  list<t_LibInfo>::iterator	j;
  char				*buf;
  bool				valRet;
  int			        ret;

  cout << "blob 1" << endl;
  if ((fd = open(filename.c_str(),  O_RDONLY)) == -1)
    return false;

  valRet = false;
  for (i = _WLib.begin(); i != _WLib.end(); i++)
    for (j = (*i).Info.begin(); j != (*i).Info.end(); j++)
      {
	cout << "extension == " << (*j).Extension.c_str() << endl;
	if ((*j).fccLenght == 0)
	  continue;
	buf = new char((*j).fccLenght);
	if (ret = (read(fd, buf, (*j).fccLenght)))
	  if (strcmp(buf, (*j).fccLabel.c_str()) == 0)
	    {
	      codecToUse[filename] = (*i).Codec->GetUniqueId();
	      valRet = true;
	      delete buf;
	      break;
	    }
	delete buf;
      }
  close(fd);
  cout << "blob2" << endl;
  return valRet;
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

t_Pcm WiredCodec::Decode(const string &filename)
{
  list<t_WLib>::iterator	i;
  list<t_LibInfo>::iterator	j;
  t_Pcm				mypcm;
  unsigned long			id;

  cout << "salut" << endl;
  if (codecToUse.find(filename) != codecToUse.end())
    id = codecToUse[filename];
  else
    {
      mypcm.pcm = NULL;
      return mypcm;
    }
  
  for (i = _WLib.begin(); i != _WLib.end(); i++)
    if ((*i).Codec->GetUniqueId() == id)
      {
	(*i).Codec->decode(filename, &mypcm);
	break;
      }
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

  t_WLib			WLibTemp;
  list<t_LibInfo>		Infos;
  static int			j = 0;

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
  NewLib->SetuniqueId(j++);
  for (i = Infos.begin(); i != Infos.end(); i++)
    if ((*i).CodecMask > EXIST)
      {
	WLibTemp.Info = Infos;
	WLibTemp.Codec = NewLib;
	_WLib.push_back(WLibTemp);
	break;
      }
  dlclose(handle);
}
