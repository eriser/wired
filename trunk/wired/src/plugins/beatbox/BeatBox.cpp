#include "BeatBox.h"
#include <wx/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include "midi.h"

#define DELETE_RYTHMS(R) {						\
                           for (unsigned char ps = 0; ps < 8; ps++)	\
			     {						\
			       for (list<BeatNote*>::iterator		\
				      bn = R[ps].begin();		\
				      bn != R[ps].end();)		\
				 {					\
				   delete *bn;				\
				   bn = R[ps].erase(bn);		\
				 }					\
			     }						\
			   delete [] R;					\
			 }


#define CLIP(x)	{							\
			if (x < 0.f)					\
			  { x = 0.f; }					\
			else if (x > 1.f)				\
			  { x = 1.f }					\
		}

static PlugInitInfo info;

BEGIN_EVENT_TABLE(WiredBeatBox, wxWindow)
  EVT_PAINT(WiredBeatBox::OnPaint)
  EVT_COMMAND_SCROLL(BB_OnMasterChange, WiredBeatBox::OnMasterChange)
  EVT_BUTTON(BB_OnStepsUp, WiredBeatBox::OnStepsUp)
  EVT_BUTTON(BB_OnStepsDown, WiredBeatBox::OnStepsDown)
  EVT_BUTTON(BB_PatternClick, WiredBeatBox::OnPatternClick)
  EVT_BUTTON(BB_ShowOpt, WiredBeatBox::ShowOpt)
  EVT_BUTTON(BB_OnPlayClick, WiredBeatBox::OnPlay)
  EVT_BUTTON(BB_OnEditClick, WiredBeatBox::OnEdit)
  EVT_BUTTON(BB_Channel, WiredBeatBox::OnToggleChannel)
  EVT_BUTTON(BB_OnLoadPatch, WiredBeatBox::OnLoadPatch)
  EVT_BUTTON(BB_OnSavePatch, WiredBeatBox::OnSavePatch)
  EVT_BUTTON(BB_OnPatternSelectors, WiredBeatBox::OnPatternSelectors)
  EVT_BUTTON(BB_OnSigChoice, WiredBeatBox::OnSigChoice)
  EVT_BUTTON(BB_OnPosChoice, WiredBeatBox::OnPositionChoice)
END_EVENT_TABLE()
  
