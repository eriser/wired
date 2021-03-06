// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "FileStat.h"

FileStat::FileStat()
{
    LoadSubTypes();
    LoadMajorTypes();
}

bool        FileStat::StatFile(wxString& FileName)
{
    SNDFILE *Info;
    Info = sf_open((const char *)FileName.mb_str(*wxConvCurrent), SFM_READ, &_FileInfo);
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
    return (0);
}

wxLongLong      FileStat::GetNbChannels()
{
    if (_FileInfo.frames > 0)
    {
        wxLongLong  Result;
        Result.Assign((double)_FileInfo.channels);
        return Result;
    }
    return (0);
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
        _SubTypes[(format_info.format & SF_FORMAT_SUBMASK)] = wxString(format_info.name, *wxConvCurrent);
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
        _MajorTypes[(format_info.format & SF_FORMAT_TYPEMASK)] = wxString(format_info.name, *wxConvCurrent);
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
