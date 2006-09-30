// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "ClipBoard.h"

cClipBoard* cClipBoard::spSingleton = 0;


cClipBoard::cClipBoard(wxString filename, bool loadmem, t_opening_mode open_mode)
: WaveFile (filename, loadmem, open_mode)
{
   sizec = 0;
}


cClipBoard::~cClipBoard()
{
	if(spSingleton)
	  delete spSingleton;
  wxRemoveFile(wxT("/tmp/tmp.wav"));
}


cClipBoard& cClipBoard::Global()
{
  if (spSingleton == 0)
    spSingleton = new cClipBoard();
  return *spSingleton;
}


void cClipBoard::Copy (WaveFile& wave, int from, int size_of_copy)
{

  if (size_of_copy < 0)
    {
      from = from + size_of_copy;
      size_of_copy = -size_of_copy;
    }

  sizec = size_of_copy;

  // Buffer which receives datas to copy
  float		*rw_buffer = new float [wave.GetNumberOfChannels() * WAVE_TEMP_SIZE];
  sf_count_t	tmp_frames = 0;

  sf_command (GetFilePtr(), SFC_FILE_TRUNCATE, &tmp_frames, sizeof (tmp_frames)) ;
  sf_command (GetFilePtr(), SFC_UPDATE_HEADER_NOW, NULL, SF_FALSE) ;
  //Setting the pointer's position to start the copy
  wave.SetCurrentPosition (from);

  //Copying frames which must be moved
  sf_count_t	frames_to_read = size_of_copy;
  sf_count_t	count = (frames_to_read > WAVE_TEMP_SIZE) ? WAVE_TEMP_SIZE : frames_to_read;

  count = wave.ReadFloatF(rw_buffer, count);
  while (count > 0)
    {
      count = WriteFloatF(rw_buffer, count);
      frames_to_read -= count;
      count = (frames_to_read > WAVE_TEMP_SIZE) ? WAVE_TEMP_SIZE : frames_to_read;
      if (count > 0)
	count = wave.ReadFloatF(rw_buffer, count);
    }

  //Updating the file's header: does it work ?
  //sf_command (sffile, SFC_UPDATE_HEADER_NOW, NULL, 0) ;
  sfinfo.frames = NumberOfFrames = size_of_copy;
  sfinfo.channels = wave.GetNumberOfChannels();
  sfinfo.format = wave.GetFormat();
  sfinfo.samplerate = wave.GetSampleRate();

  delete[] rw_buffer;
}

void cClipBoard::Cut (WaveFile& wave, int from, int size_of_cut)
{
  //cout << "[cClipBoard] - Cut " << endl;
  //cout << "[cClipBoard] - Cut : " << size_of_cut << " frames to cut" << endl;

  if (size_of_cut < 0)
    {
      from = from + size_of_cut;
      size_of_cut = -size_of_cut;
    }

  //Verifying the appropriate mode (read/write)
  if (wave.GetOpenMode() != rwrite)
    throw cException (wxT("File opened in read only mode"));

  sizec = size_of_cut;

  //Buffer which receives datas to copy
  float		*rw_buffer = new float [wave.GetNumberOfChannels() * WAVE_TEMP_SIZE];

  //Copying frames to cut in the clipboard
  Copy(wave, from, size_of_cut);

  //Moving frames to the left
  sf_count_t	frames_to_move = wave.GetNumberOfFrames() - (from + size_of_cut) ;
  sf_count_t	count = (frames_to_move > WAVE_TEMP_SIZE) ? WAVE_TEMP_SIZE : frames_to_move;
  int		idx = 0;
  int		read_pos = from + size_of_cut;

  wave.SetCurrentPosition (read_pos);
  count = wave.ReadFloatF(rw_buffer, count);
  while (count > 0)
    {
      wave.SetCurrentPosition(from + idx);
      count = wave.WriteFloatF(rw_buffer, count);
      idx += count;
      frames_to_move -= count;
      count = (frames_to_move > WAVE_TEMP_SIZE) ? WAVE_TEMP_SIZE : frames_to_move;
      wave.SetCurrentPosition(read_pos + idx);
      if (count > 0)
	count = wave.ReadFloatF(rw_buffer, count);
    }

  //Truncing files in order to delete the superfluity frames
  sf_count_t  frames = wave.GetNumberOfFrames() - size_of_cut ;
  sf_command (wave.GetFilePtr(), SFC_FILE_TRUNCATE, &frames, sizeof (frames)) ;

  //Updating the wave frames' number
  wave.SetNumberOfFrames (frames);

  //Updating the wave's header
  sf_command (wave.GetFilePtr(), SFC_UPDATE_HEADER_NOW, NULL, SF_FALSE) ;

  delete[] rw_buffer;
}

