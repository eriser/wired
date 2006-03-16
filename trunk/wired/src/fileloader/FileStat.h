#ifndef FILESTAT_H_
#define FILESTAT_H_

#include <string>
#include <map>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif
#include <wx/datetime.h>
#include <wx/file.h>
#include <sndfile.h>
using namespace std;

typedef struct				s_file_info
{
  long					Type;
  long					Steps;
  double				Max;
  double				InitValue;
} t_file_info;

class		FileStat
{
public:
    FileStat();
	FileStat(const FileStat& copy){;}
	~FileStat(){;}
	
	bool            StatFile(const wxChar *FileName);
    wxLongLong      GetSampleRate();
    wxLongLong      GetNbChannels();
    wxString        GetBitNess();
    wxString        GetFormat();
    wxTimeSpan      GetLenght();
    
private:
    void            LoadSubTypes();
    void            LoadMajorTypes();

    SF_INFO              _FileInfo;
    map<int, wxString>   _SubTypes;
    map<int, wxString>   _MajorTypes;
};

#endif /*FILESTAT_H_*/
