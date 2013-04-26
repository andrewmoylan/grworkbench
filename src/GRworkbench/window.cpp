#include <map>
#include <boost/optional.hpp>
#include "application.hpp"
#include "window.hpp"

#include <GL/gl.h>

namespace grwb
{

  using namespace boost;
  using std::cerr;
  using std::endl;
  using lift::make_vector;

#ifndef _WIN32
  int timeGetTime()
  {
    timeval t;
    gettimeofday(&t, 0);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
  };
  
  Display* window::dpy;
  std::map<Window, window*> window::conversion;
#endif

  class animate
  {
  public:
    explicit animate(window* const p)
        : _(p)
    {}

    void operator()() const
    {
      _->draw();
			if (_->visible && _->active)
				application::push_timed(timeGetTime() + 30, *this); // maximum of 33 fps normally
			else
				application::push_timed(timeGetTime() + 1000, *this); // maximum of 1 fps when not the current window focus
    }

  private:
    window* const _;
    animate& operator=(const animate&);
  };

#ifdef _WIN32
  window::window(const shared_ptr<pane>& p, const string& s)
      : pane_(p),
			visible(true)
  {
    static bool window_class_registered(false);

    if (!window_class_registered)
    {
      WNDCLASSEX cMainWindow;
      cMainWindow.cbSize			= sizeof(WNDCLASSEX);
      cMainWindow.style			= CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE; // | CS_DBLCLKS;
      cMainWindow.lpfnWndProc		= (WNDPROC) MainWndProc;
      cMainWindow.cbClsExtra		= 0;
      cMainWindow.cbWndExtra		= sizeof(this);
      cMainWindow.hInstance		= 0;
      cMainWindow.hIcon			= (struct HICON__ *) LoadImage(NULL, "icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
      cMainWindow.hCursor			= LoadCursor(NULL, IDC_CROSS);
      cMainWindow.hbrBackground	= 0;
      cMainWindow.lpszMenuName	= 0;
      cMainWindow.lpszClassName	= "grwb";
      cMainWindow.hIconSm			= 0;
      RegisterClassEx(&cMainWindow);
      window_class_registered = true;
    }

    hMainWindow = CreateWindowEx(
                    0,
                    "grwb",
                    s.c_str(),
                    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, //WS_POPUP
                    (GetSystemMetrics(SM_CXSCREEN) - 640) / 2,//0
                    (GetSystemMetrics(SM_CYSCREEN) - 480) / 2,//0
                    640,//800
                    480,//600
                    NULL,
                    NULL,
                    0,
                    this
                  );
    ShowWindow(hMainWindow, SW_SHOW);
    UpdateWindow(hMainWindow);
    application::push(animate(this));

  }
#else
using namespace std;
  window::window(const shared_ptr<pane>& p, const string& s)
    : pane_(p),
      visible(true)
  {
    int Dummy;

    if(!glXQueryExtension(dpy, &Dummy, &Dummy))
    {
      cerr << "No GLX extension" << endl;
      exit(EXIT_FAILURE);
    }

    curx = XCreateFontCursor(dpy, XC_crosshair);
    curp = XCreateFontCursor(dpy, XC_left_ptr);

    swa.border_pixel = 10;
    swa.event_mask =
      Button1MotionMask	|	// It is possible that "Motion" should not be in these identifiers ...
      Button2MotionMask	|
      Button3MotionMask	|
      Button4MotionMask	|
      Button5MotionMask	|	// ... if there is a problem, remove it from them
      ButtonMotionMask	|
      ButtonPressMask		|
      ButtonReleaseMask	|
      ExposureMask		|
      KeyPressMask		|
      KeyReleaseMask		|
      OwnerGrabButtonMask	|
      PointerMotionMask	|
      StructureNotifyMask	|
      FocusChangeMask;

    int attributes[] = { GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, GLX_RGBA, None };

    vi = glXChooseVisual(dpy, DefaultScreen(dpy), attributes);
    cx = glXCreateContext(dpy, vi, None, GL_TRUE);
    cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
    swa.colormap = cmap;
    glwin = XCreateWindow(
              dpy,
              RootWindow(dpy, vi->screen),
              (DisplayWidth(dpy, DefaultScreen(dpy)) - 640) / 2,
              (DisplayHeight(dpy, DefaultScreen(dpy)) - 480) / 2,
              640,
              480,
              0,
              vi->depth,
              InputOutput,
              vi->visual,
              CWBorderPixel | CWColormap | CWEventMask,
              &swa
            );
    //XSetStandardProperties(dpy, glwin, s.c_str(), s.c_str(), None, NULL, 0, NULL);
    XDefineCursor(dpy, glwin, curx);
    glXMakeCurrent(dpy, glwin, cx);
    XMapWindow(dpy, glwin);
    
    {
      XTextProperty name;
      char* cs = new char[s.length() + 1];
      copy(s.begin(), s.end(), cs);
      cs[s.length()] = 0;
      XmbTextListToTextProperty(dpy, &cs, 1, XStringStyle, &name);
      XSetWMProperties(dpy, glwin, &name, &name, 0, 0, 0, 0, 0);
      delete[] cs;
    }
    
    conversion.insert(std::make_pair(glwin, this));
    application::push(animate(this));    
  }
#endif

  window::~window()
  {
#ifdef _WIN32
    DestroyWindow(hMainWindow);
#endif

  }

  void window::draw()
  {
#ifdef _WIN32
    wglMakeCurrent(glhDC, glhRC);
#else
    glXMakeCurrent(dpy, glwin, cx);
#endif
    glDrawBuffer(GL_BACK);
    glColorMask(true, true, true, true);
    glDepthMask(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, size()[0], size()[1]);

    if (active)
      pane_->process(); // should be moved elsewhere

    pane_->draw(size());
    swap_buffers();
  }

  void window::swap_buffers()
  {
#ifdef _WIN32
    SwapBuffers(glhDC);
#else
    glXSwapBuffers(dpy, glwin);
#endif

  }

  void window::set_title(const string& s)
  {
#ifdef _WIN32
    SetWindowText(hMainWindow, s.c_str());
#else
    XSetStandardProperties(dpy, glwin, s.c_str(), s.c_str(), None, 0, 0, 0);
#endif

  }

  void window::hide()
  {
#ifdef _WIN32
    ShowWindow(hMainWindow, SW_HIDE);
#else
    XUnmapWindow(dpy, glwin);
#endif
		visible = false;
  }

  void window::show()
  {
#ifdef _WIN32
    ShowWindow(hMainWindow, SW_SHOW);
#else
    XMapWindow(dpy, glwin);
#endif
		visible = true;
  }

  vector<size_t, 2> window::size() const
  {
#ifdef _WIN32
    RECT s;
    GetClientRect(hMainWindow, &s);
    return make_vector<size_t>(static_cast<size_t>(s.right), static_cast<size_t>(s.bottom));
#else
    XWindowAttributes xwa;
    XGetWindowAttributes(dpy, glwin, &xwa);
    return make_vector<size_t>(xwa.width, xwa.height);
#endif

  }

#ifdef _WIN32
  LRESULT CALLBACK window::MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {

    if (uMsg == WM_CREATE)
#pragma warning (disable: 4311)
      SetWindowLong(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams));
#pragma warning (default: 4311)
#pragma warning (disable: 4312)
    if (window* p = reinterpret_cast<window*>(GetWindowLong(hWnd, GWLP_USERDATA)))
#pragma warning (default: 4312)
      return p->nsMainWndProc(hWnd, uMsg, wParam, lParam);
    else
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
#else
  void window::MainWndProc(XEvent& e)
  {
    if (conversion[((XAnyEvent*)&e)->window])
      conversion[((XAnyEvent *)&e)->window]->nsMainWndProc(&e);
    else
      cerr << "XEvent ignored (window not found)" << endl;
  }
#endif

#ifdef _WIN32
  LRESULT window::nsMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    int i;

    int c;

    switch(uMsg)
    {

      // Window management
    case WM_ACTIVATE:
      active = !(wParam == WA_INACTIVE);
      break;

    case WM_CREATE:
      glhDC = GetDC(hWnd);
      PIXELFORMATDESCRIPTOR pfd;
      pfd.nSize = sizeof (PIXELFORMATDESCRIPTOR);
      pfd.nVersion = 1;
      pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
      pfd.iPixelType = PFD_TYPE_RGBA;
      pfd.cColorBits = 24;
      pfd.cRedBits = 8;
      pfd.cRedShift = 0;
      pfd.cGreenBits = 8;
      pfd.cGreenShift = 0;
      pfd.cBlueBits = 8;
      pfd.cBlueShift = 0;
      pfd.cAlphaBits = 0;
      pfd.cAlphaShift = 0;
      pfd.cAccumBits = 0;
      pfd.cAccumRedBits = 0;
      pfd.cAccumGreenBits = 0;
      pfd.cAccumBlueBits = 0;
      pfd.cAccumAlphaBits = 0;
      pfd.cDepthBits = 32;
      pfd.cStencilBits = 0;
      pfd.cAuxBuffers = 0;
      pfd.iLayerType = PFD_MAIN_PLANE;
      pfd.bReserved = 0;
      pfd.dwLayerMask = 0;
      pfd.dwVisibleMask = 0;
      pfd.dwDamageMask = 0;
      i = ChoosePixelFormat(glhDC, &pfd);
      SetPixelFormat(glhDC, i, &pfd);
      glhRC = wglCreateContext(glhDC);
      wglMakeCurrent(glhDC, glhRC);
      //SetCursor(hCursorCrosshair);
      break;
    case WM_CLOSE:
      DestroyWindow(hWnd);
      break;
    case WM_DESTROY:
      if(glhRC)
      {
        wglDeleteContext(glhRC);
      }
      if(glhDC)
      {
        ReleaseDC(hWnd, glhDC);
      }
      //PostQuitMessage(0);
      break;
    case WM_PAINT:
      pane_->draw(size());
      ValidateRect(hWnd, NULL);
      break;
    case WM_MOUSEMOVE:
      pane_->mouse_move(make_vector<int>(LOWORD(lParam), HIWORD(lParam)));
      break;
    case WM_LBUTTONDOWN:
      pane_->mouse_down(make_vector<int>(LOWORD(lParam), HIWORD(lParam)));
      SetCapture(hMainWindow);
      break;
    case WM_LBUTTONUP:
      pane_->mouse_up(make_vector<int>(LOWORD(lParam), HIWORD(lParam)));
      ReleaseCapture();
      break;
    case WM_RBUTTONDOWN:
      SetCapture(hMainWindow);
      break;
    case WM_RBUTTONUP:
      ReleaseCapture();
      break;
    case WM_MBUTTONDOWN:
      SetCapture(hMainWindow);
      break;
    case WM_MBUTTONUP:
      ReleaseCapture();
      break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
      switch((int) wParam)
      {
      case VK_F1:
        //ScreenShot();
        //log_cout << "hey, who broke screenshot?" << endl;
        break;
      case VK_LEFT:
        application::key::state[application::key::left] = true;
        pane_->key_down(application::key::left);
        break;
      case VK_RIGHT:
        application::key::state[application::key::right] = true;
        pane_->key_down(application::key::right);
        break;
      case VK_UP:
        application::key::state[application::key::up] = true;
        pane_->key_down(application::key::up);
        break;
      case VK_DOWN:
        application::key::state[application::key::down] = true;
        pane_->key_down(application::key::down);
        break;
      case VK_RETURN:
        application::key::state[application::key::enter] = true;
        pane_->key_down(application::key::enter);
        break;
      case VK_BACK:
        application::key::state[application::key::backspace] = true;
        pane_->key_down(application::key::backspace);
        break;
      case VK_DELETE:
        application::key::state[application::key::del] = true;
        pane_->key_down(application::key::del);
        break;
      case VK_END:
        application::key::state[application::key::end] = true;
        pane_->key_down(application::key::end);
        break;
      case VK_HOME:
        application::key::state[application::key::home] = true;
        pane_->key_down(application::key::home);
        break;
      case VK_INSERT:
        application::key::state[application::key::insert] = true;
        pane_->key_down(application::key::insert);
        break;
      case VK_ESCAPE:
        application::key::state[application::key::escape] = true;
        pane_->key_down(application::key::escape);
        break;
      case VK_TAB:
        application::key::state[application::key::tab] = true;
        pane_->key_down(application::key::tab);
        break;
      case VK_PRIOR:
        application::key::state[application::key::pageup] = true;
        pane_->key_down(application::key::pageup);
        break;
      case VK_NEXT:
        application::key::state[application::key::pagedown] = true;
        pane_->key_down(application::key::pagedown);
        break;
      //case VK_ADD:
      case 187:
        application::key::state['+'] = true;
        pane_->key_down('+');
        break;
      case VK_SUBTRACT:
      case 189:
        application::key::state['-'] = true;
        pane_->key_down('-');
        break;
      case VK_MULTIPLY:
        application::key::state['*'] = true;
        pane_->key_down('*');
        break;
      case VK_DIVIDE:
        application::key::state['/'] = true;
        pane_->key_down('/');
        break;
      default:
        //pane_->key_down((int) wParam);
        break;
      }

      break;
    case WM_CHAR:
      c = (TCHAR) wParam;
      if((c >= 32) && (c <= 126))
      {
        pane_->key_down(c);
      }
      break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
      switch((int) wParam)
      {
      case VK_LEFT:
        application::key::state[application::key::left] = false;
        pane_->key_up(application::key::left);
        break;
      case VK_RIGHT:
        application::key::state[application::key::right] = false;
        pane_->key_up(application::key::right);
        break;
      case VK_UP:
        application::key::state[application::key::up] = false;
        pane_->key_up(application::key::up);
        break;
      case VK_DOWN:
        application::key::state[application::key::down] = false;
        pane_->key_up(application::key::down);
        break;
      case VK_RETURN:
        application::key::state[application::key::enter] = false;
        pane_->key_up(application::key::enter);
        break;
      case VK_BACK:
        application::key::state[application::key::backspace] = false;
        pane_->key_up(application::key::backspace);
        break;
      case VK_DELETE:
        application::key::state[application::key::del] = false;
        pane_->key_up(application::key::del);
        break;
      case VK_END:
        application::key::state[application::key::end] = false;
        pane_->key_up(application::key::end);
        break;
      case VK_HOME:
        application::key::state[application::key::home] = false;
        pane_->key_up(application::key::home);
        break;
      case VK_INSERT:
        application::key::state[application::key::insert] = false;
        pane_->key_up(application::key::insert);
        break;
      case VK_ESCAPE:
        application::key::state[application::key::escape] = false;
        pane_->key_up(application::key::escape);
        break;
      case VK_TAB:
        application::key::state[application::key::tab] = false;
        pane_->key_up(application::key::tab);
        break;
      case VK_PRIOR:
        application::key::state[application::key::pageup] = false;
        pane_->key_up(application::key::pageup);
        break;
      case VK_NEXT:
        application::key::state[application::key::pagedown] = false;
        pane_->key_up(application::key::pagedown);
        break;
      //case VK_ADD:
      case 187:
        application::key::state['+'] = false;
        pane_->key_up('+');
        break;
      case VK_SUBTRACT:
      case 189:
        application::key::state['-'] = false;
        pane_->key_up('-');
        break;
      case VK_MULTIPLY:
        application::key::state['*'] = false;
        pane_->key_up('*');
        break;
      case VK_DIVIDE:
        application::key::state['/'] = false;
        pane_->key_up('/');
        break;
      default:
        //pane_->key_up((int) wParam);
        break;
      }
      break;
    default:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
  }
#else
  void window::key_down(const int k)
  {
    application::key::state[k] = true;
    pane_->key_down(k);
  }

  void window::key_up(const int k)
  {
    application::key::state[k] = false;
    pane_->key_up(k);
  }

  void window::nsMainWndProc(XEvent *e)
  {
    KeySym k;
    int c;
    switch(e->type)
    {
    case FocusIn:
      active = true;
      cout << "active true" << endl;
      break;
    case FocusOut:
      active = false;
      cout << "active false" << endl;
      break;
    case MapNotify:
      //GUIInitialiseOpenGL();
      break;
    case DestroyNotify:
      XDestroyWindow(dpy, glwin);
      break;
    case UnmapNotify:
      break;
    case ConfigureNotify:
      //size_ = make_vector<int>(e->xconfigure.width, e->xconfigure.height);
      break;
    case Expose:
      pane_->draw(size());
      break;
    case MotionNotify:
      pane_->mouse_move(make_vector(e->xmotion.x, e->xmotion.y));
      break;
    case ButtonPress:
      switch(e->xbutton.button)
      {
      case Button1:
        pane_->mouse_down(make_vector(e->xbutton.x, e->xbutton.y));
        break;
      case Button2:
        break;
      case Button3:
        break;
      }
      break;
    case ButtonRelease:
      switch(e->xbutton.button)
      {
      case Button1:
        pane_->mouse_up(make_vector(e->xbutton.x, e->xbutton.y));
        break;
      case Button2:
        break;
      case Button3:
        break;
      }
      break;
    case KeyPress:
      XLookupString((XKeyEvent *) e, NULL, 0, &k, NULL);
      c = k;
      if((c >= 32) && (c <= 126))
      {
        //log_cout << int(c) << " key_down" << endl;
        //pane_->key_down(c);
        key_down(c);
      }
      else
      {
        switch(c)
        {
        case XK_Sys_Req:
        case XK_Print:
          //log_cout << "screenshot?!" << endl;
          break;
        case XK_space:
          key_down(' ');
          break;
        case XK_Left:
        case XK_KP_Left:
          key_down(application::key::left);
          break;
        case XK_Right:
        case XK_KP_Right:
          key_down(application::key::right);
          break;
        case XK_Up:
        case XK_KP_Up:
          key_down(application::key::up);
          break;
        case XK_Down:
        case XK_KP_Down:
          key_down(application::key::down);
          break;
        case XK_Return:
          key_down(application::key::enter);
          break;
        case XK_BackSpace:
          key_down(application::key::backspace);
          break;
        case XK_Delete:
        case XK_KP_Delete:
          key_down(application::key::del);
          break;
        case XK_End:
        case XK_KP_End:
          key_down(application::key::end);
          break;
        case XK_Home:
        case XK_KP_Home:
          key_down(application::key::home);
          break;
        case XK_Insert:
        case XK_KP_Insert:
          key_down(application::key::insert);
          break;
        case XK_Escape:
          key_down(application::key::escape);
          break;
        case XK_Tab:
          key_down(application::key::tab);
          break;
        case XK_KP_Equal:
          key_down('+');
          break;
        case XK_KP_Subtract:
          key_down('-');
          break;
        case XK_KP_Multiply:
          key_down('*');
          break;
        case XK_KP_Divide:
          key_down('/');
          break;
        case XK_Page_Up:
        case XK_KP_Page_Up:
          key_down(application::key::pageup);
          break;
        case XK_Page_Down:
        case XK_KP_Page_Down:
          key_down(application::key::pagedown);
          break;
        default:
          break;
        }
      }
      break;

    case KeyRelease:
      XLookupString((XKeyEvent *) e, NULL, 0, &k, NULL);
      c = k;
      if((c >= 32) && (c <= 126))
      {
        // Visible character. Send it to the GUI
        // log_cout << int(c) << " up" << endl;
        // pane_->key_up(c);
        key_up(c);
      }
      else
      {
        switch(c)
        {
        case XK_space:
          key_up(' ');
          break;
        case XK_Left:
        case XK_KP_Left:
          key_up(application::key::left);
          break;
        case XK_Right:
        case XK_KP_Right:
          key_up(application::key::right);
          break;
        case XK_Up:
        case XK_KP_Up:
          key_up(application::key::up);
          break;
        case XK_Down:
        case XK_KP_Down:
          key_up(application::key::down);
          break;
        case XK_Return:
          key_up(application::key::enter);
          break;
        case XK_BackSpace:
          key_up(application::key::backspace);
          break;
        case XK_Delete:
        case XK_KP_Delete:
          key_up(application::key::del);
          break;
        case XK_End:
        case XK_KP_End:
          key_up(application::key::end);
          break;
        case XK_Home:
        case XK_KP_Home:
          key_up(application::key::home);
          break;
        case XK_Insert:
        case XK_KP_Insert:
          key_up(application::key::insert);
          break;
        case XK_Escape:
          key_up(application::key::escape);
          break;
        case XK_Tab:
          key_up(application::key::tab);
          break;
        case XK_KP_Equal:
          key_up('+');
          break;
        case XK_KP_Subtract:
          key_up('-');
          break;
        case XK_KP_Multiply:
          key_up('*');
          break;
        case XK_KP_Divide:
          key_up('/');
          break;
        case XK_Page_Up:
        case XK_KP_Page_Up:
          key_up(application::key::pageup);
          break;
        case XK_Page_Down:
        case XK_KP_Page_Down:
          key_up(application::key::pagedown);
          break;
        default:
          break;
        }
      }
      break;
    default:
      break;
    }
  }
#endif

} // grwb
