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
  
  // buffer destine a recevoir les donnees a copier
  float * rw_buffer = new float [wave.GetNumberOfChannels() * WAVE_TEMP_SIZE];
  
  sf_count_t tmp_frames = 0;
  sf_command (GetFilePtr(), SFC_FILE_TRUNCATE, &tmp_frames, sizeof (tmp_frames)) ;
  sf_command (GetFilePtr(), SFC_UPDATE_HEADER_NOW, NULL, SF_FALSE) ;
  // On se positionne au debut de la selection a copier 
  wave.SetCurrentPosition (from);
  
  // Copie des frames a deplacer
  sf_count_t frames_to_read = size_of_copy;
  sf_count_t count = ( frames_to_read > WAVE_TEMP_SIZE ) ? WAVE_TEMP_SIZE : frames_to_read;
  count = wave.ReadFloatF (rw_buffer, count);
  while ( count > 0 )
    {
      count = WriteFloatF (rw_buffer, count);
      frames_to_read -= count;
      count = ( frames_to_read > WAVE_TEMP_SIZE ) ? WAVE_TEMP_SIZE : frames_to_read;
      if (count > 0 ) 
	count = wave.ReadFloatF (rw_buffer, count);
    }
  
  // Mets a jour l'entete du fichier : marche ???
  // sf_command (sffile, SFC_UPDATE_HEADER_NOW, NULL, 0) ;
  
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
  
  // Verifie qu'on est en mode read/write
  if ( wave.GetOpenMode() != rwrite )
    throw cException (wxT("File opened in read only mode"));
  
  sizec = size_of_cut;
  // buffer destine a recevoir les donnees a copier
  float * rw_buffer = new float [wave.GetNumberOfChannels() * WAVE_TEMP_SIZE];
  
  // Copier les frames a 'couper' dans le clipboard
  Copy(wave, from, size_of_cut);
  
  // Decaler les frames vers la gauche
  sf_count_t frames_to_move = wave.GetNumberOfFrames() - (from + size_of_cut) ;
  sf_count_t count = ( frames_to_move > WAVE_TEMP_SIZE ) ? WAVE_TEMP_SIZE : frames_to_move;
  
  int idx = 0;
  int read_pos = from + size_of_cut;
  wave.SetCurrentPosition (read_pos);
  count = wave.ReadFloatF (rw_buffer, count);
  while (count > 0)
    {
      wave.SetCurrentPosition (from + idx);
      count = wave.WriteFloatF (rw_buffer, count);
      idx += count;
      frames_to_move -= count;
      count = ( frames_to_move > WAVE_TEMP_SIZE ) ? WAVE_TEMP_SIZE : frames_to_move;
      wave.SetCurrentPosition (read_pos + idx);
      if (count > 0 ) 
	count = wave.ReadFloatF (rw_buffer, count);
    }
  
  // Tronquer le fichier de facon a supprimer les frames superfus
  sf_count_t  frames = wave.GetNumberOfFrames() - size_of_cut ;
  sf_command (wave.GetFilePtr(), SFC_FILE_TRUNCATE, &frames, sizeof (frames)) ;
  
  // Met a jour le nombre de frames du wave
  wave.SetNumberOfFrames (frames);
  
  // Met a jour le header du wave
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
  
  int size_of_paste = GetNumberOfFrames();
  
  // buffer destine a recevoir les donnees a copier
  float * rw_buffer = new float [wave.GetNumberOfChannels() * WAVE_TEMP_SIZE];

  // Cree un fichier temporaire 
  WaveFile temp (wxT("/tmp/tmp1.wav"), false, rwrite);

  // Copier les frames a droite de la position d'insertion ds le fichier temp
  int frames_nbr = 0;
  temp.SetCurrentPosition(0);
  wave.SetCurrentPosition(to);
  sf_count_t read_frames = wave.ReadFloatF(rw_buffer);
  while (read_frames > 0)
  {
	temp.WriteFloatF(rw_buffer, read_frames);
	frames_nbr += read_frames;
	read_frames = wave.ReadFloatF(rw_buffer);
  }

  // Met a jour le nombre de frames du fichier temp
  temp.SetNumberOfFrames(frames_nbr);

  // Ecrire les nouveaux a partir de l'emplacement position
  wave.SetCurrentPosition(to);
  SetCurrentPosition(0);
  read_frames = ReadFloatF(rw_buffer);
  while (read_frames > 0)
    {
      wave.WriteFloatF(rw_buffer, read_frames);
      read_frames = ReadFloatF(rw_buffer);
    }
  
  // Copie des frames 'decales'
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
  
  // Met a jour le nombre de frames du wave
  wave.SetNumberOfFrames (wave.GetNumberOfFrames() + sizec);
  
  
  // Met a jour le header du wave
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
  
  // Verifie qu'on est en mode read/write
  if ( wave.GetOpenMode() != rwrite )
    throw cException (wxT("File opened in read only mode"));
  
  // buffer destine a recevoir les donnees a copier
  float * rw_buffer = new float [wave.GetNumberOfChannels() * WAVE_TEMP_SIZE];
  
  
  // Decaler les frames vers la gauche
  sf_count_t frames_to_move = wave.GetNumberOfFrames() - (from + size_of_cut) ;
  sf_count_t count = ( frames_to_move > WAVE_TEMP_SIZE ) ? WAVE_TEMP_SIZE : frames_to_move;
  
  int idx = 0;
  int read_pos = from + size_of_cut;
  wave.SetCurrentPosition (read_pos);
  count = wave.ReadFloatF (rw_buffer, count);
  while (count > 0)
    {
      wave.SetCurrentPosition (from + idx);
      count = wave.WriteFloatF (rw_buffer, count);
      idx += count;
      frames_to_move -= count;
      count = ( frames_to_move > WAVE_TEMP_SIZE ) ? WAVE_TEMP_SIZE : frames_to_move;
      wave.SetCurrentPosition (read_pos + idx);
      if (count > 0 ) 
	count = wave.ReadFloatF (rw_buffer, count);
    }
  
  // Tronquer le fichier de facon a supprimer les frames superfus
  sf_count_t  frames = wave.GetNumberOfFrames() - size_of_cut ;
  sf_command (wave.GetFilePtr(), SFC_FILE_TRUNCATE, &frames, sizeof (frames)) ;
  
  // Met a jour le nombre de frames du wave
  wave.SetNumberOfFrames (frames);
  
    // Met a jour le header du wave
  sf_command (wave.GetFilePtr(), SFC_UPDATE_HEADER_NOW, NULL, SF_FALSE) ;
  
  delete[] rw_buffer;
}
