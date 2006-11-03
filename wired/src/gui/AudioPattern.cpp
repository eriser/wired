// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include <wx/filename.h>
#include "SequencerGui.h"
#include "Sequencer.h"
#include "Colour.h"
#include "OptionPanel.h"
#include "Mixer.h"
#include "AudioCenter.h"
#include "../xml/WiredSessionXml.h"
#include "ColoredBox.h"
#include "AudioPattern.h"
#include "HelpPanel.h"
#include "../sequencer/Track.h"
#include "../gui/SeqTrack.h"
#include "../engine/AudioEngine.h"
#include "../audio/WaveFile.h"
#include "../audio/WriteWaveFile.h"
#include "../mixer/Channel.h"

static long				audio_pattern_count = 1;
extern WiredSessionXml	*CurrentXmlSession;

AudioPattern::AudioPattern(double pos, double endpos, long trackindex)
  : Pattern(pos, endpos, trackindex),
    WaveDrawer(Pattern::GetSize())
{
  Init(NULL);
}

AudioPattern::AudioPattern(double pos, WaveFile *w, long trackindex)
  : Pattern(pos, pos + Seq->MeasurePerSample * w->GetNumberOfFrames(), trackindex),
    WaveDrawer(Pattern::GetSize())
{
  Init(w);
}

AudioPattern::~AudioPattern()
{
  OptPanel->DeleteTools(this);
  if (InputChan) delete InputChan;
  if (RecordWave) delete RecordWave;
}

void					AudioPattern::Init(WaveFile* w)
{
#ifdef __DEBUG__
  cout << " ### NEW AUDIO PATTERN ###\n\t Position: "<< Position << "; EndPosition: " << EndPosition << "; Length: " << Length
       << "; StartWavePos: " << StartWavePos << "; EndWavePos: " << EndWavePos << endl;
#endif

  Pattern::PenColor = CL_PATTERN_NORM;
  Pattern::BrushColor = CL_WAVEDRAWER_BRUSH;
  Name = wxString::Format(wxT("T%d A%d"), TrackIndex + 1, audio_pattern_count++);
  LastBlock = -1;  
  RecordWave = 0;
  InputChan = NULL;
  RecordWave = NULL;
  if (w)
    {
      SetWave(w);
      wxSize s = GetSize();
      SetSize(s);
      WaveDrawer::SetWave(w, s);
    }

  Connect(GetId(), wxEVT_MOTION, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnMotion);
  Connect(GetId(), wxEVT_LEFT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnClick);
  Connect(GetId(), wxEVT_LEFT_UP, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnLeftUp);
  Connect(GetId(), wxEVT_RIGHT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnRightClick);
  Connect(GetId(), wxEVT_LEFT_DCLICK, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnDoubleClick);
  Connect(GetId(), wxEVT_PAINT, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnPaint);
  Connect(GetId(), wxEVT_SIZE, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnSize);
  Connect(GetId(), wxEVT_ENTER_WINDOW, (wxObjectEventFunction)(wxEventFunction)
	  (wxMouseEventFunction) &AudioPattern::OnHelp);
  SeqPanel->PutCursorsOnTop();

#ifdef __DEBUG__
  cout << " ---  OK AUDIO PATTERN ---\n\t Position: "<< Position << "; EndPosition: " << EndPosition << "; Length: " << Length
       << "; StartWavePos: " << StartWavePos << "; EndWavePos: " << EndWavePos << endl;
#endif
}

void					AudioPattern::OnHelp(wxMouseEvent &event)
{
  if (HelpWin->IsShown())
    {
      wxString s(_("This is an Audio pattern. Double-click on it to open the Audio editor."));
      HelpWin->SetText(s);
    }
}

void					AudioPattern::Update()
{
#ifdef __DEBUG__
  printf(" [ START ] AudioPattern::Update()\n");
#endif
  Pattern::Update();
  AudioPattern::SetSize(Pattern::GetSize());
  wxWindow::SetPosition(Pattern::GetMPosition() -
			wxPoint((int) floor(SeqPanel->CurrentXScrollPos), (int) SeqPanel->CurrentYScrollPos));
  SeqPanel->SetScrolling();
#ifdef __DEBUG__
  printf(" [  END  ] AudioPattern::Update()\n");
#endif
}

