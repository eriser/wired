#ifndef FILESTAT_H_
#define FILESTAT_H_

#include <string>
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
    FileStat() {;}
	FileStat(const FileStat& copy){;}
	~FileStat(){;}
	
	bool            StatFile(const char *FileName);
    wxLongLong      GetSampleRate();
    wxLongLong      GetNbChannels();
    wxString        GetBitNess();
    wxString        GetFormat();
    wxTimeSpan      GetLenght();
    
private:
    SF_INFO              _FileInfo;
};

#endif /*FILESTAT_H_*/