WiredBeatBox::WiredBeatBox(PlugStartInfo &startinfo, PlugInitInfo *initinfo) 
  : Plugin(startinfo, initinfo)
{
  cout << "[WIREDBEATBOX] Host is " << GetHostProductName()
       << " version " << GetHostProductVersion() << endl;
  
  
  /* Master Volume */
  MVol = 
    new HintedKnob(this, BB_OnMasterChange, this,
	    new wxImage(string(GetDataDir() + string(KNOB)).c_str(),
			wxBITMAP_TYPE_PNG),
	    new wxImage(string(GetDataDir() + string(DOT)).c_str(),
			     wxBITMAP_TYPE_PNG),
		 0, 127, 100, 1,
		 wxPoint(32,108), wxSize(20,20), wxPoint(52,128));
  MVol->SetValue(100);
  MLevel = 1.0f;
  View = 0x0;

  /* Bitmaps */
  SetBackgroundColour(wxColour(200, 200, 200));
  wxImage* img_bg = 
    new wxImage(string(GetDataDir() + string(BEATBOX_BG)).c_str(), 
		wxBITMAP_TYPE_PNG);  
  if (img_bg)
    BgBmp = new wxBitmap(img_bg);
  
  

  Imgs = new wxImage*[MAX_BITMAPS];
  Imgs[ID_UNCLICKED] = new wxImage(string(GetDataDir() + string(BEATBTN_UNCLICKED)).c_str(), wxBITMAP_TYPE_PNG);
  Imgs[ID_MEDIUM] = new wxImage(string(GetDataDir() + string(BEATBTN_MEDIUM)).c_str(), wxBITMAP_TYPE_PNG);
  Imgs[ID_VLOW] = new wxImage(string(GetDataDir() + string(BEATBTN_VLOW)).c_str(), wxBITMAP_TYPE_PNG);
  Imgs[ID_LOW] = new wxImage(string(GetDataDir() + string(BEATBTN_LOW)).c_str(), wxBITMAP_TYPE_PNG);
  Imgs[ID_HIGH] = new wxImage(string(GetDataDir() + string(BEATBTN_HIGH)).c_str(), wxBITMAP_TYPE_PNG);
  Imgs[ID_VHIGH] = new wxImage(string(GetDataDir() + string(BEATBTN_VHIGH)).c_str(), wxBITMAP_TYPE_PNG);
  
  
  Bitmaps = new wxBitmap*[MAX_BITMAPS];
  if (Imgs[ID_UNCLICKED])
    Bitmaps[ID_UNCLICKED] = new wxBitmap( Imgs[ID_UNCLICKED] );
  if (Imgs[ID_MEDIUM])
    Bitmaps[ID_MEDIUM] =  new wxBitmap( Imgs[ID_MEDIUM] );
  if (Imgs[ID_VLOW])
    Bitmaps[ID_VLOW] =  new wxBitmap( Imgs[ID_VLOW] );
  if (Imgs[ID_LOW])
    Bitmaps[ID_LOW] = new wxBitmap( Imgs[ID_LOW] );
  if (Imgs[ID_HIGH])
    Bitmaps[ID_HIGH] = new wxBitmap( Imgs[ID_HIGH] );
  if (Imgs[ID_VHIGH])
    Bitmaps[ID_VHIGH] = new wxBitmap( Imgs[ID_VHIGH] );
  
  wxImage* img_tmp = 
    new wxImage(string(GetDataDir() + string(POSON)).c_str(), 
		wxBITMAP_TYPE_PNG);
  if (img_tmp)
    PositionOn = new wxBitmap(img_tmp);
  img_tmp = 
    new wxImage(string(GetDataDir() + string(POSOFF)).c_str(), 
		wxBITMAP_TYPE_PNG);
  if (img_tmp)
    PositionOff = new wxBitmap(img_tmp);
  
  
  /* general   */
  SetBackgroundColour(wxColour(200, 200, 200));
  
  /* Optional view button */
  wxImage *opt_img_up = new wxImage(string(GetDataDir() + string(SHOWOPT_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage *opt_img_dn = new wxImage(string(GetDataDir() + string(SHOWOPT_DOWN)).c_str(), wxBITMAP_TYPE_PNG);
  
  if (opt_img_up && opt_img_dn)
    OptViewBtn = new DownButton(this, BB_ShowOpt, wxPoint(571, 225), 
				wxSize(14, 17), opt_img_up, opt_img_dn, true);
  
  /* Play Button */
  Playing = false;
  AutoPlay = false;
  SeqPlaying = false;
  wxImage* play_up = new wxImage(string(GetDataDir() + string(PLAY_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage* play_down = new wxImage(string(GetDataDir() + string(PLAY_DO)).c_str(), wxBITMAP_TYPE_PNG);
  if (play_up && play_down)
    PlayButton = 
      new DownButton(this, BB_OnPlayClick, wxPoint(6,252), 
		     wxSize(27,34), play_up, play_down, false);
  
  /* Edit Button */
  wxImage* edit_up = new wxImage(string(GetDataDir() + string(EDIT_UP)).c_str(), wxBITMAP_TYPE_PNG);
  wxImage* edit_down = new wxImage(string(GetDataDir() + string(EDIT_DO)).c_str(), wxBITMAP_TYPE_PNG);
  if (edit_up && edit_down)
    EditButton = 
      new DownButton(this, BB_OnEditClick, wxPoint(15,227), 
		     wxSize(15,24), edit_up, edit_down, false);
  
  /* Load/Save Patchs/Patterns */
  wxImage* tmp_img = new wxImage(string(GetDataDir() + string(SAVEPATCH_UP)).c_str());
  wxImage* tmp_img2 = 
    new wxImage(string(GetDataDir() + string(SAVEPATCH_DO)).c_str());
  
  DownButton* db;
  if (tmp_img && tmp_img2)
    db = new DownButton(this, BB_OnSavePatch, wxPoint(24, 169), wxSize(14,15),
			tmp_img, tmp_img2, true);
  
  tmp_img = new wxImage(string(GetDataDir() + string(LOADPATCH_UP)).c_str());
  tmp_img2 = new wxImage(string(GetDataDir() + string(LOADPATCH_DO)).c_str());
  if (tmp_img && tmp_img2)
    db = 
      new DownButton(this, BB_OnLoadPatch, wxPoint(24, 150), wxSize(15,17), 
		     tmp_img, tmp_img2, true);
  
  wxStaticText* PatchLabel =
    new wxStaticText(this, -1, "p31kit", wxPoint(30, 190), wxSize(40, 10));
  PatchLabel->SetFont(wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL));
  PatchLabel->SetForegroundColour(*wxBLACK);
  PatchLabel->Show();
  
  /* Signature params */
  wxImage* up_up = new wxImage(string(GetDataDir() + string(STEP_UP_UP)).c_str());
  wxImage* up_down = new wxImage(string(GetDataDir() + string(STEP_UP_DO)).c_str());
  wxImage* down_up = new wxImage(string(GetDataDir() + string(STEP_DO_UP)).c_str());
  wxImage* down_down = new wxImage(string(GetDataDir() + string(STEP_DO_DO)).c_str());
  
  
  StepsLabel = new wxStaticText(this, -1, "16", wxPoint(491, 228), 
				wxSize(20,15), wxALIGN_LEFT);
  StepsLabel->SetFont(wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL));
  StepsLabel->SetForegroundColour(*wxWHITE);
  
  HoldButton* StepsBtn;
  if (up_up && up_down)
    StepsBtn = new HoldButton( this, BB_OnStepsUp,
					   wxPoint(506, 228), wxSize(11, 8), 
					   up_up, up_down);
  if (down_up && down_down)
    StepsBtn = new HoldButton( this, BB_OnStepsDown, 
			       wxPoint(506, 234), wxSize(11, 8), 
			       down_up, down_down);
  
  //Signatures = new float[5];
  Signatures[0] = (float)((float)1/(float)4);
  Signatures[1] = (float)((float)1/(float)8);
  Signatures[2] = (float)((float)1/(float)16);
  Signatures[3] = (float)((float)1/(float)32);
  Signatures[4] = (float)((float)1/(float)64);
  //SigDen = new char[5];
  SigDen[0] = 4;
  SigDen[1] = 8;
  SigDen[2] = 16;
  SigDen[3] = 32;
  SigDen[4] = 64;
  SignatureDen = 16;

  Steps = 16;
  Signature = (float)((float)1/(float)16);
  
  StepsSigCoef = static_cast<double>
    ( Steps / static_cast<double>(SignatureDen) );
  
  //SamplesPerBar = (unsigned long)GetSamplesPerBar();
  SamplesPerBar =
    static_cast<long>
    ((static_cast<double>(GetSamplesPerBar() * StepsSigCoef )
      )
     );
   
  SignatureButtons = new IdButton*[5];
  PositionButtons = new IdButton*[4];
  
  wxImage* posup = new wxImage(string(GetDataDir() + string(POS_UP)).c_str());
  wxImage* posdown = new wxImage(string(GetDataDir() + string(POS_DO)).c_str());
  
  if (posup && posdown) {
    SignatureButtons[0] = 
      new IdButton(this, BB_OnSigChoice, wxPoint(264, 225), wxSize(13,10),
		   posup, posdown, 0);
    SignatureButtons[1] = 
      new IdButton(this, BB_OnSigChoice, wxPoint(264, 235), wxSize(13,10),
		   posup, posdown, 1);
    SignatureButtons[2] = 
      new IdButton(this, BB_OnSigChoice, wxPoint(312, 225), wxSize(13,10),
		   posup, posdown, 2);
    SignatureButtons[3] = 
      new IdButton(this, BB_OnSigChoice, wxPoint(312, 235), wxSize(13,10),
		   posup, posdown, 3);
    SignatureButtons[4] = 
      new IdButton(this, BB_OnSigChoice, wxPoint(370, 225), wxSize(13,10),
		   posup, posdown, 4);
    SigIndex = 2;
    SignatureButtons[SigIndex]->SetOn();
    
    PositionButtons[0] = 
      new IdButton(this, BB_OnPosChoice, wxPoint(122, 225), wxSize(13,10),
		   posup, posdown, 0);
    PositionButtons[1] = 
      new IdButton(this, BB_OnPosChoice, wxPoint(122, 235), wxSize(13,10),
		   posup, posdown, 1);
    PositionButtons[2] = 
      new IdButton(this, BB_OnPosChoice, wxPoint(160, 225), wxSize(13,10),
		   posup, posdown, 2);
    PositionButtons[3] = 
      new IdButton(this, BB_OnPosChoice, wxPoint(160, 235), wxSize(13,10),
		   posup, posdown, 3);
    PosIndex = 0;
    PositionButtons[PosIndex]->SetOn();
  }
    
  
  /* Pattern Selectors */
  NewSelectedPattern = SelectedPattern = 0;
  
  PatternSelectors = new IdButton*[8];
  tmp_img = new wxImage(string(GetDataDir() + string(UP_1)).c_str());
  tmp_img2 = new wxImage(string(GetDataDir() + string(DO_1)).c_str());
  if (tmp_img && tmp_img2)
    PatternSelectors[0] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(33, 225), wxSize(12,14),
		    tmp_img, tmp_img2, 0 );
  

  tmp_img = new wxImage(string(GetDataDir() + string(UP_2)).c_str());
  tmp_img2 = new wxImage(string(GetDataDir() + string(DO_2)).c_str());
  if (tmp_img && tmp_img2)
    PatternSelectors[1] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(48, 225), wxSize(12,14),
		    tmp_img, tmp_img2, 1 );
  tmp_img = new wxImage(string(GetDataDir() + string(UP_3)).c_str());
  tmp_img2 = new wxImage(string(GetDataDir() + string(DO_3)).c_str());
  if (tmp_img && tmp_img2)
    PatternSelectors[2] =
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(33, 240), wxSize(12,14),
		    tmp_img, tmp_img2, 2 );
  
  tmp_img = new wxImage(string(GetDataDir() + string(UP_4)).c_str());
  tmp_img2 = new wxImage(string(GetDataDir() + string(DO_4)).c_str());
  if (tmp_img && tmp_img2)
    PatternSelectors[3] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(48, 240), wxSize(12,14),
		    tmp_img, tmp_img2, 3 );
  
  tmp_img = new wxImage(string(GetDataDir() + string(UP_5)).c_str());
  tmp_img2 = new wxImage(string(GetDataDir() + string(DO_5)).c_str());
  if (tmp_img && tmp_img2)
    PatternSelectors[4] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(33, 255), wxSize(12,14),
		    tmp_img, tmp_img2, 4 );
  
  tmp_img = new wxImage(string(GetDataDir() + string(UP_6)).c_str());
  tmp_img2 = new wxImage(string(GetDataDir() + string(DO_6)).c_str());
  if (tmp_img && tmp_img2)
    PatternSelectors[5] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(48, 255), wxSize(12,14),
		    tmp_img, tmp_img2, 5 );
  
  tmp_img = new wxImage(string(GetDataDir() + string(UP_7)).c_str());
  tmp_img2 = new wxImage(string(GetDataDir() + string(DO_7)).c_str());
  if (tmp_img && tmp_img2)
  PatternSelectors[6] = 
    new IdButton( this, BB_OnPatternSelectors,
		  wxPoint(33, 270), wxSize(12,14),
		  tmp_img, tmp_img2, 6 );
  
  tmp_img = new wxImage(string(GetDataDir() + string(UP_8)).c_str());
  tmp_img2 = new wxImage(string(GetDataDir() + string(DO_8)).c_str());
  if (tmp_img && tmp_img2)
    PatternSelectors[7] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(48, 270), wxSize(12,14),
		    tmp_img, tmp_img2, 7 );
  
  PatternSelectors[SelectedPattern]->SetOn();
  
  /* Pattern list */
  Beat = new BeatButton*[16];
  //BeatLabels = new wxStaticText*[16];
  wxString s;
  
  PositionLeds = new wxStaticBitmap*[16];
  
  for (int i = 0, x = 80; i < 16; i++)
    {
      
      if (i % 4)
	x += BTN_SIZE + 2;
      else if (i != 0)
	x += BTN_SIZE + 10;
      
      PositionLeds[i] = 
	new wxStaticBitmap(this, -1, *PositionOff, 
			   wxPoint(x+BTN_SIZE/2, 246), wxSize(6,6));
      Beat[i] = new BeatButton( this, BB_PatternClick, 
				wxPoint(x,255), wxSize(BTN_SIZE,BTN_SIZE), 
				Bitmaps, i );
    }

  /* Channels */
  Channels = new BeatBoxChannel*[12];
  int notenum = 0x48; //C3
  for (int i = 0; i < 12; i++)
    {
      ChanMidiNotes[i] = notenum++;
      Channels[i] = new BeatBoxChannel(this, BB_Channel, 
				       wxPoint(i*44 + 70, 10), wxSize(42,204),
				       i, this);//GetDataDir(), &PatternMutex);
    }
  SelectedChannel = Channels[0];
  Channels[0]->Select();
  
  
  /* Polyphony */
  Pool = new Polyphony();
  Pool->SetPolyphony(64);
}

