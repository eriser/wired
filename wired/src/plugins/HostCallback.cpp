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

long HostCallback(Plugin *plug, long param, void *value)
{
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
            value = (void *)WIRED_NAME;
            break;
        case wiredHostProductVersion :
            value = (void *)WIRED_VERSION;
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
                std::vector<wxString> *e;
                bool ak;
                wxString result;
            } *w;
            w = (s_wired_l *)value;
            wxFileDialog dlg(MainWin, w->t);
            if (dlg.ShowModal() == wxID_OK)
                w->result = dlg.GetFilename();
            /*
            FileLoader *dlg = new FileLoader(MainWin, MainWin_FileLoader, w->t, w->ak, false, w->e);
            if (dlg->ShowModal() == wxID_OK)
                w->result = dlg->GetSelectedFile();
            dlg->Destroy();
            */
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
            wxFileDialog dlg(MainWin, w->t, "", "", "", wxSAVE);
            if (dlg.ShowModal() == wxID_OK)
                w->result = dlg.GetFilename();
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
    return (1);
}
