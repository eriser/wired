#include "BeatBox.h"
#include <wx/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <errno.h>
#include "midi.h"


static PlugInitInfo info;

inline void CalcPan(float pan, float* panvals)
{
  float tmp;
  if (pan < 0.5)
    {
      panvals[0] = 1.0f;
      tmp = pan * 2;
      panvals[1] = tmp;
    }
  else if (pan > 0.5)
    {
      panvals[1] = 1.0f;
      tmp = (1.0 - pan) * 2;
      panvals[0] = tmp;
    }
  else
    {
      panvals[0] = 1.0f;
      panvals[1] = 1.0f;
    }
}

BEGIN_EVENT_TABLE(WiredBeatBox, wxWindow)
  EVT_PAINT(WiredBeatBox::OnPaint)
  EVT_COMMAND_SCROLL(BB_OnMasterChange, WiredBeatBox::OnMasterChange)
  EVT_BUTTON(BB_OnStepsChange, WiredBeatBox::OnStepsChange)
  EVT_BUTTON(BB_OnBankChange, WiredBeatBox::OnBankChange)
  EVT_BUTTON(BB_PatternClick, WiredBeatBox::OnPatternClick)
  EVT_BUTTON(BB_ShowOpt, WiredBeatBox::ShowOpt)
  EVT_BUTTON(BB_OnPlayClick, WiredBeatBox::OnPlay)
  EVT_BUTTON(BB_OnEditClick, WiredBeatBox::OnEditButton)
  EVT_BUTTON(BB_Channel, WiredBeatBox::OnToggleChannel)
  EVT_BUTTON(BB_OnLoadPatch, WiredBeatBox::OnLoadPatch)
  EVT_BUTTON(BB_OnSavePatch, WiredBeatBox::OnSavePatch)
  EVT_BUTTON(BB_OnPatternSelectors, WiredBeatBox::OnPatternSelectors)
  EVT_BUTTON(BB_OnSigChoice, WiredBeatBox::OnSigChoice)
  EVT_BUTTON(BB_OnPosChoice, WiredBeatBox::OnPositionChoice)
  EVT_RIGHT_DOWN(WiredBeatBox::OnRightDown)
  EVT_ENTER_WINDOW(WiredBeatBox::OnHelp)
END_EVENT_TABLE()
  