void cClipBoard::Paste (WaveFile& wave, int to)
{
  long		size_paste;


  if ( wave.GetOpenMode() != rwrite )
    throw cException (wxT("File opened in read only mode"));

  if ( GetNumberOfFrames() <= 0 )
    throw cException (wxT("Paste : Nothing to paste"));

  size_paste = sizec;
  //cout << "[cClipBoard] - Paste : " << GetNumberOfFrames() << " frames to paste" << endl;

  int		size_of_paste = GetNumberOfFrames();

  //Buffer which receives datas to copy
  float		*rw_buffer = new float [wave.GetNumberOfChannels() * WAVE_TEMP_SIZE];

  //Creating a temp file
  WaveFile	temp(wxT("/tmp/tmp1.wav"), false, rwrite);

  //Copying the right frames of the insert position in the temp file
  int		frames_nbr = 0;

  temp.SetCurrentPosition(0);
  wave.SetCurrentPosition(to);

  sf_count_t	read_frames = wave.ReadFloatF(rw_buffer);

  while (read_frames > 0)
  {
	temp.WriteFloatF(rw_buffer, read_frames);
	frames_nbr += read_frames;
	read_frames = wave.ReadFloatF(rw_buffer);
  }

  //Updating frames' number of the temp file
  temp.SetNumberOfFrames(frames_nbr);

  //Writing the new ones from position "to"
  wave.SetCurrentPosition(to);
  SetCurrentPosition(0);
  read_frames = ReadFloatF(rw_buffer);
  while (read_frames > 0)
    {
      wave.WriteFloatF(rw_buffer, read_frames);
      read_frames = ReadFloatF(rw_buffer);
    }

  //Copying moved frames
  temp.SetCurrentPosition(0);
  read_frames = temp.ReadFloatF(rw_buffer);
  while ((read_frames > 0) && (size_paste > 0))
    {
      wave.WriteFloatF(rw_buffer, read_frames);
      size_paste -= read_frames;
      read_frames = temp.ReadFloatF(rw_buffer);
      if (read_frames > size_paste)
		read_frames = size_paste;
    }

  //Updating the number of the wave frames
  wave.SetNumberOfFrames (wave.GetNumberOfFrames() + sizec);

  //Updating wave's header
  sf_command (wave.GetFilePtr(), SFC_UPDATE_HEADER_NOW, NULL, SF_FALSE) ;

  wxRemoveFile(wxT("/tmp/tmp1.wav"));
  delete[] rw_buffer;
}


void cClipBoard::Delete (WaveFile& wave, int from, int size_of_cut)
{
  if (size_of_cut < 0)
    {
      from = from + size_of_cut;
      size_of_cut = -size_of_cut;
    }

  //Verifying the appropriate mode (read/write)
  if (wave.GetOpenMode() != rwrite)
    throw cException (wxT("File opened in read only mode"));

  //Buffer which receives datas to copy
  float		*rw_buffer = new float [wave.GetNumberOfChannels() * WAVE_TEMP_SIZE];


  //Moving frames to the left
  sf_count_t	frames_to_move = wave.GetNumberOfFrames() - (from + size_of_cut) ;
  sf_count_t	count = ( frames_to_move > WAVE_TEMP_SIZE ) ? WAVE_TEMP_SIZE : frames_to_move;
  int		idx = 0;
  int		read_pos = from + size_of_cut;

  wave.SetCurrentPosition (read_pos);
  count = wave.ReadFloatF (rw_buffer, count);
  while (count > 0)
    {
      wave.SetCurrentPosition (from + idx);
      count = wave.WriteFloatF (rw_buffer, count);
      idx += count;
      frames_to_move -= count;
      count = (frames_to_move > WAVE_TEMP_SIZE) ? WAVE_TEMP_SIZE : frames_to_move;
      wave.SetCurrentPosition (read_pos + idx);
      if (count > 0)
	count = wave.ReadFloatF (rw_buffer, count);
    }

  //Truncing the file in order to delete the superfluity frames
  sf_count_t  frames = wave.GetNumberOfFrames() - size_of_cut ;
  sf_command (wave.GetFilePtr(), SFC_FILE_TRUNCATE, &frames, sizeof (frames)) ;

  //Updating the number of the wave frames
  wave.SetNumberOfFrames (frames);

  //Updating wave's header
  sf_command (wave.GetFilePtr(), SFC_UPDATE_HEADER_NOW, NULL, SF_FALSE) ;

  delete[] rw_buffer;
}
