// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __ASLIST_H__
#define __ASLIST_H__ 

#include <wx/wx.h>
#include <iostream>
#include <vector>

using namespace std;

#define ITEMHEIGHT 15
#define BUTTONSHEIGHT 16

class ASListEntry : public wxPanel
{
  private:
    wxString name;
    bool selected;
    void *entry;
  public:
    ASListEntry(wxWindow *parent, int id, wxPoint pos, wxSize size, wxString &name, void *entry);
    ~ASListEntry();
    void OnPaint(wxPaintEvent &e);
    void OnClick(wxMouseEvent &e);
    void OnRename(wxMouseEvent &e);
    void Rename(const wxString name) { this->name = name; }
    void SetSelected(bool selected) { this->selected = selected; Refresh(); }
    void InvertSelection() { selected = !selected; Refresh();}
    bool IsSelected() { return selected; }
    wxString GetName() { return name; }
    void *GetEntry() { return entry; }
  DECLARE_EVENT_TABLE()
};

class ASTextCtrl : public wxTextCtrl
{
  private:
    ASListEntry *ale;
  public:
    ASTextCtrl(wxWindow *, int, wxString, wxPoint, wxSize, ASListEntry *);
    void KillControl(wxFocusEvent &);
    void KillControl2(wxCommandEvent &);
  DECLARE_EVENT_TABLE()
};

class ASList: public wxPanel
{
  public:
  ASList(wxWindow *parent, wxWindowID id, const wxPoint& pos,
      const wxSize& size);
  ~ASList();
  void OnScroll(wxScrollEvent &e);
  void OnResize(wxSizeEvent &e);
  ASListEntry *AddEntry(wxString name, void *entry);
  void DelEntry(void *entry);
  void AddControl(wxWindow *);  
  ASListEntry * GetSelected();
  void SetSelected(ASListEntry *sel);
  int size() { return entries.size(); }
  vector<ASListEntry *> GetEntries() { return entries; }
  private:
  void Repos();
  vector<ASListEntry *> entries;
  wxPanel *list;
  wxPanel *buttons;
  wxScrolledWindow *sw;
  wxPoint bpos;
  wxScrollBar *sb;

  DECLARE_EVENT_TABLE()
};

#endif