WiredBeatBox::~WiredBeatBox()
{
  //channels
  for (int i = 0; i < 12; i++)
    delete Channels[i];
  delete [] Channels;
  
  //patterns
  for (int i = 0; i < 16; i++)
    {
      delete PositionLeds[i];
      delete Beat[i];
      //delete BeatLabels[i];
    }
  delete [] PositionLeds;
  delete [] Beat;
  delete PositionOn;
  delete PositionOff;
//delete [] BeatLabels;
  
  
//  delete [] Signatures;
//  delete [] SigDen;
  
  //polyphony
  delete Pool;
  
  delete PlayButton;
  
  
  for (int i = 0; i < 8; i++)
    delete PatternSelectors[i];
  delete [] PatternSelectors;
  
  delete Imgs[ID_UNCLICKED];
  delete Imgs[ID_MEDIUM];
  delete Imgs[ID_VLOW];
  delete Imgs[ID_LOW];
  delete Imgs[ID_HIGH];
  delete Imgs[ID_VHIGH];
  delete [] Imgs;
  
  delete BgBmp;
  delete Bitmaps[ID_UNCLICKED];
  delete Bitmaps[ID_MEDIUM];
  delete Bitmaps[ID_VLOW];
  delete Bitmaps[ID_LOW];
  delete Bitmaps[ID_HIGH];
  delete Bitmaps[ID_VHIGH];
  delete [] Bitmaps;
  
}

