#include "CompressorPlug.h"

static PlugInitInfo info;

/******** CompressorPlugin Implementation *********/

BEGIN_EVENT_TABLE(CompressorPlugin, wxWindow)
  EVT_COMMAND_SCROLL(Compressor_Target, CompressorPlugin::OnTarget)
  EVT_COMMAND_SCROLL(Compressor_Res, CompressorPlugin::OnResonance)
  EVT_PAINT(CompressorPlugin::OnPaint)
END_EVENT_TABLE()

CompressorPlugin::CompressorPlugin(PlugStartInfo &startinfo, PlugInitInfo *initinfo)
  : Plugin(startinfo, initinfo)
{
  Init();

  wxImage *tr_bg = 
    new wxImage(string(GetDataDir() + string(IMG_CP_BG)).c_str(), wxBITMAP_TYPE_PNG);
  if (tr_bg)
    TpBmp = new wxBitmap(tr_bg);
  
  bmp = new wxBitmap(string(GetDataDir() + string(IMG_CP_BMP)).c_str(), wxBITMAP_TYPE_BMP); 

  img_bg = new wxImage(string(GetDataDir() + string(IMG_CP_FADER_BG)).c_str(), wxBITMAP_TYPE_PNG );
  img_fg = new wxImage(string(GetDataDir() + string(IMG_CP_FADER_FG)).c_str(), wxBITMAP_TYPE_PNG );
  
  TargetFader = new 
    FaderCtrl(this, Compressor_Target, img_bg, img_fg, 0, 100, 100,
	      wxPoint(18, 8)/*wxPoint(GetSize().x / 2, 10)*/, wxSize(22,78));
  ResFader = new 
    FaderCtrl(this, Compressor_Res, img_bg, img_fg, 0, 100, 0,
	      wxPoint(142, 8)/*wxPoint(GetSize().x / 2 + 40, 10)*/, wxSize(22,78));
  
  SetBackgroundColour(wxColour(237, 237, 237));
}


void CompressorPlugin::Init()
{

}

#define IS_DENORMAL(f) (((*(unsigned int *)&f)&0x7f800000)==0)

void CompressorPlugin::Process(float **input, float **output, long sample_length)
{
  
}

CompressorPlugin::~CompressorPlugin()
{

}

bool CompressorPlugin::IsAudio()
{
  return (true);
}

bool CompressorPlugin::IsMidi()
{
  return (true);
}

wxBitmap *CompressorPlugin::GetBitmap()
{
  return (bmp);
}

void CompressorPlugin::OnTarget(wxScrollEvent &e)
{
  Target = - (100 - TargetFader->GetValue());
  //if (Target <= 0.f)
  //Target = 0.01f;
  cout << "Target: " << Target << "; Res: " << Res << endl;
}
  
void CompressorPlugin::OnResonance(wxScrollEvent &e)
{
  Res = ResFader->GetValue() / 100.f;
  cout << "Target: " << Target << "; Res: " << Res << endl;
}

void CompressorPlugin::OnPaint(wxPaintEvent &event)
{
  wxMemoryDC memDC;
  wxPaintDC dc(this);
  
  memDC.SelectObject(*TpBmp);    
  wxRegionIterator upd(GetUpdateRegion()); // get the update rect list   
  while (upd)
    {    
      dc.Blit(upd.GetX(), upd.GetY(), upd.GetW(), upd.GetH(), &memDC, upd.GetX(), upd.GetY(), 
	      wxCOPY, FALSE);      
      upd++;
    }
  Plugin::OnPaintEvent(event);
}

/******** Main and mandatory library functions *********/

extern "C"
{

  PlugInitInfo init()
  {  
    WIRED_MAKE_STR(info.UniqueId, "COMP");
    info.Name = PLUGIN_NAME;
    info.Type = PLUG_IS_EFFECT;  
    info.UnitsX = 1;
    info.UnitsY = 1;
    return (info);
  }

  Plugin *create(PlugStartInfo *startinfo)
  {
    Plugin *p = new CompressorPlugin(*startinfo, &info);
    return (p);
  }

  void destroy(Plugin *p)
  {
    delete p;
  }

}