WiredBeatBox::WiredBeatBox(PlugStartInfo &startinfo, PlugInitInfo *initinfo) 
  : Plugin(startinfo, initinfo)
{
  cout << "[DRM31] Host is " << GetHostProductName()
       << " version " << GetHostProductVersion() << endl;
  
  OnLoading = false;
  
  AskUpdateSteps = false;
  AskUpdatePattern = false;
  AskUpdateBank = false;
  AskUpdateLevel = false;
  
  /* Popup Menu */
  
  PopMenu = new wxMenu();
  wxMenu* bank_menu = new wxMenu();
  wxMenu* pattern_menu = new wxMenu();
  
  PopMenu->Append(BB_PopMenu, _T("Copy pattern to"), bank_menu);
  
  bank_menu->Append(BB_BankCopy, _T("bank 1"), pattern_menu);
  bank_menu->Append(BB_BankCopy, _T("bank 2"), pattern_menu);
  bank_menu->Append(BB_BankCopy, _T("bank 3"), pattern_menu);
  bank_menu->Append(BB_BankCopy, _T("bank 4"), pattern_menu);
  bank_menu->Append(BB_BankCopy, _T("bank 5"), pattern_menu);
  pattern_menu->Append(BB_PatternCopy, _T("pattern 1"));
  pattern_menu->Append(BB_PatternCopy, _T("pattern 2"));
  pattern_menu->Append(BB_PatternCopy, _T("pattern 3"));
  pattern_menu->Append(BB_PatternCopy, _T("pattern 4"));
  pattern_menu->Append(BB_PatternCopy, _T("pattern 5"));
  pattern_menu->Append(BB_PatternCopy, _T("pattern 6"));
  pattern_menu->Append(BB_PatternCopy, _T("pattern 7"));
  pattern_menu->Append(BB_PatternCopy, _T("pattern 8"));
  
  Connect(BB_PatternCopy, wxEVT_COMMAND_MENU_SELECTED,
	  (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)
	  &WiredBeatBox::OnCopyPattern);
  
  
  /* Master Volume */
  wxImage* mini_bmp = 
    new wxImage(_T(string(GetDataDir() + string(BEATBOX_MINI_BG)).c_str()), 
		wxBITMAP_TYPE_BMP);  
  if (mini_bmp)
    MiniBmp = new wxBitmap(mini_bmp);
  
  delete mini_bmp;
  
  MVol = 
    new KnobCtrl(this, BB_OnMasterChange,
		 new wxImage(_T(string(GetDataDir() + string(KNOB)).c_str()),
			     wxBITMAP_TYPE_PNG),
		 new wxImage(_T(string(GetDataDir() + string(DOT)).c_str()),
			     wxBITMAP_TYPE_PNG),
		 0, 127, 100, 1,
		 wxPoint(27,103), wxSize(29,30),
		 this, wxPoint(0,0));
  MVol->SetValue(100);
  MLevel = 1.0f;
  View = 0x0;

  /* Bitmaps */
  SetBackgroundColour(wxColour(200, 200, 200));
  wxImage* img_bg = 
    new wxImage(_T(string(GetDataDir() + string(BEATBOX_BG)).c_str()), 
		wxBITMAP_TYPE_PNG);  
  if (img_bg)
    BgBmp = new wxBitmap(img_bg);
  
  delete img_bg;
  
  Imgs = new wxImage*[MAX_BITMAPS];
  Imgs[ID_UNCLICKED] = 
    new wxImage(_T(string(GetDataDir() + string(BEATBTN_UNCLICKED)).c_str()), 
		wxBITMAP_TYPE_PNG);
  Imgs[ID_MEDIUM] = 
    new wxImage(_T(string(GetDataDir() + string(BEATBTN_MEDIUM)).c_str()), 
		wxBITMAP_TYPE_PNG);
  Imgs[ID_VLOW] = 
    new wxImage(_T(string(GetDataDir() + string(BEATBTN_VLOW)).c_str()), 
		wxBITMAP_TYPE_PNG);
  Imgs[ID_LOW] =
    new wxImage(_T(string(GetDataDir() + string(BEATBTN_LOW)).c_str()), 
		wxBITMAP_TYPE_PNG);
  Imgs[ID_HIGH] = 
    new wxImage(_T(string(GetDataDir() + string(BEATBTN_HIGH)).c_str()), 
		wxBITMAP_TYPE_PNG);
  Imgs[ID_VHIGH] = 
    new wxImage(_T(string(GetDataDir() + string(BEATBTN_VHIGH)).c_str()), 
		wxBITMAP_TYPE_PNG);
  
  
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
    new wxImage(_T(string(GetDataDir() + string(POSON)).c_str()), 
		wxBITMAP_TYPE_PNG);
  if (img_tmp)
    PositionOn = new wxBitmap(img_tmp);
  img_tmp = 
    new wxImage(_T(string(GetDataDir() + string(POSOFF)).c_str()), 
		wxBITMAP_TYPE_PNG);
  if (img_tmp)
    PositionOff = new wxBitmap(img_tmp);
  
  
  /* general   */
  SetBackgroundColour(wxColour(200, 200, 200));
  
  /* Optional view button */
  wxImage *opt_img_up = new wxImage(_T(string(GetDataDir() + string(SHOWOPT_UP)).c_str()), wxBITMAP_TYPE_PNG);
  wxImage *opt_img_dn = new wxImage(_T(string(GetDataDir() + string(SHOWOPT_DOWN)).c_str()), wxBITMAP_TYPE_PNG);
  
  if (opt_img_up && opt_img_dn)
    OptViewBtn = new DownButton(this, BB_ShowOpt, wxPoint(769, 322), 
				wxSize(14, 17), opt_img_up, opt_img_dn, true);
  
  /* Play Button */
  Playing = false;
  AutoPlay = false;
  SeqPlaying = false;
  wxImage* play_up = new wxImage(_T(string(GetDataDir() + string(PLAY_UP)).c_str()), wxBITMAP_TYPE_PNG);
  wxImage* play_down = new wxImage(_T(string(GetDataDir() + string(PLAY_DO)).c_str()), wxBITMAP_TYPE_PNG);
  if (play_up && play_down)
    PlayButton = 
      new DownButton(this, BB_OnPlayClick, wxPoint(8,259), 
		     wxSize(22,31), play_up, play_down, false);
  
  /* Edit Button */
  wxImage* edit_up = new wxImage(_T(string(GetDataDir() + string(EDIT_UP)).c_str()), wxBITMAP_TYPE_PNG);
  wxImage* edit_down = new wxImage(_T(string(GetDataDir() + string(EDIT_DO)).c_str()), wxBITMAP_TYPE_PNG);
  if (edit_up && edit_down)
    EditButton = 
      new DownButton(this, BB_OnEditClick, wxPoint(15,231), 
		     wxSize(15,25), edit_up, edit_down, false);
  
  /* Load/Save Patchs/Patterns */
  wxImage* tmp_img = new wxImage(_T(string(GetDataDir() + string(SAVEPATCH_UP)).c_str()));
  wxImage* tmp_img2 = 
    new wxImage(_T(string(GetDataDir() + string(SAVEPATCH_DO)).c_str()));
  
  DownButton* db;
  if (tmp_img && tmp_img2)
    db = new DownButton(this, BB_OnSavePatch, wxPoint(23, 168), wxSize(17,18),
			tmp_img, tmp_img2, true);
  
  tmp_img = new wxImage(_T(string(GetDataDir() + string(LOADPATCH_UP)).c_str()));
  tmp_img2 = new wxImage(_T(string(GetDataDir() + string(LOADPATCH_DO)).c_str()));
  if (tmp_img && tmp_img2)
    db = 
      new DownButton(this, BB_OnLoadPatch, wxPoint(23, 149), wxSize(17,18), 
		     tmp_img, tmp_img2, true);
  
  wxStaticText* PatchLabel =
    new wxStaticText(this, -1, _T("p31kit"), wxPoint(24,190), wxSize(30, 10), 
		     wxALIGN_RIGHT);
  
  PatchLabel->SetFont(wxFont(8, wxDEFAULT, wxNORMAL, wxNORMAL));
  PatchLabel->SetForegroundColour(*wxBLACK);
  PatchLabel->Show();
  
  /* Signature params */
  
  StepsLabel = new wxStaticText(this, -1, _T("16"), wxPoint(674, 360), 
				wxSize(18,15), wxALIGN_RIGHT);
  StepsLabel->SetFont(wxFont(12, wxBOLD, wxBOLD, wxBOLD));
  StepsLabel->SetForegroundColour(*wxWHITE);
  
  Signatures[0] = (float)((float)1/(float)4);
  Signatures[1] = (float)((float)1/(float)8);
  Signatures[2] = (float)((float)1/(float)16);
  Signatures[3] = (float)((float)1/(float)32);
  Signatures[4] = (float)((float)1/(float)64);
  
  SigDen[0] = 4;
  SigDen[1] = 8;
  SigDen[2] = 16;
  SigDen[3] = 32;
  SigDen[4] = 64;
    
  OldSamplesPerBar = GetSamplesPerBar();
  OldBarsPerSample = GetBarsPerSample();
  
  float default_sig = (float)((float)1/(float)16);
  double default_stepssig_coef = static_cast<double>
    ( 16.0 / static_cast<double>(16.0) );
  long default_spb =
    static_cast<long>
    ((static_cast<double>(OldSamplesPerBar * default_stepssig_coef)));
  double default_bps = OldBarsPerSample / default_stepssig_coef;
  
  for (int b = 0; b < NUM_BANKS; b++)
    for (int p = 0; p < NUM_PATTERNS; p++)
      {
	SignatureDen[b][p] = 16;
	Steps[b][p] = 16;
	Signature[b][p] = default_sig;
	StepsSigCoef[b][p] = default_stepssig_coef;
	SamplesPerBar[b][p] = default_spb;
	BarsPerSample[b][p] = default_bps;
	SigIndex[b][p] = 2;
      }
  
  
  SignatureButtons = new IdButton*[5];
  PositionButtons = new IdButton*[4];
  
  wxImage* posup = new wxImage(_T(string(GetDataDir() + string(POS_UP)).c_str()));
  wxImage* posdown = new wxImage(_T(string(GetDataDir() + string(POS_DO)).c_str()));
  
  if (posup && posdown) {
    SignatureButtons[0] = 
      new IdButton(this, BB_OnSigChoice, wxPoint(423, 310), wxSize(15,12),
		   posup, posdown, 0);
    SignatureButtons[1] = 
      new IdButton(this, BB_OnSigChoice, wxPoint(423, 321), wxSize(15,12),
		   posup, posdown, 1);
    SignatureButtons[2] = 
      new IdButton(this, BB_OnSigChoice, wxPoint(485, 310), wxSize(15,12),
		   posup, posdown, 2);
    SignatureButtons[3] = 
      new IdButton(this, BB_OnSigChoice, wxPoint(485, 321), wxSize(15,12),
		   posup, posdown, 3);
    SignatureButtons[4] = 
      new IdButton(this, BB_OnSigChoice, wxPoint(557, 310), wxSize(15,12),
		   posup, posdown, 4);
    
    SignatureButtons[2]->SetOn();
    
    PositionButtons[0] = 
      new IdButton(this, BB_OnPosChoice, wxPoint(215, 310), wxSize(15,12),
		   posup, posdown, 0);
    PositionButtons[1] = 
      new IdButton(this, BB_OnPosChoice, wxPoint(215, 321), wxSize(15,12),
		   posup, posdown, 1);
    PositionButtons[2] = 
      new IdButton(this, BB_OnPosChoice, wxPoint(266, 310), wxSize(15,12),
		   posup, posdown, 2);
    PositionButtons[3] = 
      new IdButton(this, BB_OnPosChoice, wxPoint(266, 321), wxSize(15,12),
		   posup, posdown, 3);
    PosIndex = 0;
    PositionButtons[PosIndex]->SetOn();
  }
    
  
  /* Pattern Selectors */
  EditedPattern = NewSelectedPattern = SelectedPattern = 0;
  EditedBank = NewSelectedBank = SelectedBank = 0;
 
  OnEdit = false;

  PatternSelectors = new IdButton*[8];
  tmp_img = new wxImage(_T(string(GetDataDir() + string(UP_1)).c_str()));
  tmp_img2 = new wxImage(_T(string(GetDataDir() + string(DO_1)).c_str()));
  if (tmp_img && tmp_img2)
    PatternSelectors[0] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(33, 230), wxSize(12,13),
		    tmp_img, tmp_img2, 0 );
  

  tmp_img = new wxImage(_T(string(GetDataDir() + string(UP_2)).c_str()));
  tmp_img2 = new wxImage(_T(string(GetDataDir() + string(DO_2)).c_str()));
  if (tmp_img && tmp_img2)
    PatternSelectors[1] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(48, 230), wxSize(12,13),
		    tmp_img, tmp_img2, 1 );
  tmp_img = new wxImage(_T(string(GetDataDir() + string(UP_3)).c_str()));
  tmp_img2 = new wxImage(_T(string(GetDataDir() + string(DO_3)).c_str()));
  if (tmp_img && tmp_img2)
    PatternSelectors[2] =
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(33, 245), wxSize(12,13),
		    tmp_img, tmp_img2, 2 );
  
  tmp_img = new wxImage(_T(string(GetDataDir() + string(UP_4)).c_str()));
  tmp_img2 = new wxImage(_T(string(GetDataDir() + string(DO_4)).c_str()));
  if (tmp_img && tmp_img2)
    PatternSelectors[3] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(48, 245), wxSize(12,13),
		    tmp_img, tmp_img2, 3 );
  
  tmp_img = new wxImage(_T(string(GetDataDir() + string(UP_5)).c_str()));
  tmp_img2 = new wxImage(_T(string(GetDataDir() + string(DO_5)).c_str()));
  if (tmp_img && tmp_img2)
    PatternSelectors[4] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(33, 260), wxSize(12,13),
		    tmp_img, tmp_img2, 4 );
  
  tmp_img = new wxImage(_T(string(GetDataDir() + string(UP_6)).c_str()));
  tmp_img2 = new wxImage(_T(string(GetDataDir() + string(DO_6)).c_str()));
  if (tmp_img && tmp_img2)
    PatternSelectors[5] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(48, 260), wxSize(12,13),
		    tmp_img, tmp_img2, 5 );
  
  tmp_img = new wxImage(_T(string(GetDataDir() + string(UP_7)).c_str()));
  tmp_img2 = new wxImage(_T(string(GetDataDir() + string(DO_7)).c_str()));
  if (tmp_img && tmp_img2)
  PatternSelectors[6] = 
    new IdButton( this, BB_OnPatternSelectors,
		  wxPoint(33, 275), wxSize(12,13),
		  tmp_img, tmp_img2, 6 );
  
  tmp_img = new wxImage(_T(string(GetDataDir() + string(UP_8)).c_str()));
  tmp_img2 = new wxImage(_T(string(GetDataDir() + string(DO_8)).c_str()));
  if (tmp_img && tmp_img2)
    PatternSelectors[7] = 
      new IdButton( this, BB_OnPatternSelectors,
		    wxPoint(48, 275), wxSize(12,13),
		    tmp_img, tmp_img2, 7 );
  
  PatternSelectors[SelectedPattern]->SetOn();
  
  /* Pattern list */
  Beat = new BeatButton*[16];
  
  PositionLeds = new StaticBitmap*[16];
  
  for (int i = 0, x = 144; i < 16; i++)
    {
      
      if (i % 4)
	x += BTN_SIZE + 2;
      else if (i != 0)
	x += BTN_SIZE + 10;
      
      LedsOn[i] = false;
      PositionLeds[i] = 
	new StaticBitmap(this, -1, *PositionOff, 
			 wxPoint(x+BTN_SIZE/2, 342), wxSize(6,6));
      Beat[i] = new BeatButton( this, BB_PatternClick, 
				wxPoint(x,350), wxSize(BTN_SIZE,BTN_SIZE), 
				Bitmaps, i );
    }
  
  /* Event Handling for dragging on patterns */
  Connect(BEATBUTTON_ID, EVT_MOTION_OUT,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxCommandEventFunction)&WiredBeatBox::OnPatternMotion);
  
  /* Channels */
  Channels = new BeatBoxChannel*[NB_CHAN];
    
  int notenum = 48; // C4
  //int notenum = 0x48; //C3 (maybe C4?)
  
  for (unsigned int i = 0; i < NB_CHAN; i++)
    {
      ChanMidiNotes[i] = notenum++;
      if (i == 10)
	Channels[i] = new BeatBoxChannel(this, BB_Channel, 
					 wxPoint(i*65 + 80, 13), 
					 wxSize(50,276),
					 i, this);
      else
	Channels[i] = new BeatBoxChannel(this, BB_Channel, 
					 wxPoint(i*65 + 74, 13), 
					 wxSize(50,276),
					 i, this);
      
      VoicesCount[i] = 0;
      //Channels[i]->Voices = 8;
    }
  SelectedChannel = Channels[0];
  Channels[0]->Select();
  
  /* Polyphony */
  
  Pool = new Polyphony();
  Pool->SetPolyphony(8 * NB_CHAN);
  Voices = 8 * NB_CHAN;
  
  /* Tests Knobs */
  
  wxImage** imgs;
  imgs = new wxImage*[5];
  imgs[0] = 
    new wxImage(_T(string(GetDataDir() + string(BANK1)).c_str()));
  imgs[1] = 
    new wxImage(_T(string(GetDataDir() + string(BANK2)).c_str()));
  imgs[2] = 
    new wxImage(_T(string(GetDataDir() + string(BANK3)).c_str()));
  imgs[3] = 
    new wxImage(_T(string(GetDataDir() + string(BANK4)).c_str()));
  imgs[4] =
    new wxImage(_T(string(GetDataDir() + string(BANK5)).c_str()));
  
  BankKnob = 
    new StaticPosKnob(this, BB_OnBankChange, 5, imgs, 50, 1, 5, 1, 
		      wxPoint(56, 315), wxDefaultSize);
  
  
  wxImage** imgs_;
  imgs_ = new wxImage*[3];
  imgs_[0] = new wxImage(_T(string(GetDataDir() + string(STEPS_KNOB1)).c_str()));
  imgs_[1] = new wxImage(_T(string(GetDataDir() + string(STEPS_KNOB2)).c_str()));
  imgs_[2] = new wxImage(_T(string(GetDataDir() + string(STEPS_KNOB3)).c_str()));
  StepsKnob = new CycleKnob(this, BB_OnStepsChange, 3, imgs_, 10, 1, 64, 16,
			    wxPoint(712, 351), wxDefaultSize);
  

  /* Help */
  HelpMode = false;
  
  Connect(BB_Channel, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnChannelHelp);
    
  Connect(BB_OnPlayClick, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnPlayHelp);
  
  Connect(BB_PatternClick, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnPatternHelp);
  
  Connect(BB_OnMasterChange, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnMasterLevHelp);
  
  
  Connect(BB_OnEditClick, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnEditHelp);
  
  Connect(BB_OnPatternSelectors,wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnPatternsSelectionHelp);
  
  Connect(BB_OnBankChange,wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnBankHelp);
  
  Connect(BB_OnStepsChange, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnStepsHelp);
  
  Connect(BB_OnSigChoice, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnSignatureHelp);
  
  Connect(BB_OnPosChoice, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnPositionHelp);
  
  Connect(BB_OnLoadPatch, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnSaveLoadHelp);
  
  Connect(BB_OnSavePatch, wxEVT_ENTER_WINDOW,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnSaveLoadHelp);
  
  /* MIDI */
  
  MidiVolume[0] = M_CONTROL;
  MidiVolume[1] = 0x7;
  MidiSteps[0] = M_CONTROL;
  MidiSteps[1] = -1;
  
  Connect(BB_OnMasterChange, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnVolumeController);
  Connect(BB_OnStepsChange, wxEVT_RIGHT_DOWN,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxMouseEventFunction)&WiredBeatBox::OnStepsController);
  
  // banks and patterns may be changed by receiving midi notes
  int note = 24; // C2
  //int note = 0x24; // C1 (maybe C2?)
  for (int b = 0; b < NUM_BANKS; b++)
    BanksMidiNotes[b] = note++;
  note = 36; // C3
  //note = 0x36; // C2 (maybe C3?)
  for (int b = 0; b < NUM_PATTERNS; b++)
    PatternsMidiNotes[b] = note++;
  
  
}

