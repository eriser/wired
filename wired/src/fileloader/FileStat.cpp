#include "FileStat.h"

FileStat::FileStat()
{
    LoadSubTypes();
    LoadMajorTypes();
}

bool        FileStat::StatFile(const char *FileName)
{
    SNDFILE *Info;
    Info = sf_open(FileName, SFM_READ, &_FileInfo);
    if (Info == NULL)
        return false;
    sf_close(Info);
    return true;
}

wxLongLong      FileStat::GetSampleRate()
{
    if (_FileInfo.frames > 0)
    {
        wxLongLong  Result;
        Result.Assign((double)_FileInfo.samplerate);
        return Result;
    }
}

wxLongLong      FileStat::GetNbChannels()
{
    if (_FileInfo.frames > 0)
    {
        wxLongLong  Result;
        Result.Assign((double)_FileInfo.channels);
        return Result;
    }  
}

void            FileStat::LoadSubTypes()
{
    SF_FORMAT_INFO  format_info;
    int             count;

    sf_command(NULL, SFC_GET_FORMAT_SUBTYPE_COUNT, &count, sizeof (int));
    while (count-- >= 0)
    {
        format_info.format = count;
        sf_command(NULL, SFC_GET_FORMAT_SUBTYPE, &format_info, sizeof (format_info));
        _SubTypes[(format_info.format & SF_FORMAT_SUBMASK)] = wxString(format_info.name);
    }
}

void            FileStat::LoadMajorTypes()
{
    SF_FORMAT_INFO  format_info;
    int             count;

    sf_command(NULL, SFC_GET_FORMAT_MAJOR_COUNT, &count, sizeof (int));
    while (count-- >= 0)
    {
        format_info.format = count;
        sf_command(NULL, SFC_GET_FORMAT_MAJOR, &format_info, sizeof (format_info));
        _MajorTypes[(format_info.format & SF_FORMAT_TYPEMASK)] = wxString(format_info.name);
    }
}

wxString        FileStat::GetBitNess()
{
    wxString    Result(_("Sub format unknown"));
    if (_FileInfo.frames > 0)
    {
        int     Format =  _FileInfo.format & SF_FORMAT_SUBMASK;
        if (_SubTypes.find(Format) != _SubTypes.end())
            return _SubTypes[Format];
        else
            return Result;
    }
    return Result;
}

wxString        FileStat::GetFormat()
{
    wxString    Result(_("Format unknown"));
    if (_FileInfo.frames > 0)
    {
      int     Format =  _FileInfo.format & SF_FORMAT_TYPEMASK;
      if (_MajorTypes.find(Format) != _MajorTypes.end())
            return _MajorTypes[Format];
        else
            return Result;
    }
    return Result;
}

wxTimeSpan      FileStat::GetLenght()
{
    wxTimeSpan  Result;
    
    if (_FileInfo.frames > 0)
    {
        Result = wxTimeSpan::Seconds(_FileInfo.frames / _FileInfo.samplerate);
    }
    return Result;
}