void					AudioPattern::OnBpmChange()
{
  Length = Seq->MeasurePerSample * Wave->GetNumberOfFrames();
  EndPosition = Position + Length;
  Update();
}

void					AudioPattern::SetFullWave(WaveFile *w)
{
  if (!w)
    {
      StartWavePos = 0;
      EndWavePos = 0;
      NumberOfChannels = 0;
    }
  else
    FileName = w->Filename;
  WaveDrawer::SetWave(w, GetSize());
}

void					AudioPattern::SetWave(WaveFile *w)
{
#ifdef __DEBUG__
  cout << "WaveDrawer::StartWavePos = " << WaveDrawer::StartWavePos<< " WaveDrawer::EndWavePos = " << WaveDrawer::EndWavePos << endl;
#endif
 
 if (!w)
    {
      StartWavePos = 0;
      EndWavePos = 0;
      NumberOfChannels = 0;
    }
  else
    FileName = w->Filename;
  WaveDrawer::SetWave(w, GetSize(), StartWavePos, EndWavePos);
}

void					AudioPattern::SetDrawing()
{
  WaveDrawer::SetPenColor(Pattern::PenColor);
  WaveDrawer::SetBrushColor(Pattern::BrushColor);
  WaveDrawer::SetDrawing(GetSize());
  Refresh();
}

float					**AudioPattern::GetBlock(long block)
{
  float					**buf;
  long					size;
  long					pos;

  if (!Wave)
    return (0x0);
  pos = (block * Audio->SamplesPerBuffer) + StartWavePos;
  size = EndWavePos - pos;
  if (size > 0)
    {
      buf = new float *[2];
      if (size > Audio->SamplesPerBuffer)
	size = Audio->SamplesPerBuffer; 
      buf[0] = new float[Audio->SamplesPerBuffer];
      buf[1] = new float[Audio->SamplesPerBuffer];
      memset(buf[0], 0, sizeof(float) * sizeof(Audio->SamplesPerBuffer));
      memset(buf[1], 0, sizeof(float) * sizeof(Audio->SamplesPerBuffer));
      Wave->Read(buf, pos, size);
      return (buf);
    }
  return (0x0);
}

void					AudioPattern::SetSelected(bool sel)
{
  Pattern::SetSelected(sel);
  if (sel)
    Pattern::BrushColor = CL_WAVEDRAWER_BRUSH_SEL;
  else
    Pattern::BrushColor = CL_WAVEDRAWER_BRUSH;
  WaveDrawer::PenColor = Pattern::PenColor;
  WaveDrawer::BrushColor = Pattern::BrushColor;
  WaveDrawer::RedrawBitmap(GetSize());
  Refresh();
}

bool					AudioPattern::PrepareRecord(int type)
{
  wxString				s;
  bool					done = false;
  int					i = 1;

  cout << "Preparing record for pattern " << this 
       << " with audio dir : " << CurrentXmlSession->GetAudioDir().mb_str() << endl;
  while (!done)
    {
      s = CurrentXmlSession->GetAudioDir() + wxT("/wired_audio");
      s += wxString::Format(wxT("%d"), i);
      s += wxT(".wav");

      wxFileName f(s);
      if (!f.FileExists())
	done = true;
      else
	i++;
    }
  if (!s.IsEmpty())
    {
      try
	{
	  long				sample_rate;

	  wxString::Format(wxT("%f"), Audio->SampleRate).ToLong(&sample_rate);
	  RecordWave = new WriteWaveFile(s, sample_rate, 1, type);
	  FileName = s;
	  InputChan = Mix->OpenInput(Seq->Tracks[TrackIndex]->TrackOpt->DeviceId);
	  Mix->FlushInput(Seq->Tracks[TrackIndex]->TrackOpt->DeviceId);
	  cout << "[AUDIOPATTERN] Recording on input: " 
	       << Seq->Tracks[TrackIndex]->TrackOpt->DeviceId << endl;
	  return (true);
	}
      catch (...)
	{
	  cout << "[AUDIOPATTERN] Error : could not create audio file" << endl; ; // FIXME
	}
    }
  else
    cout << "[AUDIOPATTERN] Error : could not create audio file" << endl;    
  return (false);
}