void WiredBeatBox::OnChannelHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("One of the 11 channels available, each channel can load a soundfile and can contain up to 40 different rythms providing fine tunings for them");
}

void WiredBeatBox::OnPlayHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("This is the auto play button, when enabled, DRM31 will play selected pattern synchronously with Wired's Sequencer");
}

void WiredBeatBox::OnPatternHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("This is a tracker edition control, it can be clicked in 5 different places to obtain different velocities, right click returns to unclicked state");
}

void WiredBeatBox::OnHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("This is Wired's Beat Box plugin (DRM31). It brings you 11 channels to make up to 40 complex and realistic drum sequences within the traditional tracker or the optional sequencer view attached");
}

void WiredBeatBox::OnMasterLevHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("This knob sets the master level output of the DRM31 plugin");
}

void WiredBeatBox::OnPositionHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("These buttons control the position of the 16 strokes represented below");
}

void WiredBeatBox::OnSignatureHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("These buttons control the signature of the current rythm, note that the greater the denominator is, the faster the rythm will play");
}

void WiredBeatBox::OnStepsHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("Set the number of steps for the current rythm");
}
void WiredBeatBox::OnBankHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("The bank control allows you to choose one of the 5 banks proposed, each bank contains 8 rythms");
}
void WiredBeatBox::OnEditHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("This button switches the plugin to edit mode, when activated, you can edit other rythms on an other bank while playing the old selected rythm");
}
void WiredBeatBox::OnPatternsSelectionHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("These buttons represent 8 different rythms");
}
void WiredBeatBox::OnSaveLoadHelp(wxMouseEvent& WXUNUSED(event))
{
  if (HelpMode)
    SendHelp("These buttons are used if you want to import or export a patch apart from the session");
}


WiredBeatBox::~WiredBeatBox()
{
  //channels
  for (int i = 0; i < NB_CHAN; i++)
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
  PatternMutex.Lock();
  BufferSize = size;
  Pool->SetBufferSize(BufferSize);
  PatternMutex.Unlock();
  /*
    for (int i = 0; i <= NB_CHAN; i++)
    Channels[i]->Voices->SetBufferSize(size);
  */
}

void WiredBeatBox::SetVoices()
{
  Voices = 0;
  for (int i = 0; i < NB_CHAN; i++)
    {
      Voices += Channels[i]->Voices;
    }
  Pool->SetPolyphony(Voices);
}

inline void WiredBeatBox::RefreshPosLeds(double bar_pos)
{
  unsigned int cpt = 0;
  int i = static_cast<int>
    (floor(bar_pos * Steps[SelectedBank][SelectedPattern]));
  
  while (i >= 16)
    {
      cpt++;
      i -= 16;
    }
  for (int j = 0; j < 16; j++)
    if (LedsOn[j] == true && i != j)
      {
	PositionLeds[j]->SetBitmap(*PositionOff);
	//cout << "led[" << j << "] OFF" << endl;
	LedsOn[j] = false;
      }
  if (LedsOn[i] == false)
    if (cpt == PosIndex)
      {
	//cout << "led[" << i << "] ON" << endl;
	PositionLeds[i]->SetBitmap(*PositionOn);
	LedsOn[i] = true;
      }
}


