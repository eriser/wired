#ifndef _ASSAMPLELIST_H_
#define _ASSAMPLELIST_H_

#include <wx/wx.h>
#include "ASPlugin.h"
#include "ASPlugPanel.h"
#include "ASList.h"
#include "Plugin.h"
#include "ASKeygroupList.h"

using namespace std;

#define IMAGE_BT_ADD_UP           "plugins/akaisampler/add_up.png"
#define IMAGE_BT_DEL_UP           "plugins/akaisampler/del_up.png"
#define IMAGE_BT_ADD_DOWN      "plugins/akaisampler/add_down.png"
#define IMAGE_BT_DEL_DOWN      "plugins/akaisampler/del_down.png"

class ASSampleList : public ASPlugin
{
  public:
    ASSampleList(wxString Name);
    ~ASSampleList();
    wxWindow *CreateView(wxPanel *, wxPoint &, wxSize &);
    void OnAddSample(wxCommandEvent &);
    void OnDelSample(wxCommandEvent &);
//    void OnAssignSample(wxCommandEvent &);
 //   void OnEffectSample(wxCommandEvent &);
    void OnResize(wxSizeEvent &);
 //   void OnSelectEffect(wxCommandEvent &);
    ASList *List;
  //  ASPlugPanel *pp;
 //   void SetPlugPanel(ASPlugPanel *pp) { this->pp = pp; }

  DECLARE_EVENT_TABLE()
};

extern ASSampleList *Samples;

enum 
{
  ASSampleList_AddSample = 1234,
  ASSampleList_DelSample,
  /*
  ASSampleList_AssignSample,
  ASSampleList_EffectSample
  */
};

#endif
