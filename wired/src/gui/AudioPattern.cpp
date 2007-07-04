// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <math.h>
#include <wx/filename.h>
#include "SequencerGui.h"
#include "Sequencer.h"
#include "Colour.h"
#include "OptionPanel.h"
#include "Mixer.h"
#include "AudioCenter.h"
#include "ColoredBox.h"
#include "AudioPattern.h"
#include "HelpPanel.h"
#include "Track.h"
#include "SeqTrack.h"
#include "AudioEngine.h"
#include "WaveFile.h"
#include "WriteWaveFile.h"
#include "Channel.h"
#include "SaveCenter.h"

static long				audio_pattern_count = 1;
extern SaveCenter	*saveCenter;

// basically called when we add an empty audio track
AudioPattern::AudioPattern(WiredDocument *parent, double pos, double endpos, long trackindex)
  : Pattern(parent, wxT("AudioPattern"), pos, endpos, trackindex),
    WaveDrawer(Pattern::GetSize())
{
  Init(NULL, parent);
}

// basically called when we import a wave file
AudioPattern::AudioPattern(WiredDocument *parent, double pos, WaveFile *w, long trackindex)
  : Pattern(parent,wxT("AudioPattern"), pos, pos + Seq->MeasurePerSample * w->GetNumberOfFrames(), trackindex),
    WaveDrawer(Pattern::GetSize())
{
  Init(w, parent);
}

AudioPattern::~AudioPattern()
{
  Wave->DelAssociatedPattern();
  OptPanel->DeleteTools(this);
  if (InputChan) delete InputChan;
  if (RecordWave) delete RecordWave;
}

void					AudioPattern::Init(WaveFile* w, WiredDocument* parent)
{
#ifdef __DEBUG__
  cout << " ### NEW AUDIO PATTERN ###\n\t Position: "<< Position << "; EndPosition: " << EndPosition << "; Length: " << Length
       << "; StartWavePos: " << StartWavePos << "; EndWavePos: " << EndWavePos << endl;
#endif
  // init graphics things (must be prior)
  Name = wxString::Format(wxT("T%d A%d"), (int) TrackIndex + 1, (int) audio_pattern_count++);
  wxSize s = GetSize();
  SetSize(s);
  WaveDrawer::SetWave(w, s); // the WaveDrawer has WaveFile instance.

  // init audio things
  SetWave(w);
  LastBlock = -1;
  RecordWave = 0;
  InputChan = NULL;
  RecordWave = NULL;

  _documentParent = parent;

  Connect(GetId(), wxEVT_MOTION, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnMotion);
  Connect(GetId(), wxEVT_LEFT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnClick);
  Connect(GetId(), wxEVT_LEFT_UP, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnLeftUp);
  Connect(GetId(), wxEVT_RIGHT_DOWN, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
	  &AudioPattern::OnRightClick);
  // Double Click action commented FOR EPITECH FORUM PURPOSE
  //   Connect(GetId(), wxEVT_LEFT_DCLICK, (wxObjectEventFunction)(wxEventFunction)(wxMouseEventFunction)
  // 	  &AudioPattern::OnDoubleClick);
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
  if (HelpWin == NULL)
      return;
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
  wxMutexLocker				m(SeqMutex);

#ifdef __DEBUG__
  printf("\tAudioPattern : MeasurePerSample %f , Frames %d\n",
	 Seq->MeasurePerSample, Wave->GetNumberOfFrames());
#endif
  Length = Seq->MeasurePerSample * (EndWavePos - StartWavePos);
  EndPosition = Position + Length;
  Update();
}

void					AudioPattern::SetWave(WaveFile *w)
{
#ifdef __DEBUG__
  cout << "WaveDrawer::StartWavePos = " << WaveDrawer::StartWavePos<< " WaveDrawer::EndWavePos = " << WaveDrawer::EndWavePos << endl;
#endif
  WaveDrawer::SetWave(w, GetSize(), StartWavePos, EndWavePos);
  if (w)
    {
      FileName = w->Filename;
      wxMutexLocker  m(SeqMutex);

      OnBpmChange();
    }
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
        memset(buf[0], 0, sizeof(float) * Audio->SamplesPerBuffer);
        memset(buf[1], 0, sizeof(float) * Audio->SamplesPerBuffer);
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
       << " with audio dir : " << saveCenter->getAudioDir().mb_str() << endl;
  while (!done)
    {
      s = saveCenter->getAudioDir() + wxT("/wired_audio");
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
	  InputChan = Mix->OpenInput(Seq->Tracks[TrackIndex]->GetTrackOpt()->DeviceId);
	  Mix->FlushInput(Seq->Tracks[TrackIndex]->GetTrackOpt()->DeviceId);
	  cout << "[AUDIOPATTERN] Recording on input: "
	       << Seq->Tracks[TrackIndex]->GetTrackOpt()->DeviceId << endl;
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
      SetWave(w);
      Refresh();
    }
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
  p = new AudioPattern(_documentParent, pos, Wave, TrackIndex);
  Wave->AddAssociatedPattern();
  SeqMutex.Lock();
  p->FileName = Wave->Filename;
  p->StartWavePos = StartWavePos;
  p->EndWavePos = EndWavePos;
  p->EndPosition = pos +  Length;
  p->Length = Length;
  p->SetDrawColour(WaveDrawer::PenColor);
  p->Position = pos;
  p->Update();
  SeqMutex.Unlock();
  //Seq->Tracks[TrackIndex]->AddColoredPattern((Pattern *) p);

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
      Wave->AddAssociatedPattern();
#ifdef __DEBUG__
      cout << " >>> HERE OLD:\n\t Position = " << Position << "\n\t Length = " << Length << "\n\t EndPosition = " << EndPosition << endl;
      cout << "new pos: " << pos << endl;
#endif
      p = new AudioPattern(_documentParent, pos, EndPosition, TrackIndex);
#ifdef __DEBUG__
      cout << " >>> HERE NEW :\n\t p->Position = " << p->Position << "\n\t p->Length = " << p->Length << "\n\t p->EndPosition = " << p->EndPosition << endl;
#endif
      SeqMutex.Lock();
      p->StartWavePos = StartWavePos + (long) floor((pos - Position) * Seq->SamplesPerMeasure);
      p->EndWavePos = p->StartWavePos + (long) floor(p->Length * Seq->SamplesPerMeasure);

      p->SetWave(Wave);
      p->FileName = FileName;
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

void				AudioPattern::Save()
{
  wxFileName			file(FileName);

  file.MakeRelativeTo(saveCenter->getAudioDir());
  saveDocData(new SaveElement(wxT("FileName"), file.GetFullPath()));
  Pattern::Save();
}

void				AudioPattern::Load(SaveElementArray data)
{
  int				i;
  WaveFile*		newWave;
  for (i = 0; i < data.GetCount(); i++)
    {
      if (data[i]->getKey() == wxT("FileName"))
	{
	  wxFileName		file(data[i]->getValue());

	  file.MakeAbsolute(saveCenter->getAudioDir());
	  FileName = file.GetFullPath();
	  newWave = WaveCenter.AddWaveFile(FileName);
	  Init(newWave, _documentParent);
	  Pattern::Load(data);
	}
    }
}