void WiredBeatBox::Process(float** WXUNUSED(input), 
			   float **output, long sample_length)
{
  PatternMutex.Lock();
  if (OnLoading)
    { 
      PatternMutex.Unlock();
      return; 
    }
  
  if (OldSamplesPerBar != GetSamplesPerBar() ||
      OldBarsPerSample != GetBarsPerSample())
    {
      OldSamplesPerBar = GetSamplesPerBar();
      OldBarsPerSample = GetBarsPerSample();
      
      SamplesPerBar[SelectedBank][SelectedPattern] =
	static_cast<long>
	((static_cast<double>
	  (OldSamplesPerBar * StepsSigCoef[SelectedBank][SelectedPattern])));
      BarsPerSample[SelectedBank][SelectedPattern] = 
	OldBarsPerSample / StepsSigCoef[SelectedBank][SelectedPattern];
      
      if (SelectedBank != NewSelectedBank || 
	  SelectedPattern != NewSelectedPattern)
	{
	  SamplesPerBar[NewSelectedBank][NewSelectedPattern] =
	    static_cast<long>
	    ((static_cast<double>
	      (OldSamplesPerBar * 
	       StepsSigCoef[NewSelectedBank][NewSelectedPattern])));
	  BarsPerSample[NewSelectedBank][NewSelectedPattern] =
	    OldBarsPerSample/StepsSigCoef[NewSelectedBank][NewSelectedPattern];
	}
    }
  
  double bar_pos = fmod( (GetBarPos() / 
			  StepsSigCoef[SelectedBank][SelectedPattern]), 1.0 );
  double bar_end = 
    static_cast<double>
    (bar_pos + static_cast<double>
     (sample_length * BarsPerSample[SelectedBank][SelectedPattern]));
  
  double new_bar_end, new_bar_pos;
  if (SelectedBank != NewSelectedBank || 
      SelectedPattern != NewSelectedPattern)
    {
      new_bar_pos = 
	fmod( (GetBarPos() / 
	       StepsSigCoef[NewSelectedBank][NewSelectedPattern]), 1.0 );
      new_bar_end =
	static_cast<double>
	(new_bar_pos + static_cast<double>
	 (sample_length * BarsPerSample[NewSelectedBank][NewSelectedPattern]));
    }
  else
    {
      new_bar_pos = bar_pos;
      new_bar_end = bar_end;
    }

  if (Playing)
    { // recuperation des notes a jouer
      
      //RefreshPosLeds(bar_pos);
      bool isend = false;
      /*
	if (!SelectedChannel->Muted)
	GetNotesFromChannel(SelectedChannel, bar_pos, bar_end, 
			    new_bar_pos, new_bar_end, &isend);
			    else*/
	for (int i = 0; i < NB_CHAN; i++)
	  if ( (Channels[i]->Wave != 0x0) && !Channels[i]->Muted )
	    GetNotesFromChannel(Channels[i], bar_pos, bar_end, 
				new_bar_pos, new_bar_end, &isend);
      if (isend)
	{
	  SelectedPattern = NewSelectedPattern;
	  SelectedBank = NewSelectedBank;
	}
    }

  //playing notes

  long len = 0;
  long newoffset = 0;
  float curvel, velstep;
  float **buf;
  //int cpt_note = 0;
  for (list<BeatNoteToPlay*>::iterator bn = NotesToPlay.begin(); 
       bn != NotesToPlay.end(); )
    {
      if ( !Channels[(*bn)->NumChan]->Wave )
	{
	  //if ((*bn)->Buffer)
	  //bn = NotesToPlay.erase(bn);
	  continue;
	}      
      
      if ((*bn)->Buffer == 0x0)
	{
	  if ( VoicesCount[(*bn)->NumChan] + 1 >
	       Channels[(*bn)->NumChan]->Voices )
	    {
	      delete *bn;
	      bn = NotesToPlay.erase(bn);
	      cout << "[DRM31] not enough voices" << endl;
	      //cout << "note cpt=" << cpt_note++ << " continuing" << endl;
	      continue;
	    }
	  else
	    {
	      VoicesCount[(*bn)->NumChan]++;
	      buf = Pool->GetFreeBuffer();
	      if (!buf)
		{
		  cout << "[DRM31] Couldnt Get Free Buffer" << endl;
		  bn = NotesToPlay.erase(bn);
		  continue;
		}
	      (*bn)->Buffer = buf;
	    }
	}
      //cout << "note cpt=" << cpt_note++ << " continuing" << endl;
      len = sample_length - (*bn)->Delta;
      
      if ( (*bn)->OffSet >= 
	   Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames() ||
	   (*bn)->OffSet >= (*bn)->SEnd )
	{
	  
	  cout << "[DRM31] " << "note offset: " << (*bn)->OffSet
	       << " / frames " << (*bn)->SEnd
	       << " Offset > Wave Frames, maybe normal" <<endl;
	  //memset((*bn)->Buffer[0], 0, sample_length * sizeof(float));
	  //memset((*bn)->Buffer[1], 0, sample_length * sizeof(float));
	  //if ((*bn)->Buffer)
	  //cout << "note before " << (*bn)->Params[VEL] << endl;
	  bn++;
	  continue;
	}
      
      
      /* note velocity calculation */
      curvel = Channels[(*bn)->NumChan]->Params[VEL] * (*bn)->Params[VEL];
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
      
      //assert(end <= Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames());
      
      if ( (*bn)->OffSet + len > end )
	   //Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames()) 	
	{
	  //cout << "finishing sound1\n" << endl;
	  len = end - (*bn)->OffSet;
	  if (len <= 0)
	    {
	      //cout << " len < 0 !! " << endl;
	      bn++;
	      continue;
	    }
	}
      /*else if ((*bn)->OffSet + len == end )
	{
	  cout << "finishing sound2\n" << endl;
	}
      */
      Channels[(*bn)->NumChan]->Wave->SetPitch((*bn)->Params[PIT]);
      Channels[(*bn)->NumChan]->Wave->SetInvert((*bn)->Reversed);
      
      /*(*bn)->OffSet += */
      Channels[(*bn)->NumChan]->Wave->Read((*bn)->Buffer,
					   (*bn)->OffSet,
					   len,
					   (*bn)->Delta,
					   &newoffset);
      (*bn)->OffSet = newoffset;
      if ((*bn)->Delta)
	{
	  //Channels[(*bn)->NumChan]->PlayButton->SetOn();
	  (*bn)->Delta = 0;
	}
      for (long i = 0; i < sample_length; i++)
	{
	  if (curvel <= 0.f)
	    {
	      curvel = 0.f;
	      //printf("vel < 0.f     break\n");
	      //break;
	    }
	  
	  (*bn)->Buffer[0][i] *= 
	    (*bn)->Params[LEV] * curvel * (*bn)->Pan[0];
	  (*bn)->Buffer[1][i] *= 
	    (*bn)->Params[LEV] * curvel * (*bn)->Pan[1];
	  
	  curvel -= velstep;
	  //printf("curvel=%f\n", curvel);
	}
      bn++;
    }
  Pool->GetMix(output);
  /*
    for (long i = 0; i < sample_length; i++)
    {
      if (IS_DENORMAL(output[0][i]) && output[0][i] != 0.f)
	{
	  cout << "Denormal found: L: " << i << endl;
	}
      if (IS_DENORMAL(output[1][i]) && output[1][i] != 0.f)
	{
	  cout << "Denormal found: R: " << i << endl;
	}
    }
  */
  //PatternMutex.Unlock();
  
  // effacage des notes finies
  for (list<BeatNoteToPlay*>::iterator bn = NotesToPlay.begin(); 
       bn != NotesToPlay.end(); )
    {
      if ( !(Channels[(*bn)->NumChan]->Wave) || 
	   ((*bn)->OffSet >= 
	    Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames()) ||
	   ((*bn)->OffSet >= ((*bn)->SEnd)) )
	{
	  
	  //Channels[(*bn)->NumChan]->PlayButton->SetOff();
	  VoicesCount[(*bn)->NumChan]--;
	  if (VoicesCount[(*bn)->NumChan] < 0)
	    {
	      cout << "[DRM31] error VoicesCount < 0" <<endl;
	    }
	  if ((*bn)->Buffer)
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
  PatternMutex.Lock();
   if (OnLoading)
    { PatternMutex.Unlock(); return; }
   else
     PatternMutex.Unlock();
   
  if (((event.MidiData[0] == M_NOTEON1) || (event.MidiData[0] == M_NOTEON2)))
    {
      if (event.MidiData[2])
	{
	  int i;
	  if (event.MidiData[1] >= ChanMidiNotes[0])
	    {
	      for (i = 0; i < NB_CHAN; i++)
		if (ChanMidiNotes[i] == event.MidiData[1])
		  {
		    BeatNoteToPlay *n = 
		      new BeatNoteToPlay( event.MidiData[2] / 100.f,
					  event.DeltaFrames, i);
		    PatternMutex.Lock();
		    SetMidiNoteAttr(n, Channels[i]);
		    NotesToPlay.push_back(n);
		    PatternMutex.Unlock();
		    return;//break;
		  }
	    }
	  else if (event.MidiData[1] >= PatternsMidiNotes[0])
	    {
	      for (i = 0; i < NUM_PATTERNS; i++)
		if (PatternsMidiNotes[i] == event.MidiData[1])
		  {
		    PatternMutex.Lock();
		    EditedPattern = NewSelectedPattern = i;
		    PatternMutex.Unlock();
		    
		    AskUpdatePattern = true;
		    AskUpdate();
		    return;
		  }
	    }
	  else if (event.MidiData[1] >= BanksMidiNotes[0])
	    for (i = 0; i < NUM_BANKS; i++)
	      if (BanksMidiNotes[i] == event.MidiData[1])
		{
		  PatternMutex.Lock();
		  EditedBank = NewSelectedBank = i;
		  PatternMutex.Unlock();
		  AskUpdateBank = true;
		  AskUpdate();
		  return;
		}
	}
    }
  else
    { ProcessMidiControls(event.MidiData); }
}

inline void WiredBeatBox::ProcessMidiControls(int data[3])
{
  if ((MidiVolume[0] == data[0]) && (MidiVolume[1] == data[1]))
    {
      float tmp = data[2] / 100.0f;
      MLevel = tmp;
      
      PatternMutex.Lock();
      MidiVolume[2] = data[2];
      Pool->SetVolume(MLevel);
      AskUpdateLevel = true;
      PatternMutex.Unlock();
      
      AskUpdate();
    }
  else if ((MidiSteps[0] == data[0]) && (MidiSteps[1] == data[1]))
    {
      PatternMutex.Lock();
      UpdateStepsDeps(data[2]/2);
      AskUpdateSteps = true;
      PatternMutex.Unlock();
      
      AskUpdate();
    }
  else 
    {
      for (int i = 0; i < NB_CHAN; i++)
	{
	  if ((Channels[i]->MidiVolume[0] == data[0]) && 
	      (Channels[i]->MidiVolume[1] == data[1]))
	    {
	      Channels[i]->MidiVolume[2] = data[2];
	      Channels[i]->SetLev(data[2]);
	      break;
	    }
	  else if ((Channels[i]->MidiPan[0] == data[0]) && 
		   (Channels[i]->MidiPan[1] == data[1]))
	    {
	      Channels[i]->MidiPan[2] = data[2];
	      Channels[i]->SetPan(data[2]);
	      break;
	    }
	  else if ((Channels[i]->MidiPitch[0] == data[0]) && 
	      (Channels[i]->MidiPitch[1] == data[1]))
	    {
	      Channels[i]->MidiPitch[2] = data[2];
	      Channels[i]->SetPitch(data[2]);
	      break;
	    }
	  else if ((Channels[i]->MidiVel[0] == data[0]) && 
	      (Channels[i]->MidiVel[1] == data[1]))
	    {
	      Channels[i]->MidiVel[2] = data[2];
	      Channels[i]->SetVel(data[2]);
	      break;
	    }
	  else if ((Channels[i]->MidiStart[0] == data[0]) && 
	      (Channels[i]->MidiStart[1] == data[1]))
	    {
	      Channels[i]->MidiStart[2] = data[2];
	      Channels[i]->SetStart(data[2]);
	      break;
	    }
	  else if ((Channels[i]->MidiEnd[0] == data[0]) && 
	      (Channels[i]->MidiEnd[1] == data[1]))
	    {
	      Channels[i]->MidiEnd[2] = data[2];
	      Channels[i]->SetEnd(data[2]);
	      break;
	    }
	  else if ((Channels[i]->MidiPoly[0] == data[0]) && 
		   (Channels[i]->MidiPoly[1] == data[1]))
	    {
	      Channels[i]->MidiPoly[2] = data[2];
	      break;
	    }
	}
    }
}


inline void WiredBeatBox::SetNoteAttr(BeatNoteToPlay* note, BeatBoxChannel* c)
{
  note->Params[LEV] = floor(c->Params[LEV] * note->Params[LEV] * 100) / 100;
  note->Params[VEL] = floor(c->Params[VEL] * note->Params[VEL] * 100) / 100;
  note->Params[PIT] = floor(c->Params[PIT] * note->Params[PIT] * 100) / 100;
  note->Params[PAN] = floor(c->Params[PAN] * note->Params[PAN] * 100) / 100;
  if (c->Params[STA] && note->Params[STA])
    note->Params[STA] = floor(c->Params[STA] * note->Params[STA] * 100) / 100;
  else if (c->Params[STA])
    note->Params[STA] = c->Params[STA];
  
  note->Params[END] = floor(c->Params[END] * note->Params[END] * 100) / 100;
  
  CalcPan(note->Params[PAN], note->Pan);
  
  note->Reversed = note->Reversed ? true : c->Reversed;
  if (c->Wave)
    {
      note->OffSet =
	static_cast<unsigned long>
	(floor(c->Wave->GetNumberOfFrames() * (note->Params[STA])));
      note->SEnd = 
	static_cast<unsigned long>
	(floor(c->Wave->GetNumberOfFrames() * note->Params[END]));
    }
  else
    note->OffSet = note->SEnd = 0;
}

inline void WiredBeatBox::SetChanAttrToNote(BeatNoteToPlay* note, 
					    BeatBoxChannel* c)
{
  note->NumChan = c->Id;
  for (int i = 0; i < NB_PARAMS; i++)
    note->Params[i] = c->Params[i];
  note->Reversed = c->Reversed;
  CalcPan(note->Params[PAN], note->Pan);
  note->Buffer = 0x0;
  note->Delta = 0;
  
  if (c->Wave)
    {
      note->OffSet = static_cast<unsigned long>
	(floor(c->Wave->GetNumberOfFrames() * note->Params[STA]));
      note->SEnd = static_cast<unsigned long>
	(floor(c->Wave->GetNumberOfFrames() * note->Params[END]));
    }
  else
    note->OffSet = note->SEnd = 0;
}

inline void WiredBeatBox::SetMidiNoteAttr(BeatNoteToPlay* note, 
					  BeatBoxChannel* c)
{
  note->NumChan = c->Id;
  note->Params[LEV] = c->Params[LEV];
  note->Params[PIT] = c->Params[PIT];
  note->Params[PAN] = c->Params[PAN];
  note->Params[STA] = c->Params[STA];
  note->Params[END] = c->Params[END];
  
  note->Reversed = c->Reversed;
  CalcPan(note->Params[PAN], note->Pan);
  note->Buffer = 0x0;
  
  if (c->Wave)
    {
      note->OffSet =
	static_cast<unsigned long>
	(floor(c->Wave->GetNumberOfFrames() * (note->Params[STA])));
      note->SEnd = 
	static_cast<unsigned long>
	(floor(c->Wave->GetNumberOfFrames() * note->Params[END]));
    }
  else
    note->OffSet = note->SEnd = 0;
}
	      

inline void WiredBeatBox::GetNotesFromChannel(BeatBoxChannel* c, 
					      double bar_pos, 
					      double bar_end,
					      double new_bar_pos,
					      double new_bar_end,
					      bool* isend)
{
  BeatNoteToPlay*	note;
  unsigned long		delta;
  double		lasts;
  
  if (bar_end > 1.0f)
    {
      *isend = true;
      if (new_bar_end > 1.0f)
	{
	  lasts = new_bar_end - 1.0f;
	  for (list<BeatNote*>::iterator bn = 
		 c->Rythms[NewSelectedBank][NewSelectedPattern].begin();
	       bn != c->Rythms[NewSelectedBank][NewSelectedPattern].end(); 
	       bn++)
	    {
	      if ((*bn)->BarPos > lasts)
		break;
	      
	      delta = static_cast<unsigned long>
		( ((1.0 - new_bar_pos + (*bn)->BarPos) * 
		   SamplesPerBar[NewSelectedBank][NewSelectedPattern]) );
	      note = 
		new BeatNoteToPlay(*bn, delta);
	      SetNoteAttr(note, c);
	      NotesToPlay.push_back(note);
	    }
	}
      else
	{
	  for (list<BeatNote*>::iterator bn = 
		 c->Rythms[NewSelectedBank][NewSelectedPattern].begin();
	       bn != c->Rythms[NewSelectedBank][NewSelectedPattern].end(); 
	       bn++)
	    {
	      if ((*bn)->BarPos >= 1.0)
		{
		  break;
		}
	      else if ((*bn)->BarPos >= new_bar_pos && 
		       (*bn)->BarPos < new_bar_end)
		{
		  delta = static_cast<unsigned long>
		    ( (((*bn)->BarPos - new_bar_pos) * 
		       SamplesPerBar[NewSelectedBank][NewSelectedPattern]) );

		  note = 
		    new BeatNoteToPlay(*bn, delta);
		  SetNoteAttr(note, c);
		  NotesToPlay.push_back(note);
		}
	    }
	}
    }
  
  for (list<BeatNote*>::iterator bn = 
	 c->Rythms[SelectedBank][SelectedPattern].begin();
       bn != c->Rythms[SelectedBank][SelectedPattern].end(); bn++)
    {
      if ((*bn)->BarPos >= 1.0)
	{
	  break;
	}
      else if ((*bn)->BarPos >= bar_pos && (*bn)->BarPos < bar_end)
	{
	  delta = static_cast<unsigned long>
	    ( (((*bn)->BarPos - bar_pos) * 
	       SamplesPerBar[SelectedBank][SelectedPattern]) );
	  note = 
	    new BeatNoteToPlay(*bn, delta);
	  SetNoteAttr(note, c);
	  NotesToPlay.push_back(note);
	}
    }
}

inline void WiredBeatBox::UpdateNotesPositions(unsigned int bank, 
					       unsigned int track)
{
  for (unsigned char i = 0; i < NB_CHAN; i++)
    for ( list<BeatNote*>::iterator bn = 
	    Channels[i]->Rythms[bank][track].begin();
	  bn != Channels[i]->Rythms[bank][track].end(); 
	  bn++ )
      {
	(*bn)->BarPos = 
	  static_cast<double>((*bn)->Position / 
			      static_cast<float>(Steps[bank][track]));
      }
}


void WiredBeatBox::OnPaint(wxPaintEvent &event)
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
  Plugin::OnPaintEvent(event);
}


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
  
  //PatternMutex.Lock();
  
  /////
  unsigned int bank = (OnEdit ? EditedBank : NewSelectedBank);
  unsigned int track = (OnEdit ? EditedPattern : NewSelectedPattern);
  
  SignatureButtons[SigIndex[bank][track]]->SetOff();
  SignatureButtons[*i]->SetOn();
  
  SigIndex[bank][track] = *i;
  SignatureDen[bank][track] = SigDen[*i];
  Signature[bank][track] = Signatures[*i];
  
  double steps_sig_coef = static_cast<double>
    ( Steps[bank][track] / static_cast<double>(SignatureDen[bank][track]) );
  //////
  
  PatternMutex.Lock();
  StepsSigCoef[bank][track] = steps_sig_coef;
  SamplesPerBar[bank][track] = static_cast<long>
    ((static_cast<double>(OldSamplesPerBar * steps_sig_coef )));
  BarsPerSample[bank][track] = OldBarsPerSample / StepsSigCoef[bank][track];
  //SamplesPerBar = spb;
  //BarsPerSample = bps;
  PatternMutex.Unlock();
}