void WiredBeatBox::SetBufferSize(long size)
{
  BufferSize = size;
  Pool->SetBufferSize(size);
}

inline void WiredBeatBox::RefreshPosLeds(double bar_pos)
{
  int i = static_cast<int>(floor(bar_pos * Steps));
  unsigned int cpt;
  
  for (cpt = 0; cpt < 16; cpt++)
    PositionLeds[cpt]->SetBitmap(*PositionOff);
  cpt = 0;
  while (i >= 16)
    {
      cpt++;
      i -= 16;
    }
  if (cpt == PosIndex)
    {
      PositionLeds[i]->SetBitmap(*PositionOn);
    }
}

void WiredBeatBox::Process(float** WXUNUSED(input), float **output, long sample_length)
{
  PatternMutex.Lock();
  

  // try to do this calculation when GetSamplesPerBar() has changed
  SamplesPerBar =
    static_cast<long>
    ((static_cast<double>(GetSamplesPerBar() * StepsSigCoef )
      )
     );
  
  
  double bar_pos = fmod( (GetBarPos() / StepsSigCoef), 1.0 );
  
  double bps = GetBarsPerSample() / StepsSigCoef;
  
  double bars_length =
    static_cast<double>(sample_length * bps);
  
  double bars_end = static_cast<double>(bar_pos + bars_length);
  
  if (Playing)
    { // recuperation des notes a jouer
      //RefreshPosLeds(bar_pos);
      if (SelectedChannel->IsSolo && !SelectedChannel->Muted)
	GetNotesFromChannel(SelectedChannel, bar_pos, bars_end);
      else
	for (int i = 0; i < 12; i++)
	  if ( (Channels[i]->Wave != 0x0) && !Channels[i]->Muted )
	    GetNotesFromChannel(Channels[i], bar_pos, bars_end);
      
    }
  //jouage des notes
  long len = 0;
  long newoffset = 0;
  float curvel, velstep;
  for (list<BeatNoteToPlay*>::iterator bn = NotesToPlay.begin(); 
       bn != NotesToPlay.end();  bn++)
    {
      len = sample_length - (*bn)->Delta;
      
      if ( !Channels[(*bn)->NumChan]->Wave )
	cout << "COMMENT?!!" << endl;
      
      //cout << (*bn)->OffSet << " " << (*bn)->SEnd << endl;
      if ( (*bn)->OffSet >= 
	   Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames() ||
	   (*bn)->OffSet >= (*bn)->SEnd )
	{
	  memset((*bn)->Buffer[0], 0, sample_length * sizeof(float));
	  memset((*bn)->Buffer[1], 0, sample_length * sizeof(float));
	  continue;
	}
      
      
      /* note velocity calculation */
      curvel = Channels[(*bn)->NumChan]->Vel * (*bn)->Vel;
      velstep = static_cast<float>
	( curvel / 
	  static_cast<float>
	  (Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames()) );
      curvel -= (velstep * (*bn)->OffSet);
      
      //printf("velstep=%f, curvel=%f\n", velstep, curvel);
      
      //CLIP(curvel)
      
      long end = 
	( Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames() > (*bn)->SEnd ? 
	  (*bn)->SEnd : Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames());
      if ( (*bn)->OffSet + len > end )
	   //Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames()) 	
	{
	  //printf("finishing sound\n");
	  len = end - (*bn)->OffSet; 
	  if (len <= 0)
	    {
	      cout << " len < 0 !! " << endl;
	      continue;
	    }
	  // cout << "samples to read="<<len << endl;
	  
	  //Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames() - (*bn)->OffSet;
	  
	}
      
      Channels[(*bn)->NumChan]->Wave->SetPitch((*bn)->Pitch);
      Channels[(*bn)->NumChan]->Wave->SetInvert((*bn)->Reversed);
      /*(*bn)->OffSet += */
      Channels[(*bn)->NumChan]->Wave->Read((*bn)->Buffer,
					   (*bn)->OffSet,
					   len,
					   (*bn)->Delta,
					   &newoffset);
      //cout << "offsets=" << (*bn)->OffSet << "-"<< newoffset << endl;
      (*bn)->OffSet = newoffset;
      if ((*bn)->Delta)
	{
	  //Channels[(*bn)->NumChan]->PlayButton->SetOn();
	  (*bn)->Delta = 0;
	}
      for (long i = 0; i < sample_length; i++)
	{
	  (*bn)->Buffer[0][i] *= Channels[(*bn)->NumChan]->Lev * curvel;
	  (*bn)->Buffer[1][i] *= Channels[(*bn)->NumChan]->Lev * curvel;
	  curvel -= velstep;
	  if (curvel < 0.f)
	    {
	      //printf("vel < 0.f     break\n");
	      break;
	    }
	}
    }
  Pool->GetMix(output);
  
  // effacage des notes finies
  for (list<BeatNoteToPlay*>::iterator bn = NotesToPlay.begin(); 
       bn != NotesToPlay.end(); )
    {
      if ( (*bn)->OffSet >= 
	   Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames() )
	{
	  //Channels[(*bn)->NumChan]->PlayButton->SetOff();
	  Pool->SetFreeBuffer((*bn)->Buffer);
	  delete *bn;
	  bn = NotesToPlay.erase(bn);
	  
	}
      else
	bn++;
    }
  
  PatternMutex.Unlock();
}

