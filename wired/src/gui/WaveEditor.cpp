// Copyright (C) 2004 by Wired Team
// Under the GNU General Public License

#include "WaveEditor.h"
#include "Colour.h"
#include "Settings.h"
#include "ClipBoard.h"

#include <math.h>
#include <iostream>

using namespace std;

WaveEditor::WaveEditor(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, bool fulldraw) 
  : WaveView (parent, id, pos, size, fulldraw)
{
  cout << "[WaveEditor] - Constructor" << endl;
}


WaveEditor::~WaveEditor()
{
}


void WaveEditor::OnPaint(wxPaintEvent &event)
{
  // cout << "[WaveEditor] - OnPaint" << endl;
  /* Wave = 0x0 */
  //if ((Data || !Wave->LoadedInMem) && Bmp)
  if ((Data || (Wave && !Wave->LoadedInMem)) && Bmp)
    {
      // Get the panel width and heigth 
      int width, height;
      GetSize(&width, &height);
      mSelectedRegion.height = height;
      // Create the selection background
      wxBrush selectedBrush;
      selectedBrush.SetColour(148, 148, 170);
      // Create the selecting pen
      wxPen selectedPen;
      selectedPen.SetColour(148, 148, 170);
      // Set back the initial background and border colour
      // and paint the wave background
      memDC.SetPen(GetPenColor());
      memDC.SetBrush(CL_SEQ_BACKGROUND);
      memDC.DrawRectangle(0, 0, width, height);
      // Draw the selection region
      memDC.SetBrush(selectedBrush);
      memDC.SetPen(selectedPen);
      if (mSelectedRegion.width != 0)
	{
	  // cout << "\nDS - "<< mSelectedRegion.x << "; " << mSelectedRegion.y
	  // 	  << "; " << mSelectedRegion.width << "; " << mSelectedRegion.height << endl;
	  memDC.DrawRectangle(mSelectedRegion);  
	}
      else
	{
	  memDC.DrawLine(mSelectedRegion.x, 0, mSelectedRegion.x, height);
	}
      // Repaint the wave form
      memDC.SetPen(CL_WAVE_DRAW);
      long coeff = height / 2;
      for (int i = 0; i < width; i++)
	memDC.DrawLine(i, coeff - DrawData[i], i, coeff + DrawData[i]);
    }
  // Appel de la methode paint de la classe mere
  WaveView::OnPaint(event); 
}


void WaveEditor::OnSize(wxSizeEvent &event)
{
#ifdef __DEBUG__
  cout << "[WaveEditor] - OnSize" << endl;
#endif
  WaveView::OnSize(event);
}


void WaveEditor::OnMouseEvent(wxMouseEvent& event)
{
  // cout << "\n[WaveEditor] - WaveEditor::OnMouseEvent" << endl;
  // Get the panel width and heigth
  int width, height;
  GetSize(&width, &height);
  long inc = (EndWavePos / width);

  try 
  {
    // Copie 
    if (event.ShiftDown() && event.ButtonDown(1))
      {
	// cout << "\n[WaveEditor] - Coefficient " << inc << endl;
	// cout << "\n[WaveEditor] - Copy from " << (mSelectedRegion.x * inc) << ", "  << (mSelectedRegion.width * inc) << " frames" << endl;  
	cClipBoard::Global().Copy(*Wave, (mSelectedRegion.x*inc), (mSelectedRegion.width*inc));
	mSelectedRegion.width = 0;
	Refresh();
	return;
      }
    // Couper
    if (event.ShiftDown() && event.ButtonDown(2))
      {
#ifdef __DEBUG__
	cout << "\n[WaveEditor] - Coefficient " << inc << endl;
	cout << "\n[WaveEditor] - Cut from " << (mSelectedRegion.x * inc) << ", "  << (mSelectedRegion.width * inc) << " frames" << endl;  
#endif
	cClipBoard::Global().Cut(*Wave, mSelectedRegion.x*inc, mSelectedRegion.width*inc);
	mSelectedRegion.width = 0;
	WaveDrawer::SetDrawing(GetSize());
	Refresh();
	return;
	}
    // Coller
    if (event.ShiftDown() && event.ButtonDown(3))
      {
	//cout << "\n[WaveEditor] - Button down 3" << endl;
	cClipBoard::Global().Paste(*Wave, event.m_x*inc);
	WaveDrawer::SetDrawing(GetSize());
	Refresh();
	return;
      }
  }
  catch (cException & e)
    {
      cout << "\n[WaveEditor] - Error occured : " << e.What() << endl;
    }
  mSelectedRegion.height = height;
  if (event.ButtonDown(1) && (mIsSelecting == false))
    {
      // cout << "\n[WaveEditor] - Click on frame " << (inc * event.m_x) << endl;
      // cout << "\n[WaveEditor] - event.ButtonDown(1) && (mIsSelecting == false" << endl;
      mIsSelecting = true;
      mSelectedRegion.x = event.m_x;
      mSelectedRegion.width = 0;
    }
  else
    if (mIsSelecting == true)
      {
	// cout << "\n[WaveEditor] - event.ButtonDown(1) && (mIsSelecting == true" << endl;
	mSelectedRegion.width =  event.m_x - mSelectedRegion.x;
      }
  
  if (event.ButtonUp(1))
    {
      // cout << "\n[WaveEditor] - event.ButtonUp(1)" << endl;
      mIsSelecting = false;
    }
  // cout << "\nDS - "<< mSelectedRegion.x << "; " << mSelectedRegion.y << "; " << mSelectedRegion.width << "; " << mSelectedRegion.height << endl;
  Refresh ();
}


BEGIN_EVENT_TABLE(WaveEditor, wxWindow)
  EVT_MOUSE_EVENTS(WaveEditor::OnMouseEvent)
  EVT_PAINT(WaveEditor::OnPaint)
  EVT_SIZE(WaveEditor::OnSize)
END_EVENT_TABLE()