inline void WiredBeatBox::ReCalcStepsSigCoef(void)
{
  for (int bank = 0; bank < NUM_BANKS; bank++)
    for (int track = 0; track < NUM_PATTERNS; track++)
      {
	StepsSigCoef[bank][track] = static_cast<double>
       ( Steps[bank][track] / static_cast<double>(SignatureDen[bank][track]) );
	SamplesPerBar[bank][track] = static_cast<long>
	((static_cast<double>(OldSamplesPerBar * StepsSigCoef[bank][track] )));
	BarsPerSample[bank][track] = 
	  OldBarsPerSample / StepsSigCoef[bank][track];
	UpdateNotesPositions(bank, track);
      }
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
  
  //PatternMutex.Lock();
  
  switch (data[1])
    {
    case ACT_SELECT:
      for (unsigned char i = 0; i < NB_CHAN; i++)
	Channels[i]->DeSelect();
      tmp->Select();
      
      PatternMutex.Lock();
      SelectedChannel = tmp;
      PatternMutex.Unlock();

      SetPatternList();
      /*
	if (View && SelectedChannel->Wave)
	{
	  View->SetWaveFile(SelectedChannel->Wave);
	}
      */
      break;
    case ACT_SOLO:
      PatternMutex.Lock();
      if (tmp->IsSolo)
	for (unsigned char i = 0; i < NB_CHAN; i++)
	  {
	    Channels[i]->DeSelect();
	    Channels[i]->Mute();
	  }
      else
	for (unsigned char i = 0; i < NB_CHAN; i++)
	  {
	    Channels[i]->DeSelect();
	    Channels[i]->UnMute();
	  }
      tmp->Select();
      tmp->UnMute();
      SelectedChannel = tmp;
      PatternMutex.Unlock();
      SetPatternList();
      break;
    case ACT_PLAY:
      if (!tmp->Wave)
	{ break; }
      
      bn = new BeatNoteToPlay();
      PatternMutex.Lock();
      SetChanAttrToNote(bn, tmp);
      NotesToPlay.push_back(bn);
      PatternMutex.Unlock();
      break;
    case ACT_SETWAVE:
      break;
    default:
      break;
    }
}

