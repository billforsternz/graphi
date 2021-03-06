//
// wxWidgets "Hello world" Program, adjusted
//

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "scaler.h"
#include "console.h"
#include "uart.h"
#include "conio.h"
#include "OptionsDialog.h"
#include "Repository.h"

//#define TEMPERATURE_HACK
static bool clear_request=false;

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
    Repository repository;
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnIdleCore();
    void OnChar( wxKeyEvent &event );
    void OnTimeout(wxTimerEvent& event);

    MyCanvas *m_canvas;
    wxTimer   m_timer;
    wxDECLARE_EVENT_TABLE();
};


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

void MyFrame::OnIdle(wxIdleEvent& event)
{
    OnIdleCore();
}

void MyFrame::OnIdleCore()
{
    //printf("*");
    char buf[1026];
    unsigned int nbr_available = uart.poll();
    bool new_data = false;
    while( nbr_available > 0 )
    {
        unsigned int nbr_read = uart.read( buf, sizeof(buf)-2 );
        buf[nbr_read] = '\0';
#ifdef TEMPERATURE_HACK
        if(nbr_read == 1 )
        {
            char buf2[100];
            sprintf(buf2,"character read = %d, temperature = %d degrees C\r\n", buf[0], buf[0]+72 );
            _cputs(buf2);
        }
#else
        for( unsigned int i=0; i<nbr_read; i++ )
        {
            if( buf[i] == '\0' )
                buf[i] = 0x7f;  // DEL character
        }
        _cputs(buf);
#endif
        new_data |= collect(buf);
        nbr_available = uart.poll();
    }
    while( _kbhit() )
    {
        buf[0] = _getch();
        if( buf[0] == '\x1b' ) // ESC ?
        {
            clear_request = true;
            m_canvas->Update();
            m_canvas->Refresh();
        }
        else
            uart.write( buf, 1 );
    }
    if( new_data )
    {
	    m_canvas->Update();
	    m_canvas->Refresh();
    }
}

void MyFrame::OnChar(wxKeyEvent& event)
{
    // We never reach here for some reason - keep focus on console instead
    long keycode = event.GetKeyCode();
	printf( "OnChar() keycode=%lu\n", keycode );
}


enum
{
    ID_Hello = 1,
    ID_Options
};


wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Hello,   MyFrame::OnHello)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_MENU(ID_Options, MyFrame::OnOptions)
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
    scaler_init(frame->repository.options.m_y_min,frame->repository.options.m_y_max);
    int com_port = frame->repository.options.m_com_port;
    char buf[80];
    sprintf( buf, "COM%d", com_port );
#ifdef TEMPERATURE_HACK
    bool ok = uart.open( buf, 9600, 1, 'N' );
#else
    bool ok = uart.open( buf, 115200, 1, 'N' );
#endif
    printf( "uart.open(COM%d) returns %s\n", com_port, ok?"ok":"fail");
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
    wxMenu *menuOptions = new wxMenu;
    menuOptions->Append(ID_Options, "Options", "Options" );
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuHelp, "&Help" );
    menuBar->Append( menuOptions, "&Options" );
    SetMenuBar( menuBar );
    CreateStatusBar();
#ifdef TEMPERATURE_HACK
    SetStatusText( "COM4 9600 terminal plus graphing" );
#else
    SetStatusText( "COM4 115K terminal plus graphing" );
#endif
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
#ifdef TEMPERATURE_HACK
    wxMessageBox( "Serial 9600 terminal plus graphing, work in progress",
                  "Serial 9600 terminal", wxOK | wxICON_INFORMATION );
#else
    wxMessageBox( "Serial 115K terminal plus graphing, work in progress",
                  "Serial 115K terminal", wxOK | wxICON_INFORMATION );
#endif
}
void MyFrame::OnOptions(wxCommandEvent& event)
{
    OptionsDialog dialog( repository.options, this );
    if( wxID_OK == dialog.ShowModal() )
    {
        repository.options = dialog.dat;
    }
}

void MyFrame::OnHello(wxCommandEvent& event)
{
#ifdef TEMPERATURE_HACK
    wxLogMessage( "Serial 9600 terminal plus graphing, work in progress" );
#else
    wxLogMessage("Serial 115K terminal plus graphing, work in progress");
#endif
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
                           wxHSCROLL | wxVSCROLL | wxFULL_REPAINT_ON_RESIZE)
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
    if( clear_request ) {
        clear_request = false;
        samples_clear( pdc, this, m_owner );
    }
    else
        samples_draw( pdc, this, m_owner );
}

