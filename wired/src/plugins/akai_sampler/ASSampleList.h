#ifndef _ASSAMPLELIST_H_
#define _ASSAMPLELIST_H_

#include <wx/wx.h>
#include "ASPlugin.h"
#include "ASPlugPanel.h"
#include "ASList.h"
#include "Plugin.h"
#include "ASKeygroupList.h"

using namespace std;

#define IMAGE_BT_ADD_SAMPLE       "plugins/akaisampler/add.png"
#define IMAGE_BT_DEL_SAMPLE       "plugins/akaisampler/del.png"
#define IMAGE_BT_ASSIGN_SAMPLE    "plugins/akaisampler/assign.png"
#define IMAGE_BT_EFFECT_SAMPLE    "plugins/akaisampler/effect.png"

class ASSampleList : public ASPlugin
{
  public:
    ASSampleList(wxString Name);
    ~ASSampleList();
    wxWindow *CreateView(wxPanel *, wxPoint &, wxSize &);
    void OnAddSample(wxCommandEvent &);
    void OnDelSample(wxCommandEvent &);
    void OnAssignSample(wxCommandEvent &);
    void OnEffectSample(wxCommandEvent &);
    void OnResize(wxSizeEvent &);
    void OnSelectEffect(wxCommandEvent &);
    ASList *List;
    ASPlugPanel *pp;
    void SetPlugPanel(ASPlugPanel *pp) { this->pp = pp; }

  DECLARE_EVENT_TABLE()
};

extern ASSampleList *Samples;

enum 
{
  ASSampleList_AddSample = 1234,
  ASSampleList_DelSample,
  ASSampleList_AssignSample,
  ASSampleList_EffectSample
};

#endif
