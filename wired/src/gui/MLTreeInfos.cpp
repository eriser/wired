// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include <wx/wx.h>
#include "MLTreeInfos.h"


MLTreeInfos::MLTreeInfos(wxWindow *MediaLibraryPanel, wxPoint p, wxSize s, long style, s_nodeInfo infos)
  : wxWindow(MediaLibraryPanel->GetParent(), -1, p, s, style)
{
  wxStaticText			*Text;
  wxString			textContent;

  this->SetBackgroundColour(wxColour(206, 200, 200));
  new wxString(textContent);
  textContent.Append(infos.label);
  textContent.Append(_("\nExtention : "));
  textContent.Append(infos.extention);
  textContent.Append(_("\nLength : "));
  textContent.Append(infos.length);
  textContent.Append(_("\nBitrate"));
  textContent.Append(_("\nSize : "));
  textContent.Append(_("\nCodec : "));
  Text = new MywxStaticText((wxWindow*)this, (wxWindowID)-1, (const wxString&)textContent, wxPoint(2, 2),
		   wxSize(200, 100), wxALIGN_LEFT| wxST_NO_AUTORESIZE, _(""));
  Text->SetFont(wxFont(7, wxSWISS , wxNORMAL, wxNORMAL, false, _("Arial")));
  Show(true);
}

MLTreeInfos::~MLTreeInfos()
{
  
}


MywxStaticText::MywxStaticText(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = _("staticText")) 
  : wxStaticText(parent, id, label, pos, size, style, name)
{
  this->infoParent = parent;
}

void		MywxStaticText::InfoDestroy(wxMouseEvent& event)
{

  this->infoParent->Destroy();
}

BEGIN_EVENT_TABLE(MywxStaticText, wxStaticText)
  EVT_LEFT_DOWN(MywxStaticText::InfoDestroy)
END_EVENT_TABLE();
