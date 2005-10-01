#include	"WiredCodec.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

wxMutex		WiredCodecMutex;

WiredCodec::WiredCodec()
{
  _WiredPath = CODEC_PATH;
  _CurrentUniqueID = 0;
}

WiredCodec::~WiredCodec()
{
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
	  cout << "\tFccStartPos:" << (*j).fccStartPos << endl;
	  cout << "\tfccLenght:" << (*j).fccLenght << endl;
	  cout << "\tFccLabel:" <<  (*j).fccLabel << endl;
	}
    }
}

bool WiredCodec::CanDecode(const string &filename)
{
	int									fd;
	list<t_WLib>::const_iterator		iterTWLib;
	list<t_LibInfo>::const_iterator		iterTLibInfo;
	char								*buf;
	bool								valRet = false;
	int			        				ret;

	WiredCodecMutex.Lock();
//	if ((fd = open(filename.c_str(),  O_RDONLY)) == -1)
//	{
//		cout << "[WIREDCODEC] Can't open file" << endl;
//    	return false;
//	}
//	for (iterTWLib = _WLib.begin(); iterTWLib != _WLib.end(); iterTWLib++)
//    	for (iterTLibInfo = (*iterTWLib).Info.begin(); iterTLibInfo != (*iterTWLib).Info.end(); iterTLibInfo++)
//	    {
//			cout << "extension == " << (*iterTLibInfo).Extension.c_str() << endl;
//			if ((*iterTLibInfo).fccLenght <= 0)
//				continue;
//			buf = new char((*iterTLibInfo).fccLenght * sizeof(char) + 1);
//			buf[(*iterTLibInfo).fccLenght * sizeof(char)] = 0;
//			if (ret = (read(fd, buf, (*iterTLibInfo).fccLenght)))
//				if (strcmp(buf, (*iterTLibInfo).fccLabel.c_str()) == 0)
//			    {
//	      			codecToUse[filename] = (*iterTWLib).Codec->GetUniqueId();
//					valRet = true;
//					delete buf;
//					break;
//			    }
//			delete buf;
//      	}
//	close(fd);
//	cout << "blob2: " << valRet << endl;
	codecToUse[filename] = 0;
	WiredCodecMutex.Unlock();
//	return valRet;
	return true;
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

list<string> WiredCodec::GetExtension()
{
	list<t_WLib>::const_iterator		iterTWLib;
	list<t_LibInfo>::const_iterator		iterLibInfo;
	list<string>						ExtList;

	WiredCodecMutex.Lock();
	for (iterTWLib = _WLib.begin(); iterTWLib != _WLib.end(); iterTWLib++)
    	for (iterLibInfo = (*iterTWLib).Info.begin();  iterLibInfo != (*iterTWLib).Info.end(); iterLibInfo++)
			if (!ExtList.empty())
			{
				if (CheckExtension((*iterLibInfo).Extension, ExtList))
					ExtList.push_back((*iterLibInfo).Extension);
			}
			else
				ExtList.push_back((*iterLibInfo).Extension);
	WiredCodecMutex.Unlock();
	return ExtList;
}

t_WLib WiredCodec::FindBestCodec(string extension)
{
	list<t_WLib>::const_iterator		iterTWLib;
	list<t_LibInfo>::const_iterator		iterTLibInfo;
	short								note;
	t_WLib								TheLib;

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
	t_WLib							lib;

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

t_Pcm WiredCodec::Decode(const string &filename)
{
	list<t_WLib>::const_iterator		iterTWLib;
//  list<t_LibInfo>::const_iterator	iterTLibInfo;
	t_Pcm								mypcm;
	unsigned long						id;

	WiredCodecMutex.Lock();
	cout << "filename: " << filename.c_str() << endl;
	if (codecToUse.find(filename) != codecToUse.end())
    	id = codecToUse[filename];
	else
    {
    	cout << "ici" << endl;
    	mypcm.pcm = NULL;
    	WiredCodecMutex.Unlock();
	    return mypcm;
    }
    cout << "ici1" << endl;
	for (iterTWLib = _WLib.begin(); iterTWLib != _WLib.end(); iterTWLib++)
	{
	    cout << "GetUniqueID: " << (*iterTWLib).Codec->GetUniqueId() << endl;
    	if ((*iterTWLib).Codec->GetUniqueId() == id)
		{
			cout << "ici3" << endl;
      		DumpCodec();
      		char *path = new char(filename.size() * sizeof(char));
			strcpy(path, filename.c_str());
			(*iterTWLib).Codec->decode(path, &mypcm);
			break;
		}
	}
	WiredCodecMutex.Unlock();
	return mypcm;
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

void WiredCodec::WLibLoader(const string& filename)
{
	list<t_LibInfo>::const_iterator		iter;
	void								*handle;
	WiredCodecConstruct					construct;
	t_WLib								WLibTemp;
	list<t_LibInfo>						Infos;
	bool								shouldDeleteNewLib = true;

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
	// Tester la note egalement
	for (iter = Infos.begin(); iter != Infos.end(); iter++)
    	if ((*iter).CodecMask > EXIST)
	    {
			WLibTemp.Info = Infos;
			WLibTemp.Codec = NewLib;
			_WLib.push_back(WLibTemp);
			shouldDeleteNewLib = false;
			break;
	    }
	if (shouldDeleteNewLib == true) delete NewLib;
	dlclose(handle);
}
