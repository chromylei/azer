#pragma once

#include <memory>

#include "wx/wxprec.h"
#include "wx/wx.h"

#include "azer/render/render.h"

class AzerCanvas : public wxWindow {
 public:
  AzerCanvas(wxWindow *parent, wxWindowID id = wxID_ANY,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize, long style = 0,
             const wxString& name = wxT("TestGLCanvas"));

  ~AzerCanvas();

  class Delegate {
   public:
    virtual void OnCreate(HWND hwnd) = 0;
    virtual void OnUpdateScene(double time, float delta_time) = 0;
    virtual void OnRenderScene(double time, float delta_time) = 0;
    virtual void OnQuit() = 0;
  };

  void Init();
  void SetDelegate(Delegate* delegate) {delegate_ = delegate;}
 private:
  void OnWindowCreate(wxWindowCreateEvent& event);
  void OnPaint(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnEraseBackground(wxEraseEvent& event);
  void OnIdle(wxIdleEvent& event);
  void Render();

  base::TimeTicks prev_frame_;
  ::base::TimeDelta total_ticking;
  Delegate* delegate_;
  bool init_;
  DECLARE_EVENT_TABLE()
};


class AzerFrame: public wxFrame {
 public:
  AzerFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
            AzerCanvas::Delegate* delegate);

  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  AzerCanvas *GetCanvas() { return canvas_; }

  DECLARE_EVENT_TABLE()
  private:
  AzerCanvas* canvas_;
};