void WiredBeatBox::OnPatternSelectors(wxCommandEvent& e)
{
  unsigned int *p = (unsigned int*)e.GetClientData();
  
  if (OnEdit && (EditedPattern != *p))
    {
      PatternSelectors[SelectedPattern]->SetOff();
      PatternSelectors[NewSelectedPattern]->SetOff();
      PatternSelectors[EditedPattern]->SetOff();
      
      PatternMutex.Lock();
      EditedPattern = *p;
      PatternMutex.Unlock();
      
      UpdateSteps(EditedBank, EditedPattern);
      SetPatternList();
      PatternSelectors[EditedPattern]->SetOn();
    }
  else if (!OnEdit && (NewSelectedPattern != *p))
    {
      PatternSelectors[SelectedPattern]->SetOff();
      PatternSelectors[NewSelectedPattern]->SetOff();
      
      PatternMutex.Lock();
      EditedPattern = NewSelectedPattern = *p;
      PatternMutex.Unlock();
      
      UpdateSteps(NewSelectedBank, NewSelectedPattern);
      PatternSelectors[NewSelectedPattern]->SetOn();
      SetPatternList();
    }
  if (View)
    View->Refresh();
}

inline void WiredBeatBox::UpdateSteps(unsigned int bank, unsigned int track)
{
  StepsKnob->SetValue(Steps[bank][track]);
  wxString s;
  s.Printf("%d", Steps[bank][track]);
  StepsLabel->SetLabel(s);
  
  for (int i = 0; i < 5; i++)
    SignatureButtons[i]->SetOff();
  SignatureButtons[SigIndex[bank][track]]->SetOn();
}

void WiredBeatBox::SetPatternList(void)
{
  double	i, j;
  int		index;
  
  for (index = 0; index < 16; index++)
    Beat[index]->SetState(ID_UNCLICKED);
  
  i = static_cast<double>(PosIndex * 16);
  j = static_cast<double>(i + 16.0);
  
  unsigned int bank = (OnEdit ? EditedBank : NewSelectedBank);
  unsigned int cur = (OnEdit ? EditedPattern : NewSelectedPattern);
  
  for (list<BeatNote*>::iterator bn = 
	 SelectedChannel->Rythms[bank][cur].begin();
       bn != SelectedChannel->Rythms[bank][cur].end(); bn++)
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

void WiredBeatBox::OnPatternMotion(wxCommandEvent& e)
{
  int* tmp = (int*)e.GetClientData();
  BeatBoxChannel* c = SelectedChannel;
  int i = PosIndex * 16;
  
  for (int p = 0; p < 16; p++)
    {
      if (Beat[p]->GetPosition().x <= tmp[ID_X]
	  && Beat[p]->GetPosition().x + BTN_SIZE >= tmp[ID_X])
	{
	  if ( Beat[p]->GetState() != tmp[ID_STATE] )
	    {
	      Beat[p]->SetState(tmp[ID_STATE]);
	      double pos = static_cast<double>(i + p);
	      AddBeatNote(c, pos, tmp[ID_STATE]);
	    }
	  break;
	}
    }
  if (View)
    View->Refresh();
}

void WiredBeatBox::RemBeatNote(BeatNote* note, BeatBoxChannel* c, 
			       unsigned int bank, unsigned int track)
{
  
  for ( list<BeatNote*>::iterator b = c->Rythms[bank][track].begin();
	b != c->Rythms[bank][track].end(); b++)
    {
      if (*b == note)
	{
	  PatternMutex.Lock();
	  c->Rythms[bank][track].erase(b);
	  PatternMutex.Unlock();
	  break;
	}
    }
  SetPatternList();
}

void WiredBeatBox::AddBeatNote(BeatNote* note, BeatBoxChannel* c, 
			       unsigned int bank, unsigned int track)
{
  
  if (c->Rythms[bank][track].empty())
    {
      PatternMutex.Lock();
      c->Rythms[bank][track].push_back(note);
      PatternMutex.Unlock();
      SetPatternList();
      return;
    }
  if (note->Position > c->Rythms[bank][track].back()->Position)
    {
      PatternMutex.Lock();
      c->Rythms[bank][track].push_back(note);
      PatternMutex.Unlock();
      SetPatternList();
      return;
    }
  
  for ( list<BeatNote*>::iterator b = c->Rythms[bank][track].begin();
	b != c->Rythms[bank][track].end(); b++)
    {  
      if ( (*b)->Position > note->Position
	   || (*b)->Position == note->Position) 
	{
	  PatternMutex.Lock();
	  c->Rythms[bank][track].insert(b, note);
	  PatternMutex.Unlock();
	  SetPatternList();
	  return;
	}
    }
}

inline void WiredBeatBox::AddBeatNote(BeatBoxChannel* c,
				      double rel_pos, unsigned int state)
{
  //PatternMutex.Lock();
  unsigned int bank = (OnEdit ? EditedBank : NewSelectedBank);
  unsigned int track = (OnEdit ? EditedPattern : NewSelectedPattern);
  //PatternMutex.Unlock();
  
  double bar_pos = 
    static_cast<double>( rel_pos / static_cast<double>(Steps[bank][track]) );
    
  if (c->Rythms[bank][track].empty())
    {
      BeatNote *note = new BeatNote(c->Id, rel_pos, state, bar_pos);
      note->Params[VEL] = 1.0f * static_cast<float>(state / 4.f);
      note->Reversed = false;
      
      PatternMutex.Lock();
      c->Rythms[bank][track].push_back(note);
      PatternMutex.Unlock();
      return;
    }
  if ( rel_pos > c->Rythms[bank][track].back()->Position ) 
    {
      BeatNote *note = new BeatNote(c->Id, rel_pos, state, bar_pos);
      //note->Params[VEL] = 1.0f * static_cast<float>(state / 4.f);
      note->Reversed = false;
      
      PatternMutex.Lock();
      c->Rythms[bank][track].push_back(note);
      PatternMutex.Unlock();
      return;
    }
  
  
  for ( list<BeatNote*>::iterator b = c->Rythms[bank][track].begin();
	b != c->Rythms[bank][track].end(); b++)
    {  
      if ( (*b)->Position == rel_pos ) 
	{
	  if ( state == ID_UNCLICKED )
	    {
	      BeatNote* note = *b;
	      PatternMutex.Lock();
	      c->Rythms[bank][track].erase(b);
	      PatternMutex.Unlock();
	      delete note;
	    }
	  else
	    {
	      PatternMutex.Lock();
	      (*b)->State = state;
	      (*b)->Params[VEL] = (float)(state / 4.f);
	      PatternMutex.Unlock();
	    }
	  return;
	}
      else if ( (*b)->Position > rel_pos ) 
	{
	  BeatNote *note = new BeatNote(c->Id, rel_pos, state, bar_pos);
	  //note->Params[VEL] = 1.0f * static_cast<float>(state / 4.f);
	  note->Reversed = false;
	  PatternMutex.Lock();
	  c->Rythms[bank][track].insert(b, note);
	  PatternMutex.Unlock();
	  return;
	}
    }
}

void WiredBeatBox::OnPatternClick(wxCommandEvent &e)
{
  int* tmp = (int*)e.GetClientData();
  
  if (tmp[ID_COPY])
    PopupMenu(PopMenu);
  BeatBoxChannel* c = SelectedChannel;
  
  int i = PosIndex * 16;
  
  double pos = static_cast<double>(tmp[ID_POS] + i);
  
  AddBeatNote(c, pos, tmp[ID_STATE]);
  if (View)
    View->Refresh();
}

void WiredBeatBox::ShowOpt(wxCommandEvent& WXUNUSED(e))
{
  ShowOptionalView();
}

void WiredBeatBox::OnSavePatch(wxCommandEvent& WXUNUSED(e))
{
  vector<string> exts;
  exts.push_back("drm\tDRM-31 patch file (*.drm)");
  //cout << "OnSavePatch(): begin" << endl;
  
  string selfile = SaveFileLoader("Save Patch", &exts);
  if (!selfile.empty())
    {
      int fd = open(selfile.c_str(),  O_CREAT | O_TRUNC | O_WRONLY, 
		    S_IRUSR | S_IWUSR);
      if (fd < 0)
	{
	  cout << "[WIREDBEATBOX] Couldnt open file: "<<selfile << " )" << endl;
	  //close(fd);
	  return;
	}
      /*
	wxProgressDialog *Progress = 
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
      OnLoading = true;
      PatternMutex.Unlock();
      
      for (int chan = 0; chan < NB_CHAN; chan++)
	Channels[chan]->Reset();
      
      struct stat st;
      fstat(fd, &st);
      cout << "[DRM31] load patch: file size: " << st.st_size 
	   << endl;
      
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
  
  EditedPattern = SelectedPattern = EditedBank = SelectedBank = 0;
  
  SelectedChannel = Channels[0];
  Channels[0]->Select();
  ReCalcStepsSigCoef();
  UpdateSteps(0,0);
  SetPatternList();
  
  PatternMutex.Lock();
  OnLoading = false;
  PatternMutex.Unlock();
  
}

long WiredBeatBox::Save(int fd)
{
  long len, size = 0;
  int steps, sig_index = 0;
  int bank, ps, res, m;
  
  PatternMutex.Lock();
  OnLoading = true;
  PatternMutex.Unlock();
  
  // writing midi params
  for (m = 0; m < 2; m++)
    if ((res = write(fd, &(MidiVolume[m]), sizeof(int))) != sizeof (int))
      return (-1);
    else
      size += res;
  for (m = 0; m < 2; m++)
    if ((res = write(fd, &(MidiSteps[m]), sizeof(int))) != sizeof (int))
      return (-1);
    else
      size += res;
  for (int i = 0; i < NB_CHAN; i++)
    {
      for (m = 0; m < 2; m++)
	if ((res = write(fd, &(Channels[i]->MidiVolume[m]), sizeof(int)) )
	    != sizeof (int))
	  { cout << "[DRM31] Save: write() error" << endl; return (-1); }
	else
	  size += res;
      for (m = 0; m < 2; m++)
	if ((res = write(fd, &(Channels[i]->MidiVel[m]), sizeof(int)))
	    != sizeof (int))
	  { cout << "[DRM31] Save: write() error" << endl; return (-1); }
	else
	  size += res;
      for (m = 0; m < 2; m++)
	if ((res = write(fd, &(Channels[i]->MidiPitch[m]), sizeof(int))) 
	    != sizeof (int))
	  { cout << "[DRM31] Save: write() error" << endl; return (-1); }
	else
	  size += res;
      for (m = 0; m < 2; m++)
	if ((res = write(fd, &(Channels[i]->MidiPan[m]), sizeof(int))) 
	    != sizeof (int))
	  { cout << "[DRM31] Save: write() error" << endl; return (-1); }
	else
	  size += res;
      for (m = 0; m < 2; m++)
	if ((res = write(fd, &(Channels[i]->MidiStart[m]), sizeof(int))) 
	    != sizeof (int))
	  { cout << "[DRM31] Save: write() error" << endl; return (-1); }
	else
	  size += res;
      for (m = 0; m < 2; m++)
	if ((res = write(fd, &(Channels[i]->MidiEnd[m]), sizeof(int))) 
	    != sizeof (int))
	  { cout << "[DRM31] Save: write() error" << endl; return (-1); }
	else
	  size += res;
      
    }
  
  //writing steps/signatures params
  for (bank = 0; bank < 5; bank++)
    for (ps = 0; ps < 8; ps++)
      {
	if ((res = write(fd, &(SigIndex[bank][ps]), sizeof (int))) 
	    != sizeof (int))
	  { cout << "[DRM31] Save: write() error" << endl; return (-1); }
	else
	  size += res;
	if ((res = write(fd, &(Steps[bank][ps]), sizeof (int))) 
	    != sizeof (int))
	  { cout << "[DRM31] Save: write() error" << endl; return (-1); }
	else
	  size += res;
      }
  
  for (int i = 0; i < NB_CHAN; i++)
    {
      if (Channels[i]->Wave)
	{
	  len = Channels[i]->Wave->Filename.size();
	  size += write(fd, &len, sizeof(long));
	  size +=
	    write(fd, Channels[i]->Wave->Filename.c_str(), len * sizeof(char));
	}
      else
	{
	  len = 0;
	  size += write(fd, &len, sizeof(long));
	}
      
      //writing params
      int p;
      for (p = 0; p < NB_PARAMS; p++)
	size += write(fd, &(Channels[i]->Params[p]), sizeof(float));
      
      //writing notes
      for (bank = 0; bank < 5; bank++)
	for (ps = 0; ps < 8; ps++)
	  {
	    len = Channels[i]->Rythms[bank][ps].size();
	    size += write(fd, &len, sizeof(long));
	    for (list<BeatNote*>::iterator bn = 
		   Channels[i]->Rythms[bank][ps].begin();
		 bn != Channels[i]->Rythms[bank][ps].end(); bn++)
	      {
		//cout << "note pos: " << (*bn)->Position;
		/*
		  printf("note pos=%f; state=%d\n", 
		       (*bn)->Position,(*bn)->State);
		*/
		size += write(fd, &((*bn)->State), sizeof(unsigned int));
		size += 
		  write(fd, &((*bn)->Position), sizeof (double));
		for (p = 0; p < NB_PARAMS; p++ )
		  size +=
		    write(fd, &((*bn)->Params[p]), sizeof (float));
		p = (*bn)->Reversed ? 1 : 0;
		size += write(fd, &p, sizeof(p));
	      }
	  }
    }
  PatternMutex.Lock();
  OnLoading = false;
  PatternMutex.Unlock();
  
  cout << "[DRM31] saved size " << size << endl;
  return size;
}

