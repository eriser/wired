#ifndef  __WIZARD_H__
#define  __WIZARD_H__

#include  <wx/wx.h>
#include  <wx/dialog.h>
#include  <vector>
#include  <wx/filename.h>
#include  <wx/string.h>

#ifdef _WIN32
#include  <wx/listbase.h>
#include  <wx/listctrl.h>
#endif

class	Wizard : public wxDialog
{
  public:
	Wizard();
	~Wizard();
	wxString			GetDir();

	/**
	 * closes the Wizard dialog and updates chosenDir if necessary
	 * \return Nothing
	 */
	void				OnNewClick(wxCommandEvent &event);

	/**
	 * Opens a dir dialog and updates the tc_* as needed
	 * \return Nothing
	 */
	void				OnBrowseClick(wxCommandEvent &event);

	/**
	 * Removes the currently selected Recent project from the list
	 * \return Nothing
	 */
	void				OnRemove(wxCommandEvent &event);

	/**
	 * Launch or create the selected project
	 * \return Nothing
	 */

	void			       OnOkClick(wxCommandEvent &event);
	/**
	 * closes the audio stream
	 * and deletes the settings before exit(0)
	 * \return Nothing
	 */
	void				OnExitClick(wxCommandEvent &event);

	/**
	 * updates the tc_* as needed
	 * \return Nothing
	 */
	void				OnListClick(wxListEvent &event);

	/**
	 * \see LoadProject
	 * \return Nothing
	 */
	void				OnListDClick(wxListEvent &event);

	/**
	 * \see UpdateText
	 * \return Nothing
	 */
	void				OnTextChange(wxCommandEvent &event);

  private:
	/**
	 * Opens a 'directory dialog'
	 * \return wxString the directory selected
	 * else it return wxT("")
	 */
	wxString			ChooseSessionDir();

	/**
	 * closes the wizard and set chosenDir to the Recent project selected
	 * \return Nothing
	 */
	void				LoadProject();

	/**
	 * refresh the list of recent project
	 * \return Nothing
	 */
	void				UpdateList();

	/**
	 * scans the filesystem to see if project path+name points to an existing session
	 * \return Nothing
	 */
	void				UpdateText();

	/**
	 * DEPRECATED creates the directory(s) needed recursivly
	 * \return true on success
	 * false otherwise
	 */
//	bool				MakeMyDirs(wxString dir);

	wxString			chosenDir;
	bool				newSession;
	wxButton			*b_NewProject;
	wxStaticText		*st_ProjectName;
	wxTextCtrl			*tc_ProjectName;
	wxStaticText		*st_NewPath;
	wxTextCtrl			*tc_NewPath;
	wxButton			*b_BrowseNew;
	wxStaticText		*st_RecentTitle;
	wxListCtrl			*lc_Recent;
	wxStaticText		*st_Recent;
	wxButton			*b_Remove;
	wxButton			*b_Exit;
	wxButton			*b_OK;

	DECLARE_EVENT_TABLE()
};

/**
 * The enum table
*/
enum
{
  Wizard_NewBtn = 633,
  Wizard_ProjectName,
  Wizard_Path,
  Wizard_Browse,
  Wizard_RecentList,
  Wizard_Remove,
  Wizard_Exit,
  Wizard_OK
};

#endif  /*__WIZARD_H__*/
