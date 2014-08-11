#include "wx/wxprec.h"
#include "wx/wx.h"

class MyApp : public wxApp {
 public:
  virtual bool OnInit();
};

class MyFrame : public wxFrame {
 public:
  MyFrame(const wxString&title, const wxPoint& pos, const wxSize& size) {
  }
};