void WiredBeatBox::Load(int fd, long size)
{
  int res, m, tmp_int;
  long tmp_long;
  int bank,  ps;
  unsigned int tmp_uint = 0;
  float tmp_float = 0.f;
  double tmp_double = 0.0;
  BeatNote* note;
  WaveFile* w;
  
  PatternMutex.Lock();
  OnLoading = true;
  PatternMutex.Unlock();
  
  //reading midi params
  cout << "[DRM31] Midi params loading" << endl;
  for (m = 0; m < 2; m++)
    if ((res = read(fd, &tmp_int, sizeof(int))) != sizeof (int))
      { cout << "[DRM31] Load: read error" << endl; return; }
    else
      { size -= res; MidiVolume[m] = tmp_int; }
  for (m = 0; m < 2; m++)
    if ((res = read(fd, &tmp_int, sizeof(int))) != sizeof (int))
      { cout << "[DRM31] Load: read error" << endl; return; }
    else
      { size -= res; MidiSteps[m] = tmp_int; }
  for (int i = 0; i < NB_CHAN; i++)
    {
      for (m = 0; m < 2; m++)
	if ((res = read(fd, &tmp_int, sizeof(int))) != sizeof (int))
	  { cout << "[DRM31] Load: read error" << endl; return; }
	else
	  { size -= res; Channels[i]->MidiVolume[m] = tmp_int; }
      for (m = 0; m < 2; m++)
	if ((res = read(fd, &tmp_int, sizeof(int))) != sizeof (int))
	  { cout << "[DRM31] Load: read error" << endl; return; }
	else
	  { size -= res; Channels[i]->MidiVel[m] = tmp_int; }
      for (m = 0; m < 2; m++)
	if ((res = read(fd, &tmp_int, sizeof(int))) != sizeof (int))
	  { cout << "[DRM31] Load: read error" << endl; return; }
	else
	  { size -= res; Channels[i]->MidiPitch[m] = tmp_int; }
      for (m = 0; m < 2; m++)
	if ((res = read(fd, &tmp_int, sizeof(int))) != sizeof (int))
	  { cout << "[DRM31] Load: read error" << endl; return; }
	else
	  { size -= res; Channels[i]->MidiPan[m] = tmp_int; }
      for (m = 0; m < 2; m++)
	if ((res = read(fd, &tmp_int, sizeof(int))) != sizeof (int))
	  { cout << "[DRM31] Load: read error" << endl; return; }
	else
	  { size -= res; Channels[i]->MidiStart[m] = tmp_int; }
      for (m = 0; m < 2; m++)
	if ((res = read(fd, &tmp_int, sizeof(int))) != sizeof (int))
	  { cout << "[DRM31] Load: read error" << endl; return; }
	else
	  { size -= res; Channels[i]->MidiEnd[m] = tmp_int; }
    }
  //cout << "[DRM31] Midi params loaded" << endl;
  
  cout << "[DRM31] Signatures/Steps params loading" << endl;
  for (bank = 0; bank < 5; bank++)
    for (ps = 0; ps < 8; ps++)
      {
	if ((res = read(fd, &tmp_int, sizeof (int))) != sizeof (int))
	  { cout << "[DRM31] Load: read error" << endl; return; }
	else
	  { size -= res; 
	  SigIndex[bank][ps] = tmp_int; 
	  SignatureDen[bank][ps] = SigDen[tmp_int];
	  Signature[bank][ps] = Signatures[tmp_int];
	  }
	if ((res = read(fd, &tmp_int, sizeof (int))) != sizeof (int))
	  { cout << "[DRM31] Load: read error" << endl; return; }
	else
	  { size -= res; Steps[bank][ps] = tmp_int; }
      }
  //cout << "[DRM31] Signatures/Steps params loaded" << endl;
  cout << "[DRM31] Loading Channels: Samples, Params, Notes" << endl;
  for (int i = 0; i < NB_CHAN; i++)
    {
      if ((res = read(fd, &tmp_long, sizeof(long))) != sizeof (long))
	{ cout << "[DRM31] Load: read error" << endl; return; }
      else
	size -= res;
      if (tmp_long > 0 && tmp_long < 255)
	{
	  char wave[tmp_long+1];
	  if ((res = read(fd, &wave, tmp_long * sizeof(char))) 
	      != (tmp_long * sizeof(char)))
	    { cout << "[DRM31] Load: read error" << endl; return; }
	  else
	    {
	      size -= res;
	      wave[tmp_long] = '\0';
	      try 
		{
		  w = new WaveFile(string(wave), true);
		}
	      catch (...)
		{
		  cout << "[DRM31] Could not load WaveFile: " << wave << endl;
		}
	      Channels[i]->SetWaveFile(w);
	    }
	}
      else if (tmp_long >= 255)
	{ cout << "[DRM31] Load: file name too long" << endl; return; }
      
      //reading channel parameters
      int p;
      for (p = 0; p < NB_PARAMS; p++)
	if ((res = read(fd, &tmp_float, sizeof(float))) != sizeof (float))
	  { cout << "[DRM31] Load: read error" << endl; return; }
	else
	  { size -= res; Channels[i]->Params[p] = tmp_float; }      
      
      //reading notes
      for (bank = 0; bank < 5; bank++)
	for (ps = 0; ps < 8; ps++)
	  {
	    if ((res = read(fd, &tmp_long, sizeof(long))) != sizeof (long))
	      { cout << "[DRM31] Load: read error" << endl; return; }
	    else
	      size -= res;
	    
	    while (tmp_long)
	      {
		if ((res = read(fd, &tmp_uint, sizeof (unsigned int))) 
		    != sizeof(unsigned int))
		  { cout << "[DRM31] Load: read error" << endl; return; }
		else
		  size -= res;
		if ((res = read(fd, &tmp_double, sizeof (double))) 
		    != sizeof(double))
		  { cout << "[DRM31] Load: read error" << endl; return; }
		else
		  size -= res;
		note = 
		  new BeatNote(i, tmp_double, tmp_uint, 0.0);
		for (p = 0; p < NB_PARAMS; p++ )
		  if ((res = read(fd, &tmp_float, sizeof (float))) 
		      != sizeof(float))
		    { cout << "[DRM31] Load: read error" << endl; return; }
		  else
		    { size -= res; note->Params[p] = tmp_float; }
		if ((res = read(fd, &tmp_int, sizeof (int))) != sizeof (int))
		  { cout << "[DRM31] Load: read error" << endl; return; }
		else
		  { size -= res; note->Reversed = p ? true : false; }
		
		Channels[i]->Rythms[bank][ps].push_back(note);
		tmp_long--;
	      }
	  }
    }
  
  cout << "[DRM31] Channels Loaded" << endl;
  
  SelectedChannel = Channels[0];
  ReCalcStepsSigCoef();
  UpdateSteps(0,0);
  SetPatternList();
  
  PatternMutex.Lock();
  OnLoading = false;
  PatternMutex.Unlock();
  
  cout << "[DRM31] Load remaining size " <<  size << endl;
}

