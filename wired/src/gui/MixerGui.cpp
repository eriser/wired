// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <math.h>
#include "VUMCtrl.h"
#include "Colour.h"
#include "MixerGui.h"
#include "ChannelGui.h"
#include "../mixer/Channel.h"
#include "../mixer/Mixer.h"
#include "../engine/Settings.h"

MixerGui				*MixerPanel = NULL;

BEGIN_EVENT_TABLE(MixerGui, wxScrolledWindow)
END_EVENT_TABLE()

MixerGui::MixerGui(wxWindow *parent, const wxPoint &pos, const wxSize &size)
  : wxScrolledWindow(parent, -1, pos, size, wxNO_BORDER)//SUNKEN_BORDER)
{
  SetScrollRate(10, 0);
  SetVirtualSize(300, 131);
  SetBackgroundColour(*wxBLACK);//CL_RULER_BACKGROUND);
  ImgFaderBg = new wxImage(wxString(WiredSettings->DataDir + wxString(FADERBG)).c_str(), wxBITMAP_TYPE_PNG );
  ImgFaderFg = new wxImage(wxString(WiredSettings->DataDir + wxString(FADERFG)).c_str(), wxBITMAP_TYPE_PNG );
  ImgLockUp = new wxImage(wxString(WiredSettings->DataDir + wxString(MIXERLOCKUP)).c_str(), wxBITMAP_TYPE_PNG );
  ImgLockDown = new wxImage(wxString(WiredSettings->DataDir + wxString(MIXERLOCKDOWN)).c_str(), wxBITMAP_TYPE_PNG );
  ImgHpUp = new wxImage(wxString(WiredSettings->DataDir + wxString(MIXERHPUP)).c_str(), wxBITMAP_TYPE_PNG );
  ImgHpDown = new wxImage(wxString(WiredSettings->DataDir + wxString(MIXERHPDOWN)).c_str(), wxBITMAP_TYPE_PNG );
  /*
    Adding Master Channel directly
   */
  Channel *c = new Channel(true);
  AddMasterChannel(c);
  // evenement refresh master volume
  Connect(ID_MIXER_REFRESH, TYPE_MIXER_REFRESH, (wxObjectEventFunction)&MixerGui::OnMasterChange);
}

MixerGui::~MixerGui()
{
  
}

void MixerGui::OnMasterChange(wxCommandEvent &event)
{
  float l, r;
  
  MixMutex.Lock();
  l = MasterLeft;
  r = MasterRight;
  MixMutex.Unlock();
  
  l = (20.f * static_cast<float>(log10( l )));
  r =  (20.f * static_cast<float>(log10( r )));
  
  l = ((l + 96.f) / 96.f) * 100.f;
  r = ((r + 96.f) / 96.f) * 100.f;
  vuMasterLeft->SetValue(static_cast<long>(floor(l)));
  vuMasterRight->SetValue(static_cast<long>(floor(r)));
  
  vector<ChannelGui*>::iterator cg = ChannelGuiVector.begin();
  for (cg++; cg != ChannelGuiVector.end(); cg++)
    (*cg)->UpdateScreen();
}


void MixerGui::AddMasterChannel(Channel *channel)
{
  MasterChannelGui *gui = new MasterChannelGui(channel, ImgFaderBg,
					       ImgFaderFg, this, -1, 
					       wxPoint(0, 0),
					       wxSize(CHANNELGUI_WIDTH, 
						      CHANNELGUI_HEIGHT));
  
  SetVirtualSize(CHANNELGUI_WIDTH, CHANNELGUI_HEIGHT);
  ChannelGuiVector.push_back(gui);
  
  vuMasterLeft = gui->VumLeft;
  vuMasterRight = gui->VumRight;
}

ChannelGui* MixerGui::AddChannel(Channel *channel, const wxString& label)
{
  int x = /* CHANNELGUI_WIDTH +*/ 
    ChannelGuiVector.size() * CHANNELGUI_WIDTH;
  ChannelGui *gui = new ChannelGui(channel, ImgFaderBg, ImgFaderFg, 
				   this, -1, wxPoint(x, 0),
				   wxSize(CHANNELGUI_WIDTH, 
					  CHANNELGUI_HEIGHT),
				   label);
  
  
  ChannelGuiVector.push_back(gui);
  UpdateChannelsPos();
  return gui;
}

void MixerGui::RemoveChannel(Channel *channel)
{
  for (vector<ChannelGui*>::iterator cg = ChannelGuiVector.begin();
       cg != ChannelGuiVector.end(); cg++)
    if ((*cg)->Chan == channel)
      {
	delete *cg;
	ChannelGuiVector.erase(cg);
	break;
      }
  UpdateChannelsPos();
}

void MixerGui::RemoveChannel(ChannelGui *gui)
{
  for (vector<ChannelGui*>::iterator cg = ChannelGuiVector.begin();
       cg != ChannelGuiVector.end(); cg++)
    if ((*cg) == gui)
      {
	delete *cg;
	ChannelGuiVector.erase(cg);
	break;
      }
  UpdateChannelsPos();
}

void MixerGui::UpdateChannelsPos()
{
  SetVirtualSize((CHANNELGUI_WIDTH * ChannelGuiVector.size()), 
		 CHANNELGUI_HEIGHT);
  int xpos, ypos, x = 0, y = 0;
  for (vector<ChannelGui*>::iterator cg = ChannelGuiVector.begin();
       cg != ChannelGuiVector.end(); cg++, x += CHANNELGUI_WIDTH)
    {
      CalcScrolledPosition(x, y, &xpos, &ypos);
      (*cg)->SetPosition(wxPoint(xpos, ypos));
    }

}

ChannelGui* MixerGui::GetGuiByChan(Channel *c)
{
  for (vector<ChannelGui*>::iterator cg = ChannelGuiVector.begin();
       cg != ChannelGuiVector.end(); cg++)
    if ((*cg)->Chan == c)
      return *cg;
  return (0x0); 
}

void MixerGui::SetLabelByChan(Channel *channel, const wxString& label)
{
  ChannelGui* cg = GetGuiByChan(channel);
  cg->SetLabel(label);
}

/*
  void MixerGui::SetChanOpt(Track *tr)
  {
  if (!tr->IsAudioTrack())
  return;
  ChannelGui* cg = GetGuiByChan(tr->Output);
  cg->SetOpt(tr);
  }
*/
