// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#ifndef __WRITEWAVEFILE_H__
#define __WRITEWAVEFILE_H__

#include <string>
#include <iostream>
#include <wx/wx.h>

using namespace std;

#include <sndfile.h>

class WriteWaveFile
{
 public:
  WriteWaveFile(wxString filename, int sample_rate, int channels, unsigned long format, 
		int type = SF_FORMAT_WAV);
  ~WriteWaveFile();

  void		Close();

  sf_count_t	WriteShort(short *ptr, sf_count_t items);
  sf_count_t	WriteInt(int *ptr, sf_count_t items);
  sf_count_t	WriteFloat(float *ptr, sf_count_t items);
  sf_count_t	WriteDouble(double *ptr, sf_count_t items);

  bool		WriteTitle(wxString s);
  bool		WriteCopyright(wxString s);
  bool		WriteSoftware(wxString s);
  bool		WriteArtist(wxString s);
  bool		WriteComment(wxString s);
  bool		WriteDate(wxString s);

  wxString	Filename;

 protected:
  SNDFILE	*sffile;
  SF_INFO	sfinfo;

};

#endif
