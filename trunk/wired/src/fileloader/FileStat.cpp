#include "FileStat.h"

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

wxString        FileStat::GetBitNess()
{
    wxString    Result(_("Sub format unknown"));
    if (_FileInfo.frames > 0)
    {
        int     Format =  _FileInfo.format & SF_FORMAT_SUBMASK;
        switch (Format)
        {
            case SF_FORMAT_PCM_S8 :
                Result = _("Signed 8 bit");
                break;
            case SF_FORMAT_PCM_16 :
                Result = _("Signed 16 bit");            
                break;
            case SF_FORMAT_PCM_24 :
                Result = _("Signed 24 bit");            
                break;
            case SF_FORMAT_PCM_32 :
                Result = _("Signed 32 bit");            
                break;
            case SF_FORMAT_PCM_U8 :
                Result = _("Unsigned 8 bit");            
                break;
            case SF_FORMAT_FLOAT :
                Result = _("32 bit float");            
                break;
            case SF_FORMAT_DOUBLE :
                Result = _("64 bit float");
                break;
            case SF_FORMAT_ULAW :
                Result = _("U-Law");
                break;
            case SF_FORMAT_ALAW :
                Result = _("A-Law");
                break;
            case SF_FORMAT_IMA_ADPCM :
                Result = _("IMA ADPCM");
                break;
            case SF_FORMAT_MS_ADPCM :
                Result = _("Microsoft ADPCM");
                break;
            case SF_FORMAT_GSM610 :
                Result = _("GSM 6.10");
                break;
            case SF_FORMAT_VOX_ADPCM :
                Result = _("Oki Dialogic ADPCM");
                break;            
            case SF_FORMAT_G721_32 :
                Result = _("32kbs G721 ADPCM");
                break;            
            case SF_FORMAT_G723_24 :
                Result = _("24kbs G723 ADPCM");
                break;
            case SF_FORMAT_G723_40 :
                Result = _("40kbs G723 ADPCM");
                break;            
            case SF_FORMAT_DWVW_12 :
                Result = _("12 bit Delta Width Variable Word encoding");
                break;
            case SF_FORMAT_DWVW_16 :
                Result = _("16 bit Delta Width Variable Word encoding");
                break;
            case SF_FORMAT_DWVW_24 :
                Result = _("24 bit Delta Width Variable Word encoding");
                break;
            case SF_FORMAT_DWVW_N :
                Result = _("N bit Delta Width Variable Word encoding");
                break;
            case SF_FORMAT_DPCM_8 :
                Result = _("8 bit differential");
                break;
            case SF_FORMAT_DPCM_16 :
                Result = _("16 bit differential");
                break;
            default :
                break;
        }
    }
    return Result;
}

wxString        FileStat::GetFormat()
{
    wxString    Result(_("Format unknown"));
    if (_FileInfo.frames > 0)
    {
      int     Format =  _FileInfo.format & SF_FORMAT_TYPEMASK;
      switch (Format)
        {
            case SF_FORMAT_WAV :
                Result = _("Microsoft WAV");
                break;
            case SF_FORMAT_AIFF :
                Result = _("Apple/SGI AIFF");            
                break;
            case SF_FORMAT_AU :
                Result = _("Sun/NeXT AU");            
                break;
            case SF_FORMAT_RAW :
                Result = _("RAW PCM");            
                break;
            case SF_FORMAT_PAF :
                Result = _("Ensoniq PARIS");            
                break;
            case SF_FORMAT_SVX :
                Result = _("Amiga IFF / SVX8 / SV16");            
                break;
            case SF_FORMAT_NIST :
                Result = _("Sphere NIST");
                break;
            case SF_FORMAT_VOC :
                Result = _("VOC");
                break;
            case SF_FORMAT_IRCAM :
                Result = _("Berkeley/IRCAM/CARL");
                break;
            case SF_FORMAT_W64 :
                Result = _("Sonic Foundry's 64 bit RIFF/WAV");
                break;
            case SF_FORMAT_MAT4 :
                Result = _("Matlab (tm) V4.2 / GNU Octave 2.0");
                break;
            case SF_FORMAT_MAT5 :
                Result = _("Matlab (tm) V5.0 / GNU Octave 2.1");
                break;
            case SF_FORMAT_PVF :
                Result = _("Portable Voice");
                break;            
            case SF_FORMAT_XI :
                Result = _("Fasttracker 2 Extended Instrument");
                break;            
            case SF_FORMAT_HTK :
                Result = _("HMM Tool Kit");
                break;
            case SF_FORMAT_SDS :
                Result = _("Midi Sample Dump Standard");
                break;            
            case SF_FORMAT_AVR :
                Result = _("Audio Visual Research");
                break;
            case SF_FORMAT_WAVEX :
                Result = _("MS WAVE with WAVEFORMATEX");
                break;
            case SF_FORMAT_SD2 :
                Result = _("Sound Designer 2");
                break;
            case SF_FORMAT_FLAC :
                Result = _("FLAC lossless file");
                break;
            case SF_FORMAT_CAF :
                Result = _("Core Audio File");
                break;
            default :
                break;
        }  
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
