#ifndef __ASLIST_H__
#define __ASLIST_H__ 

#include <wx/wx.h>
#include <iostream>
#include <vector>

using namespace std;

#define ITEMHEIGHT 15
#define TITLEHEIGHT 15

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
    void Rename(const wxString name) { this->name = name; }
    void SetSelected(bool selected) { this->selected = selected; }
    void InvertSelection() { selected = !selected; }
    bool IsSelected() { return selected; }
    wxString GetName() { return name; }
    void *GetEntry() { return entry; }
    // ...etc
  DECLARE_EVENT_TABLE()
};

class ASList: public wxPanel
{
  public:
  ASList(wxWindow *parent, wxWindowID id, const wxPoint& pos,
      const wxSize& size, wxString title);
  ~ASList();
  void OnPaint(wxPaintEvent &e);
  void AddEntry(wxString name, void *entry);
  void DelEntry(void *entry);
  private:
  void Repos();
  vector<ASListEntry *> entries;
  wxPanel *list;
  wxScrolledWindow *sw;
  wxString title;

  DECLARE_EVENT_TABLE()
};

#endif