void					AudioPattern::StopRecord()
{
  wxString				rec_name;
  WriteWaveFile				*rec_w;
  WaveFile				*w;

  SeqMutex.Lock();
  Mix->RemoveChannel(InputChan);
  InputChan = 0x0;

  // save data before erasing reference
  rec_name = RecordWave->Filename;
  rec_w = RecordWave;

  RecordWave = NULL;
  SeqMutex.Unlock();

  // be sure to delete it after setting his reference to NULL
  delete rec_w;

  // display wave
  w = WaveCenter.AddWaveFile(rec_name);
  if (w)
    {
      SetFullWave(w);
      Refresh();
    }
  /*
  SeqMutex.Lock();
  Wave = w;
  SeqMutex.Unlock();
  */
}

void					AudioPattern::GetRecordBuffer()
{
  float					*f;

  if (InputChan)
    {
      int i;
      for ( i = 0; i <= InputChan->CurBuf; i++ )
	{
	  f = InputChan->PopBuffer(i);
	  if (f)
	    {
	      RecordWave->WriteFloat(f, Audio->SamplesPerBuffer);
	    }
	}
      /*
	f = InputChan->PopBuffer(i);
	if (f)
	RecordWave->WriteFloat(f, InputChan->OffSet);
      */
    }
}

Pattern					*AudioPattern::CreateCopy(double pos)
{
  AudioPattern				*p;

#ifdef __DEBUG__
  printf(" [ START ] AudioPattern::CreateCopy(%f) on track %d\n", pos, TrackIndex);
#endif
   p = new AudioPattern(pos, Wave, TrackIndex);
   SeqMutex.Lock();
   p->StartWavePos = StartWavePos;
   p->EndWavePos = EndWavePos;
   p->EndPosition = pos +  Length;
   p->Length = Length;
   p->SetDrawColour(WaveDrawer::PenColor);
   p->Update();
   SeqMutex.Unlock();
   Seq->Tracks[TrackIndex]->AddColoredPattern((Pattern *) p);
 
  //p = new AudioPattern(pos, Wave, TrackIndex);
  //p = Seq->Tracks[TrackIndex]->AddPattern(Wave, pos);
  //printf("AudioPattern::CreateCopy(%d) new pat %d -- OVER\n", pos, p);
#ifdef __DEBUG__
  printf(" [  END  ] AudioPattern::CreateCopy(%f) on track %d\n", pos, TrackIndex);
#endif
  return ((Pattern *)p);
}

void					AudioPattern::OnClick(wxMouseEvent &e)
{
  Pattern::OnClick(e);
  if (SeqPanel->Tool == ID_TOOL_SPLIT_SEQUENCER)
    {
      Split((double) ((GetMPosition().x + e.m_x)
		      / (MEASURE_WIDTH * SeqPanel->HoriZoomFactor)));
    }
  else if (SeqPanel->Tool == ID_TOOL_MERGE_SEQUENCER)
    {
    }
  else if (SeqPanel->Tool == ID_TOOL_PAINT_SEQUENCER)
    SetDrawColour(SeqPanel->ColorBox->GetColor());
}

void					AudioPattern::OnLeftUp(wxMouseEvent &e)
{
  Pattern::OnLeftUp(e);
}