void WiredBeatBox::ProcessEvent(WiredEvent& event) 
{
  if ((event.MidiData[0] == M_NOTEON1) || (event.MidiData[0] == M_NOTEON2))
    {
      if (!event.MidiData[2]) 
	{
	  PatternMutex.Lock();
	  // Suppression des notes terminées
	  list<BeatNoteToPlay *>::iterator i;
	  for (i = NotesToPlay.begin(); i != NotesToPlay.end(); i++)
	    {  
	      if ((*i)->NoteNum == event.MidiData[1])      
		{
		  Pool->SetFreeBuffer((*i)->Buffer);
		  delete *i;	 
		  NotesToPlay.erase(i);
		  break;
		}
	    } 
	  PatternMutex.Unlock();
	}
      else
	{
	  PatternMutex.Lock();
	  
	  if ((NotesToPlay.size() + 1 > 
	       static_cast<unsigned int>(Pool->GetCount())))
	      Pool->SetPolyphony(NotesToPlay.size()+ 32);
	  int i;
	  for (i = 0; i < 12; i++)
	    {
	      if (ChanMidiNotes[i] == event.MidiData[1])
		{
		  BeatNoteToPlay *n = 
		    new BeatNoteToPlay(event.MidiData[1], 
				       event.MidiData[2] / 100.f,
				       event.DeltaFrames, 
				       Channels[i], Pool->GetFreeBuffer());
		  //event.NoteLength);
		  NotesToPlay.push_back(n);
		  printf("[BEATBOX] Note added: %2x\n", n->NoteNum);
		  break;
		}		
	    }			       
	}	  
      PatternMutex.Unlock();
    }
}

inline void WiredBeatBox::GetNotesFromChannel(BeatBoxChannel* c, 
					      double bar_pos, 
					      double bars_end)
{
  BeatNoteToPlay*	note;
  unsigned long		delta;
  double		lasts;
  
  if (bars_end > 1.0)
    {
      lasts = bars_end - 1.0f;
      SelectedPattern = NewSelectedPattern;
      for (list<BeatNote*>::iterator bn = c->Rythms[SelectedPattern].begin();
	   bn != c->Rythms[SelectedPattern].end(); bn++)
	{
	  if ((*bn)->BarPos > lasts)
	    break;
	  if ( static_cast<unsigned int>(Pool->GetCount())
	       < NotesToPlay.size()+1 )
	    Pool->SetPolyphony(NotesToPlay.size()+32);
	  
	  delta = static_cast<unsigned long>
	    ( ((1.0 - bar_pos + (*bn)->BarPos) * SamplesPerBar) );
	  
	  note = 
	    new BeatNoteToPlay(*bn, c->Id, delta, Pool->GetFreeBuffer());
	  
	  
	  note->Lev *= c->Lev;
	  note->Vel *= c->Vel;
	  note->Pitch *= c->Pitch;
	  note->Start *= c->Start;
	  note->End *= c->End;
	  note->Len *= c->Len;
	  note->Reversed = c->Reversed;
	  
	  note->OffSet =
	    static_cast<unsigned long>
	    (floor(c->Wave->GetNumberOfFrames() * (note->Start)));
	  //cout << note->Start << " offset=" << note->OffSet << endl;

	  note->SEnd = 
	    static_cast<unsigned long>
	    (floor(c->Wave->GetNumberOfFrames() * note->End));
	  
	  
	  NotesToPlay.push_back(note);
	}
      
    }
  for (list<BeatNote*>::iterator bn = c->Rythms[SelectedPattern].begin();
       bn != c->Rythms[SelectedPattern].end(); bn++)
    {
      if ((*bn)->BarPos >= 1.0)
	{
	  break;
	}
      else if ((*bn)->BarPos >= bar_pos && (*bn)->BarPos < bars_end)
	{
	  if ( static_cast<unsigned int>(Pool->GetCount())
	       < NotesToPlay.size()+1 )
	    Pool->SetPolyphony(NotesToPlay.size()+32);
	  
	  delta = static_cast<unsigned long>
	    ( (((*bn)->BarPos - bar_pos) * SamplesPerBar) );
	  
	  note = 
	    new BeatNoteToPlay(*bn, c->Id, delta, Pool->GetFreeBuffer());
	  
	  
	  note->Lev *= c->Lev;
	  note->Vel *= c->Vel;
	  note->Pitch *= c->Pitch;
	  note->Start *= c->Start;
	  note->End *= c->End;
	  note->Len *= c->Len;
	  note->Reversed |= c->Reversed;
	  
	  note->OffSet =
	    static_cast<unsigned long>
	    (floor(c->Wave->GetNumberOfFrames() * (note->Start)));
	  note->SEnd = 
	    static_cast<unsigned long>(floor(c->Wave->GetNumberOfFrames()*note->End));
	  //cout << note->Start << " offset=" << note->OffSet << endl;

	  NotesToPlay.push_back(note);
	}
    }
}

