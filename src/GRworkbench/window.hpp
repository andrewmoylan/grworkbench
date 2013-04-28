#pragma once

#include <memory>
#include <memory>
#include <lift/array.hpp>
#include "pane.hpp"

#ifdef _WIN32
#  include <windows.h>
#else
#  include <X11/keysym.h>
#  include <X11/Xlib.h>
#  include <X11/cursorfont.h>
#  include <GL/glx.h>
#  include <sys/time.h>
#endif

namespace grwb
{

  using std::string;
  using std::shared_ptr;

  class window
  {

  public:

    window(const std::shared_ptr<pane>&, const string&);
    ~window();

    void draw();
    void hide();
    void show();

		bool visible;

  private:
		
    void set_title(const string&);
    vector<size_t, 2> size() const;
    void swap_buffers();

#ifdef _WIN32
    static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT nsMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND hMainWindow;
    HDC glhDC;
    HGLRC glhRC;
#else
public:
    static Display					*dpy		;
    static void MainWndProc(XEvent&);
private:
    Window					glwin	;
    Cursor					curx		;
    Cursor					curp		;
    XVisualInfo			   *vi		;
    GLXContext				cx		;
    XSetWindowAttributes	swa		;
    Colormap				cmap	;
    static std::map<Window, window*> conversion;
    void nsMainWndProc(XEvent*);
    void key_up(const int);
    void key_down(const int);
#endif

    std::shared_ptr<pane> pane_;

	public:
		bool active;

  };
  
  #ifndef _WIN32
  int timeGetTime();
  #endif

}
