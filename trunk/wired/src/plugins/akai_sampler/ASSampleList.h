#ifndef _ASSAMPLELIST_H_
#define _ASSAMPLELIST_H_

#include <wx/wx.h>
#include "ASPlugin.h"
#include "ASPlugPanel.h"
#include "ASList.h"
#include "Plugin.h"
#include "ASKeygroupList.h"

using namespace std;

#define IMAGE_BT_ADD_UP           wxT("plugins/akaisampler/add_up.png")
#define IMAGE_BT_DEL_UP          wxT("plugins/akaisampler/del_up.png")
#define IMAGE_BT_ADD_DOWN      wxT("plugins/akaisampler/add_down.png")
#define IMAGE_BT_DEL_DOWN      wxT("plugins/akaisampler/del_down.png")

class ASSampleList : public ASPlugin
{
  public:
    ASSampleList(class AkaiSampler *as, wxString Name);
    ~ASSampleList();
    wxWindow *CreateView(wxPanel *, wxPoint &, wxSize &);
    void OnAddSample(wxCommandEvent &);
    void OnDelSample(wxCommandEvent &);
    void OnResize(wxSizeEvent &);
    ASList *List;

  DECLARE_EVENT_TABLE()
};

enum 
{
  ASSampleList_AddSample = 1234,
  ASSampleList_DelSample,
};

#endif