inline void WiredBeatBox::UpdateNotesPositions(void)
{
  for (unsigned char i = 0; i < 12; i++)
    for (unsigned char ps = 0; ps < 8; ps++)
      for ( list<BeatNote*>::iterator bn = Channels[i]->Rythms[ps].begin();
	    bn != Channels[i]->Rythms[ps].end(); bn++ )
	{
	  (*bn)->BarPos = 
	    static_cast<double>((*bn)->Position / static_cast<float>(Steps));
	}
}


void WiredBeatBox::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxMemoryDC memDC;
  wxPaintDC dc(this);
  
  memDC.SelectObject(*BgBmp);    
  wxRegionIterator upd(GetUpdateRegion());
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, 
	      upd.GetX(), upd.GetY(), wxCOPY, FALSE);      
      upd++;
    }
}


//bool WiredBeatBox::HasView() { return false; }
/*
wxWindow *CreateView(wxWindow *zone, wxPoint &pos, wxSize &size) 
    { return 0x0; } 
*/

/*
void WiredBeatBox::OnKeyEvent(wxKeyEvent& WXUNUSED(e))
{
}

void WiredBeatBox::OnMouseWheelEvent(wxMouseEvent& WXUNUSED(e))
{

}
*/

void	WiredBeatBox::OnMasterChange(wxScrollEvent& WXUNUSED(event))
{
  float mlevel = static_cast<float>(MVol->GetValue()/100.0f);
  PatternMutex.Lock();
  MLevel = mlevel;
  Pool->SetVolume(MLevel);
  PatternMutex.Unlock();
}

void WiredBeatBox::OnSigChoice(wxCommandEvent& e)
{
  unsigned int *i = static_cast<unsigned int*>(e.GetClientData());
  
  SignatureButtons[SigIndex]->SetOff();
  SignatureButtons[*i]->SetOn();
  PatternMutex.Lock();
  SigIndex = *i;
  SignatureDen = SigDen[*i];
  Signature = Signatures[*i];
  
  StepsSigCoef = static_cast<double>
    ( Steps / static_cast<double>(SignatureDen));

  PatternMutex.Unlock();
}

void WiredBeatBox::OnPositionChoice(wxCommandEvent& e)
{
  unsigned int *i = static_cast<unsigned int*>(e.GetClientData());
  PositionButtons[PosIndex]->SetOff();
  PositionButtons[*i]->SetOn();
  PatternMutex.Lock();
  PosIndex = *i;
  SetPatternList();
  PatternMutex.Unlock();
}

void WiredBeatBox::OnToggleChannel(wxCommandEvent& e)  
{
  unsigned int *data = (unsigned int*)e.GetClientData();
  BeatBoxChannel* tmp = Channels[data[0]];
  BeatNoteToPlay* bn;
  
  PatternMutex.Lock();
  
  switch (data[1])
    {
    case ACT_SELECT:
      for (unsigned char i = 0; i < 12; i++)
	Channels[i]->DeSelect();
      tmp->Select();
      SelectedChannel = tmp;
      SetPatternList();
      /*
	if (View && SelectedChannel->Wave)
	{
	  View->SetWaveFile(SelectedChannel->Wave);
	}
      */
      break;
    case ACT_SOLO:
      for (unsigned char i = 0; i < 12; i++)
	{
	  Channels[i]->DeSelect();
	  Channels[i]->UnSolo();
	}
      tmp->Select();
      tmp->Solo();
      SelectedChannel = tmp;
      SetPatternList();
      break;
    case ACT_PLAY:
      if (!tmp->Wave)
	{break;}
      if ( static_cast<unsigned int>(Pool->GetCount())
	   < NotesToPlay.size()+1 )
	Pool->SetPolyphony(NotesToPlay.size()+32);
      bn = new BeatNoteToPlay(tmp, Pool->GetFreeBuffer());
      NotesToPlay.push_back(bn);
      break;
    case ACT_SETWAVE:
      
      break;
    default:
      break;
    }
  PatternMutex.Unlock();
}

void WiredBeatBox::OnPatternSelectors(wxCommandEvent& e)
{
  unsigned int *p = (unsigned int*)e.GetClientData();
  
  PatternMutex.Lock();
  if (NewSelectedPattern != *p && !EditButton->GetOn())
    {
      PatternSelectors[SelectedPattern]->SetOff();
      PatternSelectors[NewSelectedPattern]->SetOff();
      //SelectedPattern = *p;
      NewSelectedPattern = *p;
      PatternSelectors[NewSelectedPattern]->SetOn();
    }
  SetPatternList();
  PatternMutex.Unlock();
}

void WiredBeatBox::SetPatternList(void)
{
  double	i, j;
  int		index;
  
  for (index = 0; index < 16; index++)
    Beat[index]->SetState(ID_UNCLICKED);
  
  i = static_cast<double>(PosIndex * 16);
  j = static_cast<double>(i + 16.0);
  
  for (list<BeatNote*>::iterator bn = 
	 SelectedChannel->Rythms[NewSelectedPattern].begin();
       bn != SelectedChannel->Rythms[NewSelectedPattern].end(); bn++)
    {
      if ( (*bn)->Position > j )
	break;
      else if ( (*bn)->Position >= i )
	{
	  if ( floor((*bn)->Position) == (*bn)->Position )
	    {
	      index = static_cast<int>((*bn)->Position);
	      Beat[index % 16]->SetState((*bn)->State);
	    }
	}
    }
}

