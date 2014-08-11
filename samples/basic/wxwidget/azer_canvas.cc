#include "samples/basic/wxwidget/azer_canvas.h"
#include "azer/azer.h"
#include "base/logging.h"
#include "base/time/time.h"
#include <tchar.h>

BEGIN_EVENT_TABLE(AzerCanvas, wxWindow)
  EVT_SIZE(AzerCanvas::OnSize)
  EVT_PAINT(AzerCanvas::OnPaint)
  EVT_ERASE_BACKGROUND(AzerCanvas::OnEraseBackground)
  EVT_IDLE(AzerCanvas::OnIdle)
END_EVENT_TABLE()

AzerCanvas::AzerCanvas(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos, const wxSize& size,
                       long style, const wxString& name)
: wxWindow(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name)
    , delegate_(NULL){
}

AzerCanvas::~AzerCanvas() {
}

void AzerCanvas::Init() {
  prev_frame_ = ::base::TimeTicks::HighResNow();
  if (delegate_) {
    delegate_->OnCreate((HWND)GetHWND());
  }
}

void AzerCanvas::OnPaint(wxPaintEvent& WXUNUSED(event)) {
  wxPaintDC dc(this);
}

void AzerCanvas::OnSize(wxSizeEvent& event) {
}

void AzerCanvas::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
  // Do nothing, to avoid flashing on MSW
}

void AzerCanvas::Render() {
  ::base::TimeTicks now = ::base::TimeTicks::HighResNow();
  ::base::TimeDelta delta = now - prev_frame_;
  total_ticking += delta;
  double total = total_ticking.InSecondsF();
  if (delegate_) {
    delegate_->OnRenderScene(total, (float)delta.InSecondsF());
    delegate_->OnUpdateScene(total, (float)delta.InSecondsF());
  }
}

void AzerCanvas::OnIdle(wxIdleEvent &event) {
  event.RequestMore(true);
  Render();
}


enum {
  ID_Quit = 1,
  ID_About,
};
BEGIN_EVENT_TABLE(AzerFrame, wxFrame)
EVT_MENU(ID_Quit, AzerFrame::OnQuit)
EVT_MENU(ID_About, AzerFrame::OnAbout)
END_EVENT_TABLE()

AzerFrame::AzerFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
                     AzerCanvas::Delegate* delegate)
    : wxFrame((wxFrame *)NULL, -1, title, pos, size)
    , canvas_(NULL) {
  wxMenu *menuFile = new wxMenu;

  menuFile->Append(ID_About, _T("&About..."));
  menuFile->AppendSeparator();
  menuFile->Append(ID_Quit, _T("E&xit"));

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, _T("&File"));

  SetMenuBar(menuBar);

  canvas_ = new AzerCanvas(this, wxID_ANY, wxDefaultPosition,
                           wxSize(800, 600), wxSUNKEN_BORDER);
  canvas_->SetDelegate(delegate);
  canvas_->Init();
  CreateStatusBar();
  SetStatusText(_T("Welcome to wxWidgets!"));
}

void AzerFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
  Close(TRUE);
}

void AzerFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
  wxMessageBox(_T("This is a wxWidgets D3D9 Hello world sample"),
               _T("About D3D9 Hello World"), wxOK | wxICON_INFORMATION, this);
}