inline void WiredBeatBox::LockLoading()
{
  PatternMutex.Lock();
  OnLoading = true;
  PatternMutex.Unlock();
}

inline void WiredBeatBox::UnlockLoading()
{
  PatternMutex.Lock();
  OnLoading = false;
  PatternMutex.Unlock();
}

void WiredBeatBox::OnEditButton(wxCommandEvent& WXUNUSED(e))
{
  OnEdit = !OnEdit;
  if (EditedPattern != NewSelectedPattern)
    {
      PatternMutex.Lock();
      NewSelectedPattern = EditedPattern;
      NewSelectedBank = EditedBank;
      PatternMutex.Unlock();
      SetPatternList();
    }
}


void WiredBeatBox::OnBankChange(wxCommandEvent& WXUNUSED(event))
{
  int bank = BankKnob->GetValue() - 1;
  
  if (OnEdit && (EditedBank != bank))
    {
      PatternMutex.Lock();
      EditedBank = bank;
      PatternMutex.Unlock();
      UpdateSteps(EditedBank, EditedPattern);
      SetPatternList();
    }
  else if (!OnEdit && (NewSelectedBank != bank))
    {
      PatternMutex.Lock();
      EditedBank = NewSelectedBank = bank;
      PatternMutex.Unlock();
      UpdateSteps(NewSelectedBank, NewSelectedPattern);
      SetPatternList();
    }
  if (View)
    View->Refresh();
}

void WiredBeatBox::OnStepsChange(wxCommandEvent& WXUNUSED(event))
{
  int steps = StepsKnob->GetValue();
  if (steps > 64)
    steps = 64;
  if (steps < 1)
    steps = 1;
  
  wxString s;
  s.Printf("%d", steps);
  StepsLabel->SetLabel(s);
  
  /*
    long spb = 
    static_cast<long>
    ((static_cast<double>(OldSamplesPerBar * steps_sig_coef )));
    double bps = OldBarsPerSample / StepsSigCoef;
  */
  
  unsigned int bank = (OnEdit ? EditedBank : NewSelectedBank);
  unsigned int track = (OnEdit ? EditedPattern : NewSelectedPattern);
  Steps[bank][track] = steps;
  double steps_sig_coef = static_cast<double>
    ( steps / static_cast<double>(SignatureDen[bank][track]));
  
  PatternMutex.Lock();
  StepsSigCoef[bank][track] = steps_sig_coef;
  SamplesPerBar[bank][track] = static_cast<long>
    ((static_cast<double>(OldSamplesPerBar * steps_sig_coef )));
  BarsPerSample[bank][track] = 
    OldBarsPerSample / StepsSigCoef[bank][track];
  UpdateNotesPositions(bank, track);
  PatternMutex.Unlock();
  
  if (View)
    View->Refresh();
}

inline void WiredBeatBox::UpdateStepsDeps(unsigned int steps)
{
  // this function has to be protected by PatternMutex
  Steps[EditedBank][EditedPattern] = steps;
  double steps_sig_coef = static_cast<double>
    ( steps / static_cast<double>(SignatureDen[EditedBank][EditedPattern]));
  
  StepsSigCoef[EditedBank][EditedPattern] = steps_sig_coef;
  SamplesPerBar[EditedBank][EditedPattern] = static_cast<long>
    ((static_cast<double>(OldSamplesPerBar * steps_sig_coef )));
  BarsPerSample[EditedBank][EditedPattern] = 
    OldBarsPerSample / StepsSigCoef[EditedBank][EditedPattern];
  UpdateNotesPositions(EditedBank, EditedPattern);
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
  View = new BeatBoxView(zone, ID_VIEW_ACT, this, pos, size, &PatternMutex);
  Connect(ID_VIEW_ACT, wxEVT_COMMAND_BUTTON_CLICKED,
	  (wxObjectEventFunction)(wxEventFunction) 
	  (wxCommandEventFunction)&WiredBeatBox::OnViewAction);
  return View;
}

void WiredBeatBox::DestroyView()
{
  View->Destroy();
  View = 0x0;
}

void WiredBeatBox::OnViewAction(wxCommandEvent& event)
{
  //cout << "VIEW ACTION" << endl;
}

int WiredBeatBox::GetSteps(void)
{
  return Steps[EditedBank][EditedPattern];
}

void WiredBeatBox::CheckExistingControllerData(int data[3])
{
  if ((MidiVolume[0] == data[0]) && (MidiVolume[1] == data[1]))
    {
      MidiVolume[0] = -1;
      return;
    }
  else if ((MidiSteps[0] == data[0]) && (MidiSteps[1] == data[1]))
    {
      MidiSteps[0] = -1;
    }
  else
    for (int i = 0; i < NB_CHAN; i++)
      {
	if ((Channels[i]->MidiVolume[0] == data[0]) && 
	    (Channels[i]->MidiVolume[1] == data[1]))
	  {
	    Channels[i]->MidiVolume[0] = -1;
	    return;
	  }
	else if ((Channels[i]->MidiPan[0] == data[0]) && 
		 (Channels[i]->MidiPan[1] == data[1]))
	  {
	    Channels[i]->MidiPan[0] = -1;
	    return;
	  }
	else if ((Channels[i]->MidiPitch[0] == data[0]) && 
		 (Channels[i]->MidiPitch[1] == data[1]))
	  {
	    Channels[i]->MidiPitch[0] = -1;
	    return;
	  }
	else if ((Channels[i]->MidiVel[0] == data[0]) && 
		 (Channels[i]->MidiVel[1] == data[1]))
	  {
	    Channels[i]->MidiVel[0] = -1;
	    return;
	  }
	else if ((Channels[i]->MidiStart[0] == data[0]) && 
		 (Channels[i]->MidiStart[1] == data[1]))
	  {
	    Channels[i]->MidiStart[0] = -1;
	    return;
	  }
	else if ((Channels[i]->MidiEnd[0] == data[0]) && 
		 (Channels[i]->MidiEnd[1] == data[1]))
	  {
	    Channels[i]->MidiEnd[0] = -1;
	    return;
	  }
	else if ((Channels[i]->MidiPoly[0] == data[0]) && 
	  (Channels[i]->MidiPoly[1] == data[1]))
	{
	  Channels[i]->MidiPoly[0] = -1;
	  return;
	}
    }
}

void WiredBeatBox::OnVolumeController(wxMouseEvent& WXUNUSED(event))
{
  int *midi_data;
  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      //Mutex.Lock();
      CheckExistingControllerData(midi_data);      
      MidiVolume[0] = midi_data[0];
      MidiVolume[1] = midi_data[1];
      //Mutex.Unlock();
    }
  delete midi_data;
}

void WiredBeatBox::OnStepsController(wxMouseEvent& WXUNUSED(event))
{
  int *midi_data;
  midi_data = new int[3];
  if (ShowMidiController(&midi_data))
    {
      //Mutex.Lock();
      CheckExistingControllerData(midi_data);      
      MidiSteps[0] = midi_data[0];
      MidiSteps[1] = midi_data[1];
      //Mutex.Unlock();
    }
  delete midi_data;
}

void WiredBeatBox::Update()
{
  if (AskUpdateSteps)
    {
      AskUpdateSteps = false;
      UpdateSteps(EditedBank, EditedPattern);
      if (View)
	View->Refresh();
    }
  if (AskUpdateLevel)
    {
      AskUpdateLevel = false;
      MVol->SetValue(MidiVolume[2]);
    }
  if (AskUpdateBank)
    {
      AskUpdateBank = false;
      PatternMutex.Lock();
      int bank = NewSelectedBank;
      int pat = NewSelectedPattern;
      PatternMutex.Unlock();
            
      BankKnob->SetValue(bank + 1);
      UpdateSteps(bank, pat);
      SetPatternList();

    }
  if (AskUpdatePattern)
    {
      AskUpdatePattern = false;
      
      PatternMutex.Lock();
      int bank = NewSelectedBank;
      int pat = NewSelectedPattern;
      PatternMutex.Unlock();
      
      for (int p = 0; p < NUM_PATTERNS; p++)
	PatternSelectors[p]->SetOff();
      PatternSelectors[pat]->SetOn();
      UpdateSteps(bank, pat);
      SetPatternList();
    }
  for (int i = 0; i < NB_CHAN; i++)
    if (Channels[i]->AskUpdateChannel)
      {
	Channels[i]->Update();
	Channels[i]->AskUpdateChannel = false;
      }
}

void	WiredBeatBox::OnCopyPattern(wxCommandEvent& event)
{
  cout << "copy pattern !!" << endl;
}

void	WiredBeatBox::OnRightDown(wxMouseEvent& event)
{
  cout << "wouuuuuuuuuuuuuuw" << endl;
}

/******** Main and mandatory library functions *********/

extern "C"
{
  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "WBBE");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_INSTR;  
    info.UnitsX = 4;
    info.UnitsY = 4;
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