void WiredBeatBox::OnPatternClick(wxCommandEvent &e)
{
  char* tmp = (char*)e.GetClientData();
  
  BeatBoxChannel* c = SelectedChannel;
  
  int i = PosIndex * 16;
  
  double bar_pos = 
    static_cast<float>( (tmp[ID_POS] + i) / static_cast<float>(Steps) );
  
  if (c->Rythms[NewSelectedPattern].empty())
    {
      BeatNote *note = new BeatNote(static_cast<double>(tmp[ID_POS] + i), 
				    tmp[ID_STATE], bar_pos);
      
      
      
      note->Vel = 1.0f * static_cast<float>(tmp[ID_STATE] / 4.f);
      note->Reversed = false;
      
      PatternMutex.Lock();
      c->Rythms[NewSelectedPattern].push_back(note);
      PatternMutex.Unlock();
      //cout << "pattern added" << endl;
      return;
    }
  if ( bar_pos > c->Rythms[NewSelectedPattern].back()->BarPos ) 
    {
      BeatNote *note = new BeatNote( static_cast<double>(tmp[ID_POS] + i),
				     tmp[ID_STATE], bar_pos);
      note->Vel = 1.0f * static_cast<float>(tmp[ID_STATE] / 4.f);
      note->Reversed = false;
      
      PatternMutex.Lock();
      c->Rythms[NewSelectedPattern].push_back(note);
      PatternMutex.Unlock();
      //cout << "pattern added" << endl;
      return;
    }
  //  else
  for ( list<BeatNote*>::iterator b = c->Rythms[NewSelectedPattern].begin();
	b != c->Rythms[NewSelectedPattern].end(); b++)
    {  
      if ( (*b)->BarPos == bar_pos ) 
	{
	  if ( tmp[ID_STATE] == ID_UNCLICKED )
	    {
	      PatternMutex.Lock();
	      delete *b;
	      c->Rythms[NewSelectedPattern].erase(b);
	      //cout << "pattern erased" << endl;
	      PatternMutex.Unlock();
	      break;
	    }
	  else
	    (*b)->Vel = 1.f * static_cast<float>(tmp[ID_STATE] / 4.f);
	  break;
	}
      else if ( (*b)->BarPos > bar_pos ) 
	{
	  BeatNote *note = new BeatNote(static_cast<double>(tmp[ID_POS] + i),
					tmp[ID_STATE], bar_pos);
	  note->Vel = 1.0f * static_cast<float>(tmp[ID_STATE] / 4.f);
	  note->Reversed = false;
	  PatternMutex.Lock();
	  c->Rythms[NewSelectedPattern].insert(b, note);
	  PatternMutex.Unlock();
	  //cout << "pattern added" << endl;
	  break;
	}
    }
}

void WiredBeatBox::ShowOpt(wxCommandEvent& WXUNUSED(e))
{
  ShowOptionalView();
}

void WiredBeatBox::OnSavePatch(wxCommandEvent& WXUNUSED(e))
{
  vector<string> exts;
  exts.push_back("drm\tDRM-31 patch file (*.drm)");
  cout << "OnSavePatch(): begin" << endl;
  
  string selfile = OpenFileLoader("Save Patch", &exts);
  if (!selfile.empty())
    {
      int fd = open(selfile.c_str(),  O_CREAT | O_TRUNC | O_WRONLY, 
		    S_IRUSR | S_IWUSR);
      //	    S_IRUSR | S_IWUSR );
      /*      if (fd <= 2)
	{
	  cout << "Couldnt open( "<<selfile << " )" << endl;
	  //close(fd);
	  return;
	}
      */
      cout << "open " << selfile << " fd: "<< fd << endl;
      /* wxProgressDialog *Progress = 
	new wxProgressDialog("Saving patch file", "Please wait...", 
			     100, this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT
			     | wxPD_REMAINING_TIME);
      Progress->Update(1);
      Progress->Update(55);
      delete Progress;
*/
      cout << "saved " << Save(fd) << " byte(s)"<< endl;
      
      close(fd);
      
    }
  else
    cout << "[DRM31] could not open save file" << endl;
  cout << "OnSavePatch(): end" << endl;
  //delete dlg;
}

void WiredBeatBox::OnLoadPatch(wxCommandEvent& WXUNUSED(e))
{
  vector<string> exts;
  exts.push_back("drm\tDRM-31 patch file (*.drm)");
  
  cout << "OnLoadPatch: begin" << endl;
  string selfile = OpenFileLoader("Load Patch", &exts);
  
  if (!selfile.empty())
    {
      int fd = open(selfile.c_str(), O_RDONLY);
      if (fd < 0)
	{
	  cout << "OnLoadPatch: Couldnt open( " << selfile << " )" << endl;
	  return;
	}
      PatternMutex.Lock();
      for (int chan = 0; chan < 12; chan++)
	DELETE_RYTHMS(Channels[chan]->Rythms)
      PatternMutex.Unlock();


      cout << "OnLoadPatch: open( " << selfile << " ) fd: " << fd << endl;
      
      struct stat st;
      fstat(fd, &st);
      cout << "file length: " << st.st_size << endl;
      
      wxProgressDialog *Progress = 
	new wxProgressDialog("Loading patch file", "Please wait...", 
			     100, this, wxPD_AUTO_HIDE | wxPD_CAN_ABORT
			     | wxPD_REMAINING_TIME);
      Progress->Update(1);
      Progress->Update(55);
      Load(fd, st.st_size);
      Progress->Update(75);
      Progress->Update(100);
      
      close(fd);
      
      delete Progress;
    }
  else
    cout << "[DRM31] Could not load file" << endl;
  cout << "OnLoadPatch: end" << endl;
}

