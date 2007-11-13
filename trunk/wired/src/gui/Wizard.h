#ifndef  __WIZARD_H__
#define  __WIZARD_H__

#include  <wx/wx.h>
#include  <wx/dialog.h>
#include  <vector>
#include  <wx/filename.h>
#include  <wx/string.h>

class	Wizard : public wxDialog
{
  public:
	Wizard();
	~Wizard();
	wxString			GetDir();

	void				OnNewClick(wxCommandEvent &event);
	void				OnBrowseClick(wxCommandEvent &event);
	void				OnOpenClick(wxCommandEvent &event);
	void				OnRemove(wxCommandEvent &event);
	void				OnExitClick(wxCommandEvent &event);
	void				OnListClick(wxListEvent &event);
	void				OnListDClick(wxListEvent &event);
	void				OnTextChange(wxCommandEvent &event);

  private:
	wxString			ChooseSessionDir();
	void				LoadProject();
	void				UpdateList();
	void				UpdateText();

	wxString			chosenDir;
	bool				newSession;
	wxButton			*b_NewProject;
	wxTextCtrl			*tc_NewPath;
	wxButton			*b_BrowseNew;
	wxStaticText		*st_RecentTitle;
	wxListCtrl			*lc_Recent;
	wxStaticText		*st_Recent;
	wxButton			*b_Remove;
	wxButton			*b_Exit;

	DECLARE_EVENT_TABLE()
};

/**
 * The enum table
*/
enum
{
  Wizard_NewBtn = 633,
  Wizard_Enter,
  Wizard_Browse,
  Wizard_RecentList,
  Wizard_Remove,
  Wizard_Exit
};

#endif  /*__WIZARD_H__*/
