#include "BeatBoxView.h"

#include "BeatBox.h"

BEGIN_EVENT_TABLE(BeatBoxView, wxPanel)
  //  EVT_BUTTON(BeatBoxView_Env, BeatBoxView::EnvView)
END_EVENT_TABLE()

BeatBoxView::BeatBoxView(wxWindow* parent, WiredBeatBox* bb,
			 const wxPoint& pos, const wxSize& size, 
			 wxMutex* mutex)
  : wxPanel(parent, -1, pos, size)
{
  Mutex = mutex;
  BB = bb;
  int s = BB->GetSig();
  cout << s << endl;
  
}

BeatBoxView::~BeatBoxView()
{}

void BeatBoxView::SetWaveFile(WaveFile* w)
{
  /*
  Env = new WaveEnv(this, BeatBoxView_Env, wxPoint(0,0), wxSize(GetSize().x, GetSize().y));
  
  wxBoxSizer *sizer2 = new wxBoxSizer(wxVERTICAL);
  //sizer2->Add(Rule, 0, wxEXPAND | wxALL, 0); 
  sizer2->Add(Env, 1, wxEXPAND | wxALL, 0); 
  
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  //sizer->Add(Toolbar, 0, wxEXPAND | wxALL, 0); 
  sizer->Add(sizer2, 1, wxEXPAND | wxALL, 0); 
  
  SetSizer(sizer);
  Wave = w;
  
  Env->SetWave(w);
  */
}
