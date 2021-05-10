//
// wxWidgets "Hello world" Program, adjusted
//

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "scaler.h"
#include "uart.h"
#include "conio.h"

static std::vector<double> values;
static std::vector<std::string> labels;

// Should really be a little more sophisticated about this
#define TIMER_ID 2001
static Uart uart;

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

class MyCanvas;

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnIdleCore();
    void OnChar( wxKeyEvent &event );
    void OnTimeout(wxTimerEvent& event);

    MyCanvas   *m_canvas;
    wxTimer m_timer;
    wxDECLARE_EVENT_TABLE();
};

void MyFrame::OnIdle(wxIdleEvent& event)
{
    OnIdleCore();
}

void MyFrame::OnIdleCore()
{
    //printf("*");
    char buf[1026];
    if( uart.poll() )
    {
        int nbr = uart.read( buf, sizeof(buf)-2 );
        for( int i=0; i<nbr; i++ )
            _putch( buf[i] );
    }
    if( _kbhit() )
    {
        buf[ 0 ] = _getch();
        uart.write( buf, 1 );
    }
}

void MyFrame::OnChar(wxKeyEvent& event)
{
    // We never reach here for some reason - keep focus on console instead
    long keycode = event.GetKeyCode();
	printf( "OnChar() keycode=%lu\n", keycode );
}


// define a scrollable canvas for drawing onto
class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas( MyFrame *parent );

    void OnPaint(wxPaintEvent &event);
    void Draw(wxDC& dc);
    MyFrame *m_owner;
    wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_Hello = 1
};


wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Hello,   MyFrame::OnHello)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_IDLE(MyFrame::OnIdle)
    EVT_TIMER( TIMER_ID, MyFrame::OnTimeout)
    EVT_CHAR( MyFrame::OnChar )
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( "Terminal with graph", wxPoint(50, 50), wxSize(800, 600) );
    frame->Show( true );
	RedirectIOToConsole();
    scaler_init();
    bool ok = uart.open( "COM4", 115200, 1, 'N' );
    printf( "uart.open() returns %s\n", ok?"ok":"fail");
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "To do");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuHelp, "&Help" );
    SetMenuBar( menuBar );
    CreateStatusBar();
    SetStatusText( "COM4 115K terminal plus graphing" );
	m_canvas = new MyCanvas( this );
    m_timer.SetOwner(this,TIMER_ID);
    m_timer.Start( 100, true );
}

void MyFrame::OnTimeout( wxTimerEvent& WXUNUSED(event) )
{
    OnIdleCore();
    m_timer.Start( 100, true );
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close( true );
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
	m_canvas->Update();
	m_canvas->Refresh();
    wxMessageBox( "COM4 115K terminal plus graphing, work in progress",
                  "COM4 115K terminal", wxOK | wxICON_INFORMATION );
}
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("COM4 115K terminal plus graphing, work in progress");
}


// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT  (MyCanvas::OnPaint)
//    EVT_MOTION (MyCanvas::OnMouseMove)
//    EVT_LEFT_DOWN (MyCanvas::OnMouseDown)
//    EVT_LEFT_UP (MyCanvas::OnMouseUp)
wxEND_EVENT_TABLE()


MyCanvas::MyCanvas(MyFrame *parent)
        : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE)
{
    m_owner = parent;
}

void MyCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
#if 1
    {
        wxBufferedPaintDC bpdc(this);
        Draw(bpdc);
    }
#else
    {
        wxPaintDC pdc(this);
        Draw(pdc);
    }
#endif
}


void MyCanvas::Draw(wxDC& pdc)
{
    samples_draw(pdc,this,m_owner);
}

