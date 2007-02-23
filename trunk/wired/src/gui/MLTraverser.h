// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991
#ifndef ML_TRAVERSER
#define ML_TRAVERSER

#include <wx/dir.h>

class wxDirTraverserSimple : public wxDirTraverser
{
  public:
    wxDirTraverserSimple(wxArrayString& files) : m_files(files) { }

    virtual wxDirTraverseResult OnFile(const wxString& filename);

    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname));
    //virtual wxOpenErrorTraverseResult OnOpenError(const wxString& openerrorname)

  private:
    wxArrayString& m_files;
};

/*
// get the names of all files in the array
wxArrayString files;
wxDirTraverserSimple traverser(files);

wxDir dir(dirname);
dir.Traverse(traverser);	
*/

#endif
