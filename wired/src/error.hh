#ifndef ERROR_HH_
# define ERROR_HH_

#include <wx/wx.h>

namespace Error
{
  class File
  {
  private:
    wxString name;
    wxString sup;
  public:
    File(wxString s1, wxString s2) : name(s1), sup(s2) { }
    wxString getName() { return name; }
    wxString getSup() { return sup; }
  };

  class NoChannels {};
};

#endif /* ERROR_HH_ */
