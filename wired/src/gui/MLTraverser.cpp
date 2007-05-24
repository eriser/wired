// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991
#include "MLTraverser.h"
#include <wx/dir.h>
#include <wx/arrstr.h>

wxDirTraverseResult wxDirTraverserSimple::OnFile(const wxString& filename)
{
  m_files.Add(filename);
  return (wxDIR_CONTINUE);
}

wxDirTraverseResult wxDirTraverserSimple::OnDir(const wxString& WXUNUSED(dirname))
{
  return (wxDIR_CONTINUE);
}

//wxOpenErrorTraverseResult OnOpenError(const wxString& openerrorname)