long WiredBeatBox::Save(int fd)
{
  long len, size = 0;
  
  PatternMutex.Lock();
  
  cout << "saving on fd = " << fd << endl;
  
  int sel = SigIndex;//(SignatureChoice->GetSelection());
  size += write(fd, &sel, sizeof(int));
  size += write(fd, &Steps, sizeof(int));
  
  cout << "sigindex=" << sel <<" steps=" << Steps << endl;
  
  for (int i = 0; i < 12; i++)
    {
      if (Channels[i]->Wave)
	{
	  size += write(fd, &i, sizeof(int));
	  len = Channels[i]->Wave->Filename.size();
	  size += write(fd, &len, sizeof(long));
	  size += write(fd, Channels[i]->Wave->Filename.c_str(), len);
	  cout << "writing channel: " << i 
	       << " file len: " << len << " name: " 
	       << Channels[i]->Wave->Filename.c_str() << endl;
	  
	  for (int ps = 0; ps < 8; ps++)
	    {
	      // size += write(fd, &ps, sizeof(unsigned char));
	      len = Channels[i]->Rythms[ps].size();
	      size += write(fd, &len, sizeof(long));
	      cout << "writing "<<len << " notes for rythm["<< ps
		   << "]" << endl;
	      for (list<BeatNote*>::iterator bn = 
		     Channels[i]->Rythms[ps].begin();
		   bn != Channels[i]->Rythms[ps].end(); bn++)
		{
		  //cout << "note pos: " << (*bn)->Position;
		  printf("note pos=%f; state=%d\n", 
			 (*bn)->Position,(*bn)->State);
		  size += write(fd, &((*bn)->State), sizeof(unsigned char));
		  size += 
		    write(fd, &((*bn)->Position), sizeof (double));
		}
	    }
	}
    }
  PatternMutex.Unlock();
  cout << "saved size " << size << endl;
  return size;
}

void WiredBeatBox::Load(int fd, long size)
{
  int ch;
  long len;
  unsigned char ps;
  unsigned char state = 0;
  double pos = 0;
  BeatNote* note;
  WaveFile* w;
  
  int sig, steps;
  
  
  if (fd < 0)
    cout << "loading fd = "<< fd << ", blem, file size: " << size << endl;
  cout << "loading fd = "<< fd << endl;
  //  PatternMutex.Lock();
  
  size -= read(fd, &sig, sizeof(int));
  size -= read(fd, &steps, sizeof(int));
  cout << "sig="<<sig<< " steps="<<steps << endl;
  //PatternMutex.Unlock();
  
  while (size)
    {
      size -= read(fd, &ch, sizeof(int));
      cout << "load: channel " << ch << endl;
      size -= read(fd, &len, sizeof(long));
      char wave[len+1];
      size -= read(fd, wave, len);
      wave[len] = '\0';
      printf("file len: %d, name: %s\n", len, wave);
      w = new WaveFile(string(wave), true);
      if (!w)
	cout << "error loading wave"<< endl;
      else
	Channels[ch]->SetWaveFile(w);
      
      PatternMutex.Lock();
      for (ps = 0; ps < 8; ps++)
	{
	  size -= read(fd, &len, sizeof(long));
	  //printf("list %d, notes: %d\n", ps,len);
          for (long i = 0; i < len; i++)
            {
	      size -= read(fd, &state, sizeof(unsigned char));
              size -= read(fd, &pos, sizeof(double));
	      printf("note pos=%f; state=%d\n", 
		     pos,state);
	      note = 
		new BeatNote(pos, state, 
			     static_cast<double>
			     ( pos / 
			       static_cast<double>
			       (steps)));
	      Channels[ch]->Rythms[ps].push_back(note);
	      
	      //printf("note %d %d %d\n", state, pos, spos);
	    }
	}
       PatternMutex.Unlock();
       
      //WaveFile* w_toerase;
      //w_toerase = Channels[ch]->Wave;
      //Channels[ch]->Reset();
    }
  
  Steps = steps;
  SignatureDen = SigDen[sig];
  Signature = Signatures[sig];
  
  SignatureButtons[SigIndex]->SetOff();
  SigIndex = sig;
  SignatureButtons[sig]->SetOn();
  //SignatureChoice->SetSelection(sig);
  
  SelectedChannel = Channels[0];
  SetPatternList();
  //  PatternMutex.Unlock();
  //printf("size:%d\n", size);
}

void WiredBeatBox::OnEdit(wxCommandEvent& WXUNUSED(e))
{
  
}

void WiredBeatBox::OnStepsUp(wxCommandEvent& WXUNUSED(event))
{
  PatternMutex.Lock();
  Steps++;
  if (Steps > 64)
    Steps = 64;
  wxString s;
  s.Printf("%d", Steps);
  StepsLabel->SetLabel(s);
  
  StepsSigCoef = static_cast<double>
    ( Steps / static_cast<double>(SignatureDen));
  
  UpdateNotesPositions();
  PatternMutex.Unlock();
}

void WiredBeatBox::OnStepsDown(wxCommandEvent& WXUNUSED(event))
{
  PatternMutex.Lock();
  Steps--;
  if (Steps < 1)
    Steps = 1;
  wxString s;
  s.Printf("%d", Steps);
  StepsLabel->SetLabel(s);
  
  StepsSigCoef = static_cast<double>
    ( Steps / static_cast<double>(SignatureDen));
  
  UpdateNotesPositions();
  PatternMutex.Unlock();
}

void WiredBeatBox::OnPlay(wxCommandEvent& WXUNUSED(e))
{
  PatternMutex.Lock();
  AutoPlay = !AutoPlay;
  if (SeqPlaying == true && AutoPlay == true)
    Playing = true;
  else 
    Playing = false;
  PatternMutex.Unlock();
}

void WiredBeatBox::Play()
{
  SeqPlaying = true;
  PatternMutex.Lock();
  if (AutoPlay == true)
    Playing = true;
  PatternMutex.Unlock();
}

void WiredBeatBox::Stop()
{
  PatternMutex.Lock();
  Playing = false;
  SeqPlaying = false;
  PatternMutex.Unlock();
}

wxWindow* WiredBeatBox::CreateView(wxWindow* zone, wxPoint& pos, wxSize& size)
{
  View = new BeatBoxView(zone, this, pos, size, &PatternMutex);
  return View;
}

void WiredBeatBox::DestroyView()
{
  View->Destroy();
  View = 0x0;
}

int WiredBeatBox::GetSig()
{
  int s;
  PatternMutex.Lock();
  s = GetSigNumerator();
  PatternMutex.Unlock();
  return s;
}

/******** Main and mandatory library functions *********/

extern "C"
{
  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "WBBE");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_INSTR;  
    info.UnitsX = 3;
    info.UnitsY = 3;
    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new WiredBeatBox(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }
}
