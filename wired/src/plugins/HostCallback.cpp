// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include <list>
#include <wx/wx.h>
#include "Rack.h"
#include "Sequencer.h"
#include "OptionPanel.h"
#include "HelpPanel.h"
#include "Settings.h"
#include "MainWindow.h"
#include "MidiThread.h"
#include "MidiController.h"
#include "Plugin.h"
#include "HelpCtrl.h"

extern Rack *RackPanel;

using namespace std;

wxString	MakeExtFromVector(vector<wxString> *e)
{
  wxString extensions_chosen = wxT("");
  wxString desc;
  wxString ext;
  wxString extFilters;

  for (unsigned int i = 0; i < e->size(); i++)
  {
    ext = (*e)[i];
    extFilters = ext.BeforeFirst('\t');
    extFilters = wxT("*.") + extFilters;
    extFilters.Replace(wxT(";"), wxT(";*."));
    extensions_chosen += ext.AfterLast('\t') + wxT("|") +
      extFilters + wxT("|");
  }
  extensions_chosen = extensions_chosen.Mid(0, extensions_chosen.Len() - 1);
  return (extensions_chosen);
}

long HostCallback(Plugin *plug, long param, void *value)
{
  static wxString	lastSaveDir;
  static wxString	lastOpenDir;
  wxString			extensions_chosen;
  bool				wtodel = false;
  wxFileDialog		*dlg = 0x0;

    switch (param)
    {
        case wiredSendMouseEvent :
            RackPanel->HandleMouseEvent(plug, (wxMouseEvent *)value);
            break;
        case wiredSendPaintEvent :
            RackPanel->HandlePaintEvent(plug, (wxPaintEvent *)value);
            break;
        case wiredAskUpdateGui :
            MainWin->AddUpdatePlugin(plug);
            break;
		case wiredHostProductName :
			if (value)
			  ((wxString *)value)->Append(WIRED_NAME);
            break;
        case wiredHostProductVersion :
			if (value)
			  ((wxString *)value)->Append(WIRED_VERSION);
            break;
        case wiredHostVendorName :
            value = (void *)_("P31");
            break;
        case wiredGetDataDir :
        {
            wxString *s = (wxString *)value;
            *s = WiredSettings->DataDir;
            break;
                    }
        case wiredSendHelp :
        {
            if (HelpWin == NULL)
                break;
            if (HelpWin->IsShown())
                HelpWin->Help->Load_Text((*(wxString *)value));
            break;
        }
        case wiredSendKeyEvent :
            RackPanel->HandleKeyEvent(plug, (wxKeyEvent *)value);
            break;
        case wiredGetBpm :
            *(float *)value = Seq->BPM;
            break;
        case wiredGetSigNumerator :
            *(int *)value = Seq->SigNumerator;
            break;
        case wiredGetSigDenominator :
            *(int *)value = Seq->SigDenominator;
            break;
        case wiredGetSamplePos :
            *(long *)value = Seq->CurAudioPos;
            break;
        case wiredGetSamplesPerBar :
            *(double*)value = Seq->SamplesPerMeasure;
            break;
        case wiredGetBarsPerSample :
            *(double*)value = Seq->MeasurePerSample;
            break;
        case wiredGetBarPos :
            *(double *)value = Seq->CurrentPos;
            break;
        case wiredShowOptionalView :
            OptPanel->ShowPlug(plug);
            break;
        case wiredCloseOptionalView :
            OptPanel->ClosePlug(plug);
            break;
        case wiredOpenFileLoader :
        {
            struct s_wired_l
            {
                wxString t;
                vector<wxString> *e;
                bool ak;
                wxString result;
            } *w;
            w = (s_wired_l *)value;
			bool wtodel = false;
			if (!w)
			{
			  wtodel = true;
			  w = new s_wired_l;
			  w->t = wxT("");
			  w->e = NULL;
			}
			extensions_chosen = wxT("");
			if (!w->e)
			  extensions_chosen.Append(WIRED_SUPPORTED_SNDFILES);
			else
			  extensions_chosen += MakeExtFromVector(w->e);
			//just in case...
			//extensions_chosen.Append(wxT("|"));
			//extensions_chosen.Append(WIRED_SUPPORTED_CONFFILE);
			extensions_chosen.Append(wxT("|All Files (*.*)|*.*"));
#ifdef __DEBUG__
			cout << "HostCallback() : extensions_chosen == " << extensions_chosen.mb_str() << endl;
#endif


			if (lastOpenDir && wxFileName::DirExists(lastOpenDir))
			  dlg = new wxFileDialog(MainWin, w->t, lastOpenDir, wxT(""), extensions_chosen);
			else
			  dlg = new wxFileDialog(MainWin, w->t, wxT(""), wxT(""), extensions_chosen);
            if (dlg->ShowModal() == wxID_OK)
			{
				w->result = dlg->GetPath();
				lastOpenDir = dlg->GetDirectory();
			}
			if (wtodel)
			  delete w;
            break;
        }
        case wiredSaveFileLoader :
        {
            struct s_wired_l
            {
                wxString t;
                std::vector<wxString> *e;
                wxString result;
            } *w;
            w = (s_wired_l *)value;
			if (!w)
			{
			  wtodel = true;
			  w = new s_wired_l;
			  w->t = wxT("");
			  w->e = NULL;
			}
			extensions_chosen = wxT("");
			if (!w->e)
			  extensions_chosen.Append(WIRED_SUPPORTED_SNDFILES);
			else
			  extensions_chosen += MakeExtFromVector(w->e);
			//just in case...
			//extensions_chosen.Append(wxT("|"));
			//extensions_chosen.Append(WIRED_SUPPORTED_CONFFILE);
			extensions_chosen.Append(wxT("|All Files (*.*)|*.*"));
#ifdef __DEBUG__
			cout << "HostCallback() : extensions_chosen == " << extensions_chosen.mb_str() << endl;
#endif
			if (lastSaveDir && wxFileName::DirExists(lastSaveDir))
			  dlg = new wxFileDialog(MainWin, w->t, lastSaveDir, wxT(""), extensions_chosen, wxSAVE);
			else
			  dlg = new wxFileDialog(MainWin, w->t, wxT(""), wxT(""), extensions_chosen, wxSAVE);
            if (dlg->ShowModal() == wxID_OK)
			{
			  w->result = dlg->GetPath();
			  lastSaveDir = dlg->GetDirectory();
			}
            /*
            FileLoader *dlg = new FileLoader(MainWin, MainWin_FileLoader, w->t, false, true, w->e);
            if (dlg->ShowModal() == wxID_OK)
                w->result = dlg->GetSelectedFile();
            dlg->Destroy();
            */
            break;
        }
        case wiredCreateMidiPattern :
            Seq->AddMidiPattern((list<SeqCreateEvent *> *)value, plug);
            break;
        case  wiredShowMidiController :
        {
            MidiMutex.Lock();
            Controller = new MidiController(plug);
            MidiMutex.Unlock();
            Controller->ShowModal();
            int *midi_msg = *(int **)value;
            midi_msg[0] = Controller->Type;
            midi_msg[1] = Controller->Controller;
            midi_msg[2] = Controller->Value;
            MidiMutex.Lock();
            delete Controller;
            Controller = 0x0;
            MidiMutex.Unlock();
            break;
        }
        default:
            return (-1);
    }
	delete dlg;
    return (1);
}
