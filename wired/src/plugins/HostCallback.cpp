// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License#include "HostCallback.h"

#include "Rack.h"
#include "Sequencer.h"
#include "OptionPanel.h"
#include "HelpPanel.h"
#include "Settings.h"
#include "FileLoader.h"

#include <string>
#include <iostream>

extern Rack		*RackPanel;

long HostCallback(Plugin *plug, long param, void *value)
{
  switch (param)
    {
    case wiredHostProductName :
      {
	strcpy((char *)value, "Wired");
	break;
      }
    case wiredHostProductVersion :
      {
	*(float *)value = 0.1f;
	break;
      }
    case wiredHostVendorName :
      {
	strcpy((char *)value, "P31");
	break;
      }
    case wiredGetDataDir :
      {
	std::string *s;
	s = (string *)value;
	*s = WiredSettings->DataDir;
	break;
      }
    case wiredSendHelp :
      {
	if (HelpWin->IsShown())
	  HelpWin->Help->Load_Text((*(string *)value).c_str());
	break;
      }
    case wiredSendKeyEvent :
      {
	RackPanel->HandleKeyEvent(plug, (wxKeyEvent *)value);
	break;
      }
    case wiredSendMouseEvent :
      {
	RackPanel->HandleMouseEvent(plug, (wxMouseEvent *)value);
	break;
      }
    case wiredGetBpm :      
      {
	*(float *)value = Seq->BPM;
	break;
      }
    case wiredGetSigNumerator :
      {
	*(int *)value = Seq->SigNumerator;
	break;
      }
    case wiredGetSigDenominator :
      {
	*(int *)value = Seq->SigDenominator;
	break;
      }
    case wiredGetSamplePos :
      {
	*(long *)value = Seq->CurAudioPos;
	break;
      }
    case wiredGetSamplesPerBar :
      {
	*(double*)value = Seq->SamplesPerMeasure;
	break;
      }
    case wiredGetBarsPerSample :
      {
	*(double*)value = Seq->MeasurePerSample;
	break;
      }
    case wiredGetBarPos :
      {
	*(double *)value = Seq->CurrentPos;
	break;
      }
    case  wiredShowOptionalView :
      {
	OptPanel->ShowPlug(plug);
	break;
      }
    case  wiredCloseOptionalView :
      {
	OptPanel->ClosePlug(plug);
	break;
      }
    case wiredOpenFileLoader :
      {
	struct s_wired_l
	{
	  std::string *t;
	  std::vector<std::string> *e;
	  bool ak;
	  std::string result;
	} *w;
	
	w = (s_wired_l *)value;
	FileLoader *dlg = new FileLoader(0x0, -1, *(w->t), w->ak, false, w->e);
	if (dlg->ShowModal() == wxID_OK)
	  w->result = dlg->GetSelectedFile();
	dlg->Destroy();
	break;
      }
    case wiredSaveFileLoader :
      {
	struct s_wired_l
	{
	  std::string *t;
	  std::vector<std::string> *e;
	  std::string result;
	} *w;
	
	w = (s_wired_l *)value;
	FileLoader *dlg = new FileLoader(0x0, -1, *(w->t), false, true, w->e);
	if (dlg->ShowModal() == wxID_OK)
	  w->result = dlg->GetSelectedFile();
	dlg->Destroy();
	break;
      }

    case  wiredCreateMidiPattern :
      {
	Seq->AddMidiPattern((list<SeqCreateEvent *> *)value, plug);
	break;
      }
    default:
      return (-1);
    }

  return (1);
}