void					AudioPattern::Split(double pos)
{
  AudioPattern				*p;

 
  if ((Position < pos) && (pos < EndPosition))
    {
      SeqMutex.Lock();
#ifdef __DEBUG__
      cout << " >>> HERE OLD:\n\t Position = " << Position << "\n\t Length = " << Length << "\n\t EndPosition = " << EndPosition << endl;
      cout << "new pos: " << pos << endl;
#endif
      p = new AudioPattern(pos, EndPosition, TrackIndex);
#ifdef __DEBUG__
      cout << " >>> HERE NEW :\n\t p->Position = " << p->Position << "\n\t p->Length = " << p->Length << "\n\t p->EndPosition = " << p->EndPosition << endl;
#endif
      p->StartWavePos = StartWavePos + (long) floor((pos - Position) * Seq->SamplesPerMeasure);
      p->EndWavePos = p->StartWavePos + (long) floor(p->Length * Seq->SamplesPerMeasure);
      
      p->SetWave(Wave);
      p->SetDrawColour(WaveDrawer::PenColor);
      p->SetCursor(GetCursor());
      if (IsSelected())
	p->SetSelected(false);
      p->Update();
      EndWavePos = p->StartWavePos;
      Length = (EndPosition = pos) - Position;
      
      SetDrawing();
      Update();
      SeqMutex.Unlock();
      Seq->Tracks[TrackIndex]->AddColoredPattern((Pattern *) p);
    }
  else
    std::cout << "[AudioPattern] Can't split pattern (got incorrect position)." << pos << endl;
}

void					AudioPattern::SetDrawColour(wxColour c)
{ 
  Pattern::SetDrawColour(c);
  WaveDrawer::PenColor = c;
  RedrawBitmap(GetSize());
  //Refresh();
  
}

void					AudioPattern::OnDoubleClick(wxMouseEvent &e)
{
  OnClick(e);
  if (SeqPanel->Tool == ID_TOOL_MOVE_SEQUENCER)
    OptPanel->ShowWave(this);
  /*printf("audio (%d) me (%d)\n", audio, me);*/
}

void					AudioPattern::OnDirectEdit()
{
  //  if (SeqPanel->Tool == ID_TOOL_MOVE_SEQUENCER)
  OptPanel->ShowWave(this);
}

void					AudioPattern::OnRightClick(wxMouseEvent &e)
{
  Pattern::OnRightClick(e);
}

void					AudioPattern::OnMotion(wxMouseEvent &e)
{
  Pattern::OnMotion(e);
}

void					AudioPattern::OnPaint(wxPaintEvent &e)
{
  wxPaintDC				dc(this);
  wxRegionIterator			region(GetUpdateRegion());

  WaveDrawer::OnPaint(dc, GetSize(), region);
  Pattern::DrawName(dc, GetSize());
}

void					AudioPattern::OnSize(wxSizeEvent &e)
{
  if (FullDraw && (Data || (Wave && !Wave->LoadedInMem)))
    {
      WaveDrawer::SetDrawing(GetSize());
      Refresh();
    }  
}

void					AudioPattern::SetSize(wxSize s)
{
  //  printf(" [ START ] AudioPattern::SetSize(wxSize s) >> [ %d ] [ %d ]\n", s.x, s.y);
  if (s == wxWindow::GetSize())
    return;
  wxWindow::SetSize(s);
  if (Data || (Wave && !Wave->LoadedInMem))
    {
      WaveDrawer::SetDrawing(s);
      Refresh();
    }
  //  printf(" [  END  ] AudioPattern::SetSize(wxSize s) >> [ %d ] [ %d ]\n", s.x, s.y);
}

AudioPattern			AudioPattern::operator=(const AudioPattern& right)
{
	if (this != &right)
	{
		//TODO xdrag = right.xdrag (When used)
		//TODO ydrag = right.ydrag (When used)
		Position = right.Position;
		EndPosition = right.EndPosition;
		Length = right.Length;
		TrackIndex = right.TrackIndex;
		StateMask = right.StateMask;
		m_pos = right.m_pos;
		m_size = right.m_size;
		m_click = right.m_click;
		Name = right.Name;
		//TODO PenColor = right.PenColor;
		//TODO BrushColor = right.BrushColor;

		InputChan = right.InputChan;
		LastBlock = right.LastBlock;
		FileName = right.FileName;
		RecordWave = right.RecordWave;
	}
	return *this;
}
