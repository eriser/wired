#include "BeatBox.h"
#include <wx/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include "midi.h"

#define NB_CHAN 11

#define DELETE_RYTHMS(R) {						\
                          for (unsigned char bank = 0; bank < 5; bank++)\
			    {						\
			      for (unsigned char ps = 0; ps < 8; ps++)	\
			        {					\
			         for (list<BeatNote*>::iterator		\
				      bn = R[bank][ps].begin();		\
				      bn != R[bank][ps].end();)		\
				   {					\
				     delete *bn;			\
				     bn = R[bank][ps].erase(bn);	\
				   }					\
			        }					\
			      delete [] R[bank];			\
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
  EVT_ENTER_WINDOW(WiredBeatBox::OnHelp)
END_EVENT_TABLE()
  

WiredBeatBox::WiredBeatBox(PlugStartInfo &startinfo, PlugInitInfo *initinfo) 
  : Plugin(startinfo, initinfo)
{
  cout << "[WIREDBEATBOX] Host is " << GetHostProductName()
       << " version " << GetHostProductVersion() << endl;
  
  /* Master Volume */
  MVol = 
    new HintedKnob(this, BB_OnMasterChange, this,
		 new wxImage(_T(string(GetDataDir() + string(KNOB)).c_str()),
			     wxBITMAP_TYPE_PNG),
		   new wxImage(_T(string(GetDataDir() + string(DOT)).c_str()),
			       wxBITMAP_TYPE_PNG),
		   0, 127, 100, 1,
		   wxPoint(27,103), wxSize(29,30), wxPoint(52,128));
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
  
  

  Imgs = new wxImage*[MAX_BITMAPS];
  Imgs[ID_UNCLICKED] = new wxImage(_T(string(GetDataDir() + string(BEATBTN_UNCLICKED)).c_str()), wxBITMAP_TYPE_PNG);
  Imgs[ID_MEDIUM] = new wxImage(_T(string(GetDataDir() + string(BEATBTN_MEDIUM)).c_str()), wxBITMAP_TYPE_PNG);
  Imgs[ID_VLOW] = new wxImage(_T(string(GetDataDir() + string(BEATBTN_VLOW)).c_str()), wxBITMAP_TYPE_PNG);
  Imgs[ID_LOW] = new wxImage(_T(string(GetDataDir() + string(BEATBTN_LOW)).c_str()), wxBITMAP_TYPE_PNG);
  Imgs[ID_HIGH] = new wxImage(_T(string(GetDataDir() + string(BEATBTN_HIGH)).c_str()), wxBITMAP_TYPE_PNG);
  Imgs[ID_VHIGH] = new wxImage(_T(string(GetDataDir() + string(BEATBTN_VHIGH)).c_str()), wxBITMAP_TYPE_PNG);
  
  
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
  
  PositionLeds = new wxStaticBitmap*[16];
  
  for (int i = 0, x = 144; i < 16; i++)
    {
      
      if (i % 4)
	x += BTN_SIZE + 2;
      else if (i != 0)
	x += BTN_SIZE + 10;
      
      PositionLeds[i] = 
	new wxStaticBitmap(this, -1, *PositionOff, 
			   wxPoint(x+BTN_SIZE/2, 246), wxSize(6,6));
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
  int notenum = 0x48; //C3
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
    }
  SelectedChannel = Channels[0];
  Channels[0]->Select();
  
  /* Polyphony */
  Pool = new Polyphony();
  Pool->SetPolyphony(64);

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
  BufferSize = size;
  Pool->SetBufferSize(size);
}

inline void WiredBeatBox::RefreshPosLeds(double bar_pos)
{
  /*
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
    
  */
}





void WiredBeatBox::Process(float** WXUNUSED(input), float **output, long sample_length)
{
  PatternMutex.Lock();
  
  if (OldSamplesPerBar != GetSamplesPerBar() ||
      OldBarsPerSample != GetBarsPerSample())
    {
      OldSamplesPerBar = GetSamplesPerBar();
      OldBarsPerSample = GetBarsPerSample();
      
      SamplesPerBar[SelectedBank][SelectedPattern] =
	static_cast<long>
	((static_cast<double>(OldSamplesPerBar * StepsSigCoef[SelectedBank][SelectedPattern])));
      BarsPerSample[SelectedBank][SelectedPattern] = OldBarsPerSample / StepsSigCoef[SelectedBank][SelectedPattern];
      
      if (SelectedBank != NewSelectedBank || 
	  SelectedPattern != NewSelectedPattern)
	{
	  SamplesPerBar[NewSelectedBank][NewSelectedPattern] =
	    static_cast<long>((static_cast<double>(OldSamplesPerBar * StepsSigCoef[NewSelectedBank][NewSelectedPattern])));
	  BarsPerSample[NewSelectedBank][NewSelectedPattern] =
	    OldBarsPerSample/StepsSigCoef[NewSelectedBank][NewSelectedPattern];
	}
    }
  
  double bar_pos = fmod( (GetBarPos() / StepsSigCoef[SelectedBank][SelectedPattern]), 1.0 );
  double bar_end = static_cast<double>(bar_pos + static_cast<double>(sample_length * BarsPerSample[SelectedBank][SelectedPattern]));
  
  double new_bar_end, new_bar_pos;
  if (SelectedBank != NewSelectedBank || 
      SelectedPattern != NewSelectedPattern)
    {
      new_bar_pos = fmod( (GetBarPos() / StepsSigCoef[NewSelectedBank][NewSelectedPattern]), 1.0 );
      new_bar_end = static_cast<double>(new_bar_pos + static_cast<double>(sample_length * BarsPerSample[NewSelectedBank][NewSelectedPattern]));
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
      if (SelectedChannel->IsSolo && !SelectedChannel->Muted)
	GetNotesFromChannel(SelectedChannel, bar_pos, bar_end, 
			    new_bar_pos, new_bar_end, &isend);
      else
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
  //jouage des notes
  long len = 0;
  long newoffset = 0;
  float curvel, velstep;
  for (list<BeatNoteToPlay*>::iterator bn = NotesToPlay.begin(); 
       bn != NotesToPlay.end();  bn++)
    {
      len = sample_length - (*bn)->Delta;
      
      if ( !Channels[(*bn)->NumChan]->Wave )
	{
	  continue;
	}      
      
      if ( (*bn)->OffSet >= 
	   Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames() ||
	   (*bn)->OffSet >= (*bn)->SEnd )
	{
	  //memset((*bn)->Buffer[0], 0, sample_length * sizeof(float));
	  //memset((*bn)->Buffer[1], 0, sample_length * sizeof(float));
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
      
      //assert(end <= Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames());
      
      if ( (*bn)->OffSet + len > end )
	   //Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames()) 	
	{
	  //cout << "finishing sound1\n" << endl;
	  len = end - (*bn)->OffSet;
	  if (len <= 0)
	    {
	      //cout << " len < 0 !! " << endl;
	      continue;
	    }
	}
      /*else if ((*bn)->OffSet + len == end )
	{
	  cout << "finishing sound2\n" << endl;
	}
      */
      Channels[(*bn)->NumChan]->Wave->SetPitch((*bn)->Pitch);
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
	      //printf("vel < 0.f     break\n");
	      break;
	    }
	  //printf("before b0 b1: %f %f\n", (*bn)->Buffer[0][i], (*bn)->Buffer[1][i]);
	  
	  (*bn)->Buffer[0][i] *= Channels[(*bn)->NumChan]->Lev * curvel * Channels[(*bn)->NumChan]->Pan[0];
	  (*bn)->Buffer[1][i] *= Channels[(*bn)->NumChan]->Lev * curvel * Channels[(*bn)->NumChan]->Pan[1];
	  //printf("after b0 b1: %f %f\n", (*bn)->Buffer[0][i], (*bn)->Buffer[1][i]);
	  curvel -= velstep;
	  //printf("curvel=%f\n", curvel);
	  
	}
    }
  Pool->GetMix(output);
  
  // effacage des notes finies
  for (list<BeatNoteToPlay*>::iterator bn = NotesToPlay.begin(); 
       bn != NotesToPlay.end(); )
    {
      if ( !(Channels[(*bn)->NumChan]->Wave) || 
	   ((*bn)->OffSet >= 
	    Channels[(*bn)->NumChan]->Wave->GetNumberOfFrames()) )
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
	  for (i = 0; i < NB_CHAN; i++)
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

inline void WiredBeatBox::SetNoteAttr(BeatNoteToPlay* note, BeatBoxChannel* c)
{
  note->Lev *= c->Lev;
  note->Vel *= c->Vel;
  note->Pitch *= c->Pitch;
  note->Start *= c->Start;
  note->End *= c->End;
  note->Reversed = c->Reversed;
  
  note->OffSet =
    static_cast<unsigned long>
    (floor(c->Wave->GetNumberOfFrames() * (note->Start)));
  note->SEnd = 
    static_cast<unsigned long>
    (floor(c->Wave->GetNumberOfFrames() * note->End));
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
	  for (list<BeatNote*>::iterator bn = c->Rythms[NewSelectedBank][NewSelectedPattern].begin();
	       bn != c->Rythms[NewSelectedBank][NewSelectedPattern].end(); bn++)
	    {
	      if ((*bn)->BarPos > lasts)
		break;
	      if ( static_cast<unsigned int>(Pool->GetCount())
		   < NotesToPlay.size()+1 )
		Pool->SetPolyphony(NotesToPlay.size()+32);
	      
	      delta = static_cast<unsigned long>
		( ((1.0 - new_bar_pos + (*bn)->BarPos) * SamplesPerBar[NewSelectedBank][NewSelectedPattern]) );
	      note = 
		new BeatNoteToPlay(*bn, c->Id, delta, Pool->GetFreeBuffer());	  
	      SetNoteAttr(note, c);
	      NotesToPlay.push_back(note);
	    }
	}
      else
	//GetNotesFromChannel(c, new_bar_pos, new_bar_end, new_bar_pos, new_bar_end, isend);
	{
	  for (list<BeatNote*>::iterator bn = c->Rythms[NewSelectedBank][NewSelectedPattern].begin();
	       bn != c->Rythms[NewSelectedBank][NewSelectedPattern].end(); bn++)
	    {
	      if ((*bn)->BarPos >= 1.0)
		{
		  break;
		}
	      else if ((*bn)->BarPos >= new_bar_pos && (*bn)->BarPos < new_bar_end)
		{
		  if ( static_cast<unsigned int>(Pool->GetCount())
		       < NotesToPlay.size()+1 )
		    Pool->SetPolyphony(NotesToPlay.size()+32);
		  
		  delta = static_cast<unsigned long>
		    ( (((*bn)->BarPos - new_bar_pos) * SamplesPerBar[NewSelectedBank][NewSelectedPattern]) );

		  
		  note = 
		    new BeatNoteToPlay(*bn, c->Id, delta, Pool->GetFreeBuffer());
		  SetNoteAttr(note, c);
		  NotesToPlay.push_back(note);
		}
	    }
	}
    }
  
  for (list<BeatNote*>::iterator bn = c->Rythms[SelectedBank][SelectedPattern].begin();
       bn != c->Rythms[SelectedBank][SelectedPattern].end(); bn++)
    {
      if ((*bn)->BarPos >= 1.0)
	{
	  break;
	}
      else if ((*bn)->BarPos >= bar_pos && (*bn)->BarPos < bar_end)
	{
	  if ( static_cast<unsigned int>(Pool->GetCount())
	       < NotesToPlay.size()+1 )
	    Pool->SetPolyphony(NotesToPlay.size()+32);
	  
	  delta = static_cast<unsigned long>
	    ( (((*bn)->BarPos - bar_pos) * SamplesPerBar[SelectedBank][SelectedPattern]) );
	  
	  note = 
	    new BeatNoteToPlay(*bn, c->Id, delta, Pool->GetFreeBuffer());
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
      for (unsigned char i = 0; i < NB_CHAN; i++)
	{
	  Channels[i]->DeSelect();
	  Channels[i]->UnSolo();
	}
      tmp->Select();
      
      PatternMutex.Lock();
      tmp->Solo();
      SelectedChannel = tmp;
      PatternMutex.Unlock();
      
      SetPatternList();
      break;
    case ACT_PLAY:
      if (!tmp->Wave)
	{break;}
      if ( static_cast<unsigned int>(Pool->GetCount())
	   < NotesToPlay.size()+1 )
	Pool->SetPolyphony(NotesToPlay.size()+32);
      bn = new BeatNoteToPlay(tmp, Pool->GetFreeBuffer());
      
      PatternMutex.Lock();
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
  
  //PatternMutex.Lock();
  if (OnEdit && (EditedPattern != *p))
    {
      PatternSelectors[SelectedPattern]->SetOff();
      PatternSelectors[NewSelectedPattern]->SetOff();
      PatternSelectors[EditedPattern]->SetOff();
      
      PatternMutex.Lock();
      EditedPattern = *p;
      //PatternMutex.Unlock();
      
      PatternSelectors[EditedPattern]->SetOn();
      
      UpdateSteps(EditedBank, EditedPattern);
      //PatternMutex.Lock();
      SetPatternList();
      PatternMutex.Unlock();
    }
  else if (!OnEdit && (NewSelectedPattern != *p))
    {
      PatternSelectors[SelectedPattern]->SetOff();
      PatternSelectors[NewSelectedPattern]->SetOff();
      
      PatternMutex.Lock();
      EditedPattern = NewSelectedPattern = *p;
      //PatternMutex.Unlock();
      
      PatternSelectors[NewSelectedPattern]->SetOn();
      UpdateSteps(NewSelectedBank, NewSelectedPattern);
      
      //PatternMutex.Lock();
      SetPatternList();
      PatternMutex.Unlock();
    }
  PatternMutex.Unlock();
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

inline void WiredBeatBox::AddBeatNote(BeatBoxChannel* c,
				      double rel_pos, unsigned int state)
{
  //PatternMutex.Lock();
  unsigned int bank = (OnEdit ? EditedBank : NewSelectedBank);
  unsigned int cur = (OnEdit ? EditedPattern : NewSelectedPattern);
  //PatternMutex.Unlock();
  
  double bar_pos = 
    static_cast<double>( rel_pos / static_cast<double>(Steps[bank][cur]) );
    
  if (c->Rythms[bank][cur].empty())
    {
      BeatNote *note = new BeatNote(rel_pos, state, bar_pos);
      note->Vel = 1.0f * static_cast<float>(state / 4.f);
      note->Reversed = false;
      
      PatternMutex.Lock();
      c->Rythms[bank][cur].push_back(note);
      PatternMutex.Unlock();
      return;
    }
  if ( rel_pos > c->Rythms[bank][cur].back()->Position ) 
    {
      BeatNote *note = new BeatNote( rel_pos, state, bar_pos);
      note->Vel = 1.0f * static_cast<float>(state / 4.f);
      note->Reversed = false;
      
      PatternMutex.Lock();
      c->Rythms[bank][cur].push_back(note);
      PatternMutex.Unlock();
      return;
    }
  
  
  for ( list<BeatNote*>::iterator b = c->Rythms[bank][cur].begin();
	b != c->Rythms[bank][cur].end(); b++)
    {  
      if ( (*b)->Position == rel_pos ) 
	{
	  if ( state == ID_UNCLICKED )
	    {
	      BeatNote* note = *b;
	      PatternMutex.Lock();
	      c->Rythms[bank][cur].erase(b);
	      PatternMutex.Unlock();
	      delete note;
	      
	    }
	  else
	    {
	      PatternMutex.Lock();
	      (*b)->State = state;
	      (*b)->Vel = 1.f * static_cast<float>(state / 4.f);
	      PatternMutex.Unlock();
	    }
	  return;
	}
      else if ( (*b)->Position > rel_pos ) 
	{
	  BeatNote *note = new BeatNote(rel_pos, state, bar_pos);
	  note->Vel = 1.0f * static_cast<float>(state / 4.f);
	  note->Reversed = false;
	  PatternMutex.Lock();
	  c->Rythms[bank][cur].insert(b, note);
	  PatternMutex.Unlock();
	  return;
	}
    }
}

void WiredBeatBox::OnPatternClick(wxCommandEvent &e)
{
  int* tmp = (int*)e.GetClientData();
  
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
      for (int chan = 0; chan < NB_CHAN; chan++)
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
  int steps, sig_index = 0;
  int bank, ps;
  
  PatternMutex.Lock();
  
  for (int i = 0; i < NB_CHAN; i++)
    {
      if (Channels[i]->Wave)
	{
	  len = Channels[i]->Wave->Filename.size();
	  size += write(fd, &len, sizeof(long));
	  size += write(fd, Channels[i]->Wave->Filename.c_str(), len);
	  /*
	    cout << "writing channel: " << i 
	       << " file len: " << len << " name: " 
	       << Channels[i]->Wave->Filename.c_str() << endl;
	  */
	}
      else
	{
	  len = 0;
	  size += write(fd, &len, sizeof(long));
	}
      for (bank = 0; bank < 5; bank++)
	for (ps = 0; ps < 8; ps++)
	  {
	    sig_index = SigIndex[bank][ps];
	    steps = Steps[bank][ps];
	    
	    size += write(fd, &sig_index, sizeof(int));
	    size += write(fd, &steps, sizeof(int));
	    
	    len = Channels[i]->Rythms[bank][ps].size();
	    size += write(fd, &len, sizeof(long));
	    /*
	      cout << "writing "<<len << " notes for rythm["<< bank 
		 << "][" << ps << "]" 
		 << endl;
	    */
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
	      
	      }
	  }
    }
  PatternMutex.Unlock();
  cout << "saved size " << size << endl;
  return size;
}

void WiredBeatBox::Load(int fd, long size)
{
  long len;
  int bank,  ps;
  unsigned int state = 0;
  double pos = 0;
  BeatNote* note;
  WaveFile* w;
  
  int sig_index, steps = 0;
  
  
  //PatternMutex.Lock();
  
  for (int i = 0; i < NB_CHAN; i++)
    {
      size -= read(fd, &len, sizeof(long));
      if (len > 0 && len < 255)
	{
	  char wave[len+1];
	  size -= read(fd, &wave, len*sizeof(char));
	  wave[len] = '\0';
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
      
      PatternMutex.Lock();
      
      for (bank = 0; bank < 5; bank++)
	for (ps = 0; ps < 8; ps++)
	  {
	    size -= read(fd, &sig_index, sizeof (int));
	    size -= read(fd, &steps, sizeof (int));
	    
	    Steps[bank][ps] = steps;
	    SigIndex[bank][ps] = sig_index;
	    SignatureDen[bank][ps] = SigDen[sig_index];
	    Signature[bank][ps] = Signatures[sig_index];
	    
	    size -= read(fd, &len, sizeof(long));
	    /*
	      cout << "reading "<<len << " notes for rythm["<< bank 
	      << "][" << ps << "]" 
	      << endl;
	    */
	    while (len)
	      {
		size -= read(fd, &state, sizeof(unsigned int));
		size -= read(fd, &pos, sizeof(double));
		//printf("note pos=%f; state=%d\n", pos,state);
		note = 
		  new BeatNote(pos, state, 
			       0.0);
		/*
		  static_cast<double>
		  ( pos / 
		  static_cast<double>
		  (Steps[bank][ps])));
		*/
		Channels[i]->Rythms[bank][ps].push_back(note);
		len--;
	      }
	    
	    
	  }
      PatternMutex.Unlock();
    }
  
  PatternMutex.Lock();
  
  SelectedChannel = Channels[0];
  ReCalcStepsSigCoef();
  UpdateSteps(0,0);
  SetPatternList();
  
  PatternMutex.Unlock();
  printf("size:%d\n", size);
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
  //SamplesPerBar = spb;
  //BarsPerSample = bps;
  SamplesPerBar[bank][track] = static_cast<long>
    ((static_cast<double>(OldSamplesPerBar * steps_sig_coef )));
  BarsPerSample[bank][track] = 
    OldBarsPerSample / StepsSigCoef[bank][track];
  UpdateNotesPositions(bank, track);
  PatternMutex.Unlock();
  
  if (View)
    View->Refresh();
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
  cout << "VIEW ACTION" << endl;
}

int WiredBeatBox::GetSteps(void)
{
  return Steps[EditedBank][EditedPattern];
}

int WiredBeatBox::GetSig()
{
  int s;
  PatternMutex.Lock();
  //  s = 
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
