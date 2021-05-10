// wxWidgets "Hello World" Program
#include <wx/wx.h>

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame();
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};
enum
{
    ID_Hello = 1
};
wxIMPLEMENT_APP(MyApp);
bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}
MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "Hello World")
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
        "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
        "About Hello World", wxOK | wxICON_INFORMATION);
}
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

#if 0


/////////////////////////////////////////////////////////////////////////////
// Name:        samples/drawing/drawing.cpp
// Purpose:     shows and tests wxDC features
// Author:      Robert Roebling
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wx.h"
#include "wx/colordlg.h"
#include "wx/image.h"
#include "wx/artprov.h"
#include "wx/dcbuffer.h"
#include "wx/dcgraph.h"
#include "wx/overlay.h"
#include "wx/graphics.h"
#include "wx/filename.h"
#include "wx/metafile.h"
#include "wx/settings.h"
#if wxUSE_SVG
#include "wx/dcsvg.h"
#endif
#if wxUSE_POSTSCRIPT
#include "wx/dcps.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// Standard DC supports drawing with alpha on OSX and GTK3.
#if defined(__WXOSX__) || defined(__WXGTK3__)
#define wxDRAWING_DC_SUPPORTS_ALPHA 1
#else
#define wxDRAWING_DC_SUPPORTS_ALPHA 0
#endif // __WXOSX__ || __WXGTK3__

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

static wxBitmap *gs_bmpNoMask = NULL,
                *gs_bmpWithColMask = NULL,
                *gs_bmpMask = NULL,
                *gs_bmpWithMask = NULL,
                *gs_bmp4 = NULL,
                *gs_bmp4_mono = NULL,
                *gs_bmp36 = NULL;

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;

    virtual int OnExit() wxOVERRIDE { DeleteBitmaps(); return 0; }

protected:
    void DeleteBitmaps();

    bool LoadImages();
};

class MyFrame;

// define a scrollable canvas for drawing onto
class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas( MyFrame *parent );

    void OnPaint(wxPaintEvent &event);
    void OnMouseMove(wxMouseEvent &event);
    void OnMouseDown(wxMouseEvent &event);
    void OnMouseUp(wxMouseEvent &event);

    void ToShow(int show) { m_show = show; Refresh(); }
    int GetPage() { return m_show; }

    // set or remove the clipping region
    void Clip(bool clip) { m_clip = clip; Refresh(); }
#if wxUSE_GRAPHICS_CONTEXT
    bool HasRenderer() const { return m_renderer != NULL; }
    void UseGraphicRenderer(wxGraphicsRenderer* renderer);
    bool IsDefaultRenderer() const
    {   if ( !m_renderer ) return false;
        return m_renderer == wxGraphicsRenderer::GetDefaultRenderer();
    }
    wxGraphicsRenderer* GetRenderer() const { return m_renderer; }
    void EnableAntiAliasing(bool use) { m_useAntiAliasing = use; Refresh(); }
#endif // wxUSE_GRAPHICS_CONTEXT
    void UseBuffer(bool use) { m_useBuffer = use; Refresh(); }
    void ShowBoundingBox(bool show) { m_showBBox = show; Refresh(); }
    void GetDrawingSize(int* width, int* height) const;

    void Draw(wxDC& dc);

protected:
    enum DrawMode
    {
        Draw_Normal,
        Draw_Stretch
    };

    void DrawTestLines( int x, int y, int width, wxDC &dc );
    void DrawCrossHair(int x, int y, int width, int heigth, wxDC &dc);
    void DrawTestPoly(wxDC& dc);
    void DrawTestBrushes(wxDC& dc);
    void DrawText(wxDC& dc);
    void DrawImages(wxDC& dc, DrawMode mode);
    void DrawWithLogicalOps(wxDC& dc);
#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    void DrawAlpha(wxDC& dc);
#endif
#if wxUSE_GRAPHICS_CONTEXT
    void DrawGraphics(wxGraphicsContext* gc);
#endif
    void DrawRegions(wxDC& dc);
    void DrawCircles(wxDC& dc);
    void DrawSplines(wxDC& dc);
    void DrawDefault(wxDC& dc);
    void DrawGradients(wxDC& dc);
    void DrawSystemColours(wxDC& dc);

    void DrawRegionsHelper(wxDC& dc, wxCoord x, bool firstTime);

private:
    MyFrame *m_owner;

    int          m_show;
    wxBitmap     m_smile_bmp;
    wxIcon       m_std_icon;
    bool         m_clip;
    wxOverlay    m_overlay;
    bool         m_rubberBand;
    wxPoint      m_anchorpoint;
    wxPoint      m_currentpoint;
#if wxUSE_GRAPHICS_CONTEXT
    wxGraphicsRenderer* m_renderer;
    bool         m_useAntiAliasing;
#endif
    bool         m_useBuffer;
    bool         m_showBBox;
    wxCoord      m_sizeX;
    wxCoord      m_sizeY;

    wxDECLARE_EVENT_TABLE();
};


// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnClip(wxCommandEvent& event);

#if wxUSE_GRAPHICS_CONTEXT
    void OnGraphicContextNone(wxCommandEvent& WXUNUSED(event))
    {
        m_canvas->UseGraphicRenderer(NULL);
    }

    void OnGraphicContextDefault(wxCommandEvent& WXUNUSED(event))
    {
        m_canvas->UseGraphicRenderer(wxGraphicsRenderer::GetDefaultRenderer());
    }

#if wxUSE_CAIRO
    void OnGraphicContextCairo(wxCommandEvent& WXUNUSED(event))
    {
        m_canvas->UseGraphicRenderer(wxGraphicsRenderer::GetCairoRenderer());
    }
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    void OnGraphicContextGDIPlus(wxCommandEvent& WXUNUSED(event))
    {
        m_canvas->UseGraphicRenderer(wxGraphicsRenderer::GetGDIPlusRenderer());
    }
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    void OnGraphicContextDirect2D(wxCommandEvent& WXUNUSED(event))
    {
        m_canvas->UseGraphicRenderer(wxGraphicsRenderer::GetDirect2DRenderer());
    }
#endif
#endif // __WXMSW__
    void OnAntiAliasing(wxCommandEvent& event)
    {
        m_canvas->EnableAntiAliasing(event.IsChecked());
    }

    void OnAntiAliasingUpdateUI(wxUpdateUIEvent& event)
    {
        event.Enable(m_canvas->GetRenderer() != NULL);
    }
#endif // wxUSE_GRAPHICS_CONTEXT

    void OnBuffer(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnShow(wxCommandEvent &event);
    void OnOption(wxCommandEvent &event);
    void OnBoundingBox(wxCommandEvent& evt);
    void OnBoundingBoxUpdateUI(wxUpdateUIEvent& evt);

#if wxUSE_COLOURDLG
    wxColour SelectColour();
#endif // wxUSE_COLOURDLG
    void PrepareDC(wxDC& dc) wxOVERRIDE;

    int         m_backgroundMode;
    int         m_textureBackground;
    wxMappingMode m_mapMode;
    double      m_xUserScale;
    double      m_yUserScale;
    int         m_xLogicalOrigin;
    int         m_yLogicalOrigin;
    bool        m_xAxisReversed,
                m_yAxisReversed;
#if wxUSE_DC_TRANSFORM_MATRIX
    wxDouble    m_transform_dx;
    wxDouble    m_transform_dy;
    wxDouble    m_transform_scx;
    wxDouble    m_transform_scy;
    wxDouble    m_transform_rot;
#endif // wxUSE_DC_TRANSFORM_MATRIX
    wxColour    m_colourForeground,    // these are _text_ colours
                m_colourBackground;
    wxBrush     m_backgroundBrush;
    MyCanvas   *m_canvas;
    wxMenuItem *m_menuItemUseDC;
private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    File_Quit = wxID_EXIT,
    File_About = wxID_ABOUT,

    MenuShow_First = wxID_HIGHEST,
    File_ShowDefault = MenuShow_First,
    File_ShowText,
    File_ShowLines,
    File_ShowBrushes,
    File_ShowPolygons,
    File_ShowMask,
    File_ShowMaskStretch,
    File_ShowOps,
    File_ShowRegions,
    File_ShowCircles,
    File_ShowSplines,
#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    File_ShowAlpha,
#endif // wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
#if wxUSE_GRAPHICS_CONTEXT
    File_ShowGraphics,
#endif
    File_ShowSystemColours,
    File_ShowGradients,
    MenuShow_Last = File_ShowGradients,

#if wxUSE_GRAPHICS_CONTEXT
    File_DC,
    File_GC_Default,
#if wxUSE_CAIRO
    File_GC_Cairo,
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    File_GC_GDIPlus,
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    File_GC_Direct2D,
#endif
#endif // __WXMSW__
#endif // wxUSE_GRAPHICS_CONTEXT
    File_BBox,
    File_Clip,
    File_Buffer,
#if wxUSE_GRAPHICS_CONTEXT
    File_AntiAliasing,
#endif
    File_Copy,
    File_Save,

    MenuOption_First,

    MapMode_Text = MenuOption_First,
    MapMode_Lometric,
    MapMode_Twips,
    MapMode_Points,
    MapMode_Metric,

    UserScale_StretchHoriz,
    UserScale_ShrinkHoriz,
    UserScale_StretchVertic,
    UserScale_ShrinkVertic,
    UserScale_Restore,

    AxisMirror_Horiz,
    AxisMirror_Vertic,

    LogicalOrigin_MoveDown,
    LogicalOrigin_MoveUp,
    LogicalOrigin_MoveLeft,
    LogicalOrigin_MoveRight,
    LogicalOrigin_Set,
    LogicalOrigin_Restore,

#if wxUSE_DC_TRANSFORM_MATRIX
    TransformMatrix_Set,
    TransformMatrix_Reset,
#endif // wxUSE_DC_TRANSFORM_MATRIX

#if wxUSE_COLOURDLG
    Colour_TextForeground,
    Colour_TextBackground,
    Colour_Background,
#endif // wxUSE_COLOURDLG
    Colour_BackgroundMode,
    Colour_TextureBackgound,

    MenuOption_Last = Colour_TextureBackgound
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------


// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

bool MyApp::LoadImages()
{
    gs_bmpNoMask = new wxBitmap;
    gs_bmpWithColMask = new wxBitmap;
    gs_bmpMask = new wxBitmap;
    gs_bmpWithMask = new wxBitmap;
    gs_bmp4 = new wxBitmap;
    gs_bmp4_mono = new wxBitmap;
    gs_bmp36 = new wxBitmap;

    wxPathList pathList;
    // special hack for Unix in-tree sample build, don't do this in real
    // programs, use wxStandardPaths instead
    pathList.Add(wxFileName(argv[0]).GetPath());
    pathList.Add(".");
    pathList.Add("..");
    pathList.Add("../drawing");
    pathList.Add("../../../samples/drawing");

    wxString path = pathList.FindValidPath("pat4.bmp");
    if ( !path )
        return false;

    /* 4 colour bitmap */
    gs_bmp4->LoadFile(path, wxBITMAP_TYPE_BMP);
    /* turn into mono-bitmap */
    gs_bmp4_mono->LoadFile(path, wxBITMAP_TYPE_BMP);
    wxMask* mask4 = new wxMask(*gs_bmp4_mono, *wxBLACK);
    gs_bmp4_mono->SetMask(mask4);

    path = pathList.FindValidPath("pat36.bmp");
    if ( !path )
        return false;
    gs_bmp36->LoadFile(path, wxBITMAP_TYPE_BMP);
    wxMask* mask36 = new wxMask(*gs_bmp36, *wxBLACK);
    gs_bmp36->SetMask(mask36);

    path = pathList.FindValidPath("image.bmp");
    if ( !path )
        return false;
    gs_bmpNoMask->LoadFile(path, wxBITMAP_TYPE_BMP);
    gs_bmpWithMask->LoadFile(path, wxBITMAP_TYPE_BMP);
    gs_bmpWithColMask->LoadFile(path, wxBITMAP_TYPE_BMP);

    path = pathList.FindValidPath("mask.bmp");
    if ( !path )
        return false;
    gs_bmpMask->LoadFile(path, wxBITMAP_TYPE_BMP);

    wxMask *mask = new wxMask(*gs_bmpMask, *wxBLACK);
    gs_bmpWithMask->SetMask(mask);

    mask = new wxMask(*gs_bmpWithColMask, *wxWHITE);
    gs_bmpWithColMask->SetMask(mask);

    return true;
}

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if wxUSE_LIBPNG
      wxImage::AddHandler( new wxPNGHandler );
#endif

    // Create the main application window
    MyFrame *frame = new MyFrame("Drawing sample",
                                 wxDefaultPosition, wxSize(550, 840));

    // Show it
    frame->Show(true);

    if ( !LoadImages() )
    {
        wxLogError("Can't load one of the bitmap files needed "
                   "for this sample from the current or parent "
                   "directory, please copy them there.");

        // still continue, the sample can be used without images too if they're
        // missing for whatever reason
    }

    return true;
}

void MyApp::DeleteBitmaps()
{
    wxDELETE(gs_bmpNoMask);
    wxDELETE(gs_bmpWithColMask);
    wxDELETE(gs_bmpMask);
    wxDELETE(gs_bmpWithMask);
    wxDELETE(gs_bmp4);
    wxDELETE(gs_bmp4_mono);
    wxDELETE(gs_bmp36);
}

// ----------------------------------------------------------------------------
// MyCanvas
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
wxBEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_PAINT  (MyCanvas::OnPaint)
    EVT_MOTION (MyCanvas::OnMouseMove)
    EVT_LEFT_DOWN (MyCanvas::OnMouseDown)
    EVT_LEFT_UP (MyCanvas::OnMouseUp)
wxEND_EVENT_TABLE()

#include "smile.xpm"

MyCanvas::MyCanvas(MyFrame *parent)
        : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxHSCROLL | wxVSCROLL)
{
    m_owner = parent;
    m_show = File_ShowDefault;
    m_smile_bmp = wxBitmap(smile_xpm);
    m_std_icon = wxArtProvider::GetIcon(wxART_INFORMATION);
    m_clip = false;
    m_rubberBand = false;
#if wxUSE_GRAPHICS_CONTEXT
    m_renderer = NULL;
    m_useAntiAliasing = true;
#endif
    m_useBuffer = false;
    m_showBBox = false;
    m_sizeX = 0;
    m_sizeY = 0;
}

void MyCanvas::DrawTestBrushes(wxDC& dc)
{
}

void MyCanvas::DrawTestPoly(wxDC& dc)
{
}

void MyCanvas::DrawTestLines( int x, int y, int width, wxDC &dc )
{
    dc.SetPen( wxPen( *wxBLACK, width ) );
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawText(wxString::Format("Testing lines of width %d", width), x + 10, y - 10);
    dc.DrawRectangle( x+10, y+10, 100, 190 );

    dc.DrawText("Solid/dot/short dash/long dash/dot dash", x + 150, y + 10);
    dc.SetPen( wxPen( *wxBLACK, width ) );
    dc.DrawLine( x+20, y+20, 100, y+20 );
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_DOT) );
    dc.DrawLine( x+20, y+30, 100, y+30 );
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_SHORT_DASH) );
    dc.DrawLine( x+20, y+40, 100, y+40 );
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_LONG_DASH) );
    dc.DrawLine( x+20, y+50, 100, y+50 );
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_DOT_DASH) );
    dc.DrawLine( x+20, y+60, 100, y+60 );

    dc.DrawText("Hatches", x + 150, y + 70);
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_BDIAGONAL_HATCH) );
    dc.DrawLine( x+20, y+70, 100, y+70 );
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_CROSSDIAG_HATCH) );
    dc.DrawLine( x+20, y+80, 100, y+80 );
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_FDIAGONAL_HATCH) );
    dc.DrawLine( x+20, y+90, 100, y+90 );
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_CROSS_HATCH) );
    dc.DrawLine( x+20, y+100, 100, y+100 );
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_HORIZONTAL_HATCH) );
    dc.DrawLine( x+20, y+110, 100, y+110 );
    dc.SetPen( wxPen( *wxBLACK, width, wxPENSTYLE_VERTICAL_HATCH) );
    dc.DrawLine( x+20, y+120, 100, y+120 );

    dc.DrawText("User dash", x + 150, y + 140);
    wxPen ud( *wxBLACK, width, wxPENSTYLE_USER_DASH );
    wxDash dash1[6];
    dash1[0] = 8;  // Long dash  <---------+
    dash1[1] = 2;  // Short gap            |
    dash1[2] = 3;  // Short dash           |
    dash1[3] = 2;  // Short gap            |
    dash1[4] = 3;  // Short dash           |
    dash1[5] = 2;  // Short gap and repeat +
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine( x+20, y+140, 100, y+140 );
    dash1[0] = 5;  // Make first dash shorter
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine( x+20, y+150, 100, y+150 );
    dash1[2] = 5;  // Make second dash longer
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine( x+20, y+160, 100, y+160 );
    dash1[4] = 5;  // Make third dash longer
    ud.SetDashes( 6, dash1 );
    dc.SetPen( ud );
    dc.DrawLine( x+20, y+170, 100, y+170 );

    wxPen penWithCap(*wxBLACK, width);
    dc.SetPen(penWithCap);
    dc.DrawText("Default cap", x+270, y+40);
    dc.DrawLine( x+200, y+50, x+250, y+50);

    penWithCap.SetCap(wxCAP_BUTT);
    dc.SetPen(penWithCap);
    dc.DrawText("Butt ", x+270, y+60);
    dc.DrawLine( x+200, y+70, x+250, y+70);

    penWithCap.SetCap(wxCAP_ROUND);
    dc.SetPen(penWithCap);
    dc.DrawText("Round cap", x+270, y+80);
    dc.DrawLine( x+200, y+90, x+250, y+90);

    penWithCap.SetCap(wxCAP_PROJECTING);
    dc.SetPen(penWithCap);
    dc.DrawText("Projecting cap", x+270, y+100);
    dc.DrawLine( x+200, y+110, x+250, y+110);
}

void MyCanvas::DrawCrossHair(int x, int y, int width, int heigth, wxDC &dc)
{
}

void MyCanvas::DrawDefault(wxDC& dc)
{
}

void MyCanvas::DrawText(wxDC& dc)
{
    // set underlined font for testing
    dc.SetFont( wxFontInfo(12).Family(wxFONTFAMILY_MODERN).Underlined() );
    dc.DrawText( "This is text", 110, 10 );
    dc.DrawRotatedText( "That is text", 20, 10, -45 );

    // use wxSWISS_FONT and not wxNORMAL_FONT as the latter can't be rotated
    // under MSW (it is not TrueType)
    dc.SetFont( *wxSWISS_FONT );

    wxString text;
    dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    for ( int n = -180; n < 180; n += 30 )
    {
        text.Printf("     %d rotated text", n);
        dc.DrawRotatedText(text , 400, 400, n);
    }

    dc.SetFont( wxFontInfo(18).Family(wxFONTFAMILY_SWISS) );

    dc.DrawText( "This is Swiss 18pt text.", 110, 40 );

    wxCoord length;
    wxCoord height;
    wxCoord descent;
    dc.GetTextExtent( "This is Swiss 18pt text.", &length, &height, &descent );
    text.Printf( "Dimensions are length %d, height %d, descent %d", length, height, descent );
    dc.DrawText( text, 110, 80 );

    text.Printf( "CharHeight() returns: %d", dc.GetCharHeight() );
    dc.DrawText( text, 110, 120 );

    dc.DrawRectangle( 100, 40, 4, height );

    // test the logical function effect
    wxCoord y = 150;
    dc.SetLogicalFunction(wxINVERT);
    // text drawing should ignore logical function
    dc.DrawText( "There should be a text below", 110, y );
    dc.DrawRectangle( 110, y, 100, height );

    y += height;
    dc.DrawText( "Visible text", 110, y );
    dc.DrawRectangle( 110, y, 100, height );
    dc.DrawText( "Visible text", 110, y );
    dc.DrawRectangle( 110, y, 100, height );
    dc.SetLogicalFunction(wxCOPY);

    y += height;
    dc.DrawRectangle( 110, y, 100, height );
    dc.DrawText( "Another visible text", 110, y );

    y += height;
    dc.DrawText("And\nmore\ntext on\nmultiple\nlines", 110, y);
    y += 5*height;

    dc.SetTextForeground(*wxBLUE);
    dc.DrawRotatedText("Rotated text\ncan have\nmultiple lines\nas well", 110, y, 15);

    y += 7*height;
    dc.SetFont(wxFontInfo(12).Family(wxFONTFAMILY_TELETYPE));
    dc.SetTextForeground(wxColour(150, 75, 0));
    dc.DrawText("And some text with tab characters:\n123456789012345678901234567890\n\taa\tbbb\tcccc", 10, y);
}

static const struct
{
    wxString name;
    wxRasterOperationMode rop;
} rasterOperations[] =
{
    { "wxAND",          wxAND           },
    { "wxAND_INVERT",   wxAND_INVERT    },
    { "wxAND_REVERSE",  wxAND_REVERSE   },
    { "wxCLEAR",        wxCLEAR         },
    { "wxCOPY",         wxCOPY          },
    { "wxEQUIV",        wxEQUIV         },
    { "wxINVERT",       wxINVERT        },
    { "wxNAND",         wxNAND          },
    { "wxNO_OP",        wxNO_OP         },
    { "wxOR",           wxOR            },
    { "wxOR_INVERT",    wxOR_INVERT     },
    { "wxOR_REVERSE",   wxOR_REVERSE    },
    { "wxSET",          wxSET           },
    { "wxSRC_INVERT",   wxSRC_INVERT    },
    { "wxXOR",          wxXOR           },
};

void MyCanvas::DrawImages(wxDC& dc, DrawMode mode)
{
}

void MyCanvas::DrawWithLogicalOps(wxDC& dc)
{
}

#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
void MyCanvas::DrawAlpha(wxDC& dc)
{
}
#endif // wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT

#if wxUSE_GRAPHICS_CONTEXT

const int BASE  = 80.0;
const int BASE2 = BASE/2;
const int BASE4 = BASE/4;


// modeled along Robin Dunn's GraphicsContext.py sample

void MyCanvas::DrawGraphics(wxGraphicsContext* gc)
{
    wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    gc->SetFont(font,*wxBLACK);

    // make a path that contains a circle and some lines, centered at 0,0
    wxGraphicsPath path = gc->CreatePath() ;
    path.AddCircle( 0, 0, BASE2 );
    path.MoveToPoint(0, -BASE2);
    path.AddLineToPoint(0, BASE2);
    path.MoveToPoint(-BASE2, 0);
    path.AddLineToPoint(BASE2, 0);
    path.CloseSubpath();
    path.AddRectangle(-BASE4, -BASE4/2, BASE2, BASE4);

    // Now use that path to demonstrate various capabilities of the graphics context
    gc->PushState(); // save current translation/scale/other state
    gc->Translate(60, 75); // reposition the context origin

    gc->SetPen(wxPen("navy"));
    gc->SetBrush(wxBrush("pink"));

    for( int i = 0 ; i < 3 ; ++i )
    {
        wxString label;
        switch( i )
        {
            case 0 :
                label = "StrokePath";
                break;
            case 1 :
                label = "FillPath";
                break;
            case 2 :
                label = "DrawPath";
                break;
        }
        wxDouble w, h;
        gc->GetTextExtent(label, &w, &h, NULL, NULL);
        gc->DrawText(label, -w/2, -BASE2-h-4);
        switch( i )
        {
            case 0 :
                gc->StrokePath(path);
                break;
            case 1 :
                gc->FillPath(path);
                break;
            case 2 :
                gc->DrawPath(path);
                break;
        }
        gc->Translate(2*BASE, 0);
    }

    gc->PopState(); // restore saved state
    gc->PushState(); // save it again
    gc->Translate(60, 200); // offset to the lower part of the window

    gc->DrawText("Scale", 0, -BASE2);
    gc->Translate(0, 20);

    gc->SetBrush(wxBrush(wxColour(178,  34,  34, 128)));// 128 == half transparent
    for( int i = 0 ; i < 8 ; ++i )
    {
        gc->Scale(1.08, 1.08); // increase scale by 8%
        gc->Translate(5,5);
        gc->DrawPath(path);
    }

    gc->PopState(); // restore saved state
    gc->PushState(); // save it again
    gc->Translate(400, 200);

    gc->DrawText("Rotate", 0, -BASE2);

    // Move the origin over to the next location
    gc->Translate(0, 75);

    // draw our path again, rotating it about the central point,
    // and changing colors as we go
    for ( int angle = 0 ; angle < 360 ; angle += 30 )
    {
        gc->PushState(); // save this new current state so we can
        //  pop back to it at the end of the loop
        wxImage::RGBValue val = wxImage::HSVtoRGB(wxImage::HSVValue(angle / 360.0, 1, 1));
        gc->SetBrush(wxBrush(wxColour(val.red, val.green, val.blue, 64)));
        gc->SetPen(wxPen(wxColour(val.red, val.green, val.blue, 128)));

        // use translate to artfully reposition each drawn path
        gc->Translate(1.5 * BASE2 * cos(wxDegToRad(angle)),
                     1.5 * BASE2 * sin(wxDegToRad(angle)));

        // use Rotate to rotate the path
        gc->Rotate(wxDegToRad(angle));

        // now draw it
        gc->DrawPath(path);
        gc->PopState();
    }
    gc->PopState();

    gc->PushState();
    gc->Translate(60, 400);
    const wxString labelText("Scaled smiley inside a square");
    gc->DrawText(labelText, 0, 0);
    // Center a bitmap horizontally
    wxDouble textWidth;
    gc->GetTextExtent(labelText, &textWidth, NULL);
    const wxDouble rectWidth = 100;
    wxDouble x0 = (textWidth - rectWidth) / 2;
    gc->DrawRectangle(x0, BASE2, rectWidth, 100);
    gc->DrawBitmap(m_smile_bmp, x0, BASE2, rectWidth, 100);
    gc->PopState();

    // Draw graphics bitmap and its subbitmap
    gc->PushState();
    gc->Translate(300, 400);
    gc->DrawText("Smiley as a graphics bitmap", 0, 0);

    wxGraphicsBitmap gbmp1 = gc->CreateBitmap(m_smile_bmp);
    gc->DrawBitmap(gbmp1, 0, BASE2, 50, 50);
    int bmpw = m_smile_bmp.GetWidth();
    int bmph = m_smile_bmp.GetHeight();
    wxGraphicsBitmap gbmp2 = gc->CreateSubBitmap(gbmp1, 0, bmph/5, bmpw/2, bmph/2);
    gc->DrawBitmap(gbmp2, 80, BASE2, 50, 50*(bmph/2)/(bmpw/2));
    gc->PopState();
}
#endif // wxUSE_GRAPHICS_CONTEXT

void MyCanvas::DrawCircles(wxDC& dc)
{
}

void MyCanvas::DrawSplines(wxDC& dc)
{
#if wxUSE_SPLINES
    dc.DrawText("Some splines", 10, 5);

    // values are hardcoded rather than randomly generated
    // so the output can be compared between native
    // implementations on platforms with different random
    // generators

    const int R = 300;
    const wxPoint center( R + 20, R + 20 );
    const int angles[7] = { 0, 10, 33, 77, 13, 145, 90 };
    const int radii[5] = { 100 , 59, 85, 33, 90 };
    const int numPoints = 200;
    wxPoint pts[numPoints];

    // background spline calculation
    unsigned int radius_pos = 0;
    unsigned int angle_pos = 0;
    int angle = 0;
    for ( int i = 0; i < numPoints; i++ )
    {
        angle += angles[ angle_pos ];
        int r = R * radii[ radius_pos ] / 100;
        pts[ i ].x = center.x + (wxCoord)( r * cos(wxDegToRad(angle)) );
        pts[ i ].y = center.y + (wxCoord)( r * sin(wxDegToRad(angle)) );

        angle_pos++;
        if ( angle_pos >= WXSIZEOF(angles) ) angle_pos = 0;

        radius_pos++;
        if ( radius_pos >= WXSIZEOF(radii) ) radius_pos = 0;
    }

    // background spline drawing
    dc.SetPen(*wxRED_PEN);
    dc.DrawSpline(WXSIZEOF(pts), pts);

    // less detailed spline calculation
    wxPoint letters[4][5];
    // w
    letters[0][0] = wxPoint( 0,1); //  O           O
    letters[0][1] = wxPoint( 1,3); //   *         *
    letters[0][2] = wxPoint( 2,2); //    *   O   *
    letters[0][3] = wxPoint( 3,3); //     * * * *
    letters[0][4] = wxPoint( 4,1); //      O   O
    // x1
    letters[1][0] = wxPoint( 5,1); //  O*O
    letters[1][1] = wxPoint( 6,1); //     *
    letters[1][2] = wxPoint( 7,2); //      O
    letters[1][3] = wxPoint( 8,3); //       *
    letters[1][4] = wxPoint( 9,3); //        O*O
    // x2
    letters[2][0] = wxPoint( 5,3); //        O*O
    letters[2][1] = wxPoint( 6,3); //       *
    letters[2][2] = wxPoint( 7,2); //      O
    letters[2][3] = wxPoint( 8,1); //     *
    letters[2][4] = wxPoint( 9,1); //  O*O
    // W
    letters[3][0] = wxPoint(10,0); //  O           O
    letters[3][1] = wxPoint(11,3); //   *         *
    letters[3][2] = wxPoint(12,1); //    *   O   *
    letters[3][3] = wxPoint(13,3); //     * * * *
    letters[3][4] = wxPoint(14,0); //      O   O

    const int dx = 2 * R / letters[3][4].x;
    const int h[4] = { -R/2, 0, R/4, R/2 };

    for ( int m = 0; m < 4; m++ )
    {
        for ( int n = 0; n < 5; n++ )
        {
            letters[m][n].x = center.x - R + letters[m][n].x * dx;
            letters[m][n].y = center.y + h[ letters[m][n].y ];
        }

        dc.SetPen( wxPen( *wxBLUE, 1, wxPENSTYLE_DOT) );
        dc.DrawLines(5, letters[m]);
        dc.SetPen( wxPen( *wxBLACK, 4) );
        dc.DrawSpline(5, letters[m]);
    }

#else
    dc.DrawText("Splines not supported.", 10, 5);
#endif
}

void MyCanvas::DrawGradients(wxDC& dc)
{
}

void MyCanvas::DrawSystemColours(wxDC& dc)
{
}

void MyCanvas::DrawRegions(wxDC& dc)
{
}

void MyCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    if ( m_useBuffer )
    {
        wxBufferedPaintDC bpdc(this);
        Draw(bpdc);
    }
    else
    {
        wxPaintDC pdc(this);
        Draw(pdc);
    }
}

void MyCanvas::Draw(wxDC& pdc)
{
#if wxUSE_GRAPHICS_CONTEXT
    wxGCDC gdc;

    if ( m_renderer )
    {
        wxGraphicsContext* context;
        if ( wxPaintDC *paintdc = wxDynamicCast(&pdc, wxPaintDC) )
        {
            context = m_renderer->CreateContext(*paintdc);
        }
        else if ( wxMemoryDC *memdc = wxDynamicCast(&pdc, wxMemoryDC) )
        {
            context = m_renderer->CreateContext(*memdc);
        }
#if wxUSE_METAFILE && defined(wxMETAFILE_IS_ENH)
        else if ( wxMetafileDC *metadc = wxDynamicCast(&pdc, wxMetafileDC) )
        {
            context = m_renderer->CreateContext(*metadc);
        }
#endif
        else
        {
            wxFAIL_MSG( "Unknown wxDC kind" );
            return;
        }

        context->SetAntialiasMode(m_useAntiAliasing ? wxANTIALIAS_DEFAULT : wxANTIALIAS_NONE);

        gdc.SetBackground(GetBackgroundColour());
        gdc.SetGraphicsContext(context);
    }

    wxDC &dc = m_renderer ? static_cast<wxDC&>(gdc) : pdc;
#else
    wxDC &dc = pdc ;
#endif

    // Adjust scrolled contents for screen drawing operations only.
    if ( wxDynamicCast(&pdc, wxBufferedPaintDC) ||
         wxDynamicCast(&pdc, wxPaintDC) )
    {
        PrepareDC(dc);
    }

    m_owner->PrepareDC(dc);

    dc.SetBackgroundMode( m_owner->m_backgroundMode );
    if ( m_owner->m_backgroundBrush.IsOk() )
        dc.SetBackground( m_owner->m_backgroundBrush );
    if ( m_owner->m_colourForeground.IsOk() )
        dc.SetTextForeground( m_owner->m_colourForeground );
    if ( m_owner->m_colourBackground.IsOk() )
        dc.SetTextBackground( m_owner->m_colourBackground );

    if ( m_owner->m_textureBackground) {
        if ( ! m_owner->m_backgroundBrush.IsOk() ) {
            dc.SetBackground(wxBrush(wxColour(0, 128, 0)));
        }
    }

    if ( m_clip )
        dc.SetClippingRegion(100, 100, 100, 100);

    dc.Clear();

    if ( m_owner->m_textureBackground )
    {
        dc.SetPen(*wxMEDIUM_GREY_PEN);
        for ( int i = 0; i < 200; i++ )
            dc.DrawLine(0, i*10, i*10, 0);
    }

    switch ( m_show )
    {
        case File_ShowDefault:
            DrawDefault(dc);
            break;

        case File_ShowCircles:
            DrawCircles(dc);
            break;

        case File_ShowSplines:
            DrawSplines(dc);
            break;

        case File_ShowRegions:
            DrawRegions(dc);
            break;

        case File_ShowText:
            DrawText(dc);
            break;

        case File_ShowLines:
            DrawTestLines( 0, 100, 0, dc );
            DrawTestLines( 0, 320, 1, dc );
            DrawTestLines( 0, 540, 2, dc );
            DrawTestLines( 0, 760, 6, dc );
            DrawCrossHair( 0, 0, 400, 90, dc);
            break;

        case File_ShowBrushes:
            DrawTestBrushes(dc);
            break;

        case File_ShowPolygons:
            DrawTestPoly(dc);
            break;

        case File_ShowMask:
            DrawImages(dc, Draw_Normal);
            break;

        case File_ShowMaskStretch:
            DrawImages(dc, Draw_Stretch);
            break;

        case File_ShowOps:
            DrawWithLogicalOps(dc);
            break;

#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
        case File_ShowAlpha:
            DrawAlpha(dc);
            break;
#endif // wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
#if wxUSE_GRAPHICS_CONTEXT
        case File_ShowGraphics:
            DrawGraphics(gdc.GetGraphicsContext());
            break;
#endif

        case File_ShowGradients:
            DrawGradients(dc);
            break;

        case File_ShowSystemColours:
            DrawSystemColours(dc);
            break;

        default:
            break;
    }

    // For drawing with raw wxGraphicsContext
    // there is no bounding box to obtain.
    if ( m_showBBox
#if wxUSE_GRAPHICS_CONTEXT
         && m_show != File_ShowGraphics
#endif // wxUSE_GRAPHICS_CONTEXT
       )
    {
        dc.SetPen(wxPen(wxColor(0, 128, 0), 1, wxPENSTYLE_DOT));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(dc.MinX(), dc.MinY(), dc.MaxX()-dc.MinX()+1, dc.MaxY()-dc.MinY()+1);
    }

    // Adjust drawing area dimensions only if screen drawing is invoked.
    if ( wxDynamicCast(&pdc, wxBufferedPaintDC) ||
         wxDynamicCast(&pdc, wxPaintDC) )
    {
        wxCoord x0, y0;
        dc.GetDeviceOrigin(&x0, &y0);
        m_sizeX = dc.LogicalToDeviceX(dc.MaxX()) - x0 + 1;
        m_sizeY = dc.LogicalToDeviceY(dc.MaxY()) - y0 + 1;
    }
}

void MyCanvas::OnMouseMove(wxMouseEvent &event)
{
#if wxUSE_STATUSBAR
    {
        wxClientDC dc(this);
        PrepareDC(dc);
        m_owner->PrepareDC(dc);

        wxPoint pos = dc.DeviceToLogical(event.GetPosition());
        wxString str;
        str.Printf( "Current mouse position: %d,%d", pos.x, pos.y );
        m_owner->SetStatusText( str );
    }

    if ( m_rubberBand )
    {
        int x,y, xx, yy ;
        event.GetPosition(&x,&y);
        CalcUnscrolledPosition( x, y, &xx, &yy );
        m_currentpoint = wxPoint( xx , yy ) ;
        wxRect newrect ( m_anchorpoint , m_currentpoint ) ;

        wxClientDC dc( this ) ;
        PrepareDC( dc ) ;

        wxDCOverlay overlaydc( m_overlay, &dc );
        overlaydc.Clear();
#ifdef __WXMAC__
        dc.SetPen( *wxGREY_PEN );
        dc.SetBrush( wxColour( 192,192,192,64 ) );
#else
        dc.SetPen( wxPen( *wxLIGHT_GREY, 2 ) );
        dc.SetBrush( *wxTRANSPARENT_BRUSH );
#endif
        dc.DrawRectangle( newrect );
    }
#else
    wxUnusedVar(event);
#endif // wxUSE_STATUSBAR
}

void MyCanvas::OnMouseDown(wxMouseEvent &event)
{
    int x,y,xx,yy ;
    event.GetPosition(&x,&y);
    CalcUnscrolledPosition( x, y, &xx, &yy );
    m_anchorpoint = wxPoint( xx , yy ) ;
    m_currentpoint = m_anchorpoint ;
    m_rubberBand = true ;
    CaptureMouse() ;
}

void MyCanvas::OnMouseUp(wxMouseEvent &event)
{
    if ( m_rubberBand )
    {
        ReleaseMouse();
        {
            wxClientDC dc( this );
            PrepareDC( dc );
            wxDCOverlay overlaydc( m_overlay, &dc );
            overlaydc.Clear();
        }
        m_overlay.Reset();
        m_rubberBand = false;

        wxPoint endpoint = CalcUnscrolledPosition(event.GetPosition());

        // Don't pop up the message box if nothing was actually selected.
        if ( endpoint != m_anchorpoint )
        {
            wxLogMessage("Selected rectangle from (%d, %d) to (%d, %d)",
                         m_anchorpoint.x, m_anchorpoint.y,
                         endpoint.x, endpoint.y);
        }
    }
}

#if wxUSE_GRAPHICS_CONTEXT
void MyCanvas::UseGraphicRenderer(wxGraphicsRenderer* renderer)
{
    m_renderer = renderer;
    if (renderer)
    {
        int major, minor, micro;
        renderer->GetVersion(&major, &minor, &micro);
        wxString str = wxString::Format("Graphics renderer: %s %i.%i.%i",
                         renderer->GetName(), major, minor, micro);
        m_owner->SetStatusText(str, 1);
    }
    else
    {
        m_owner->SetStatusText(wxEmptyString, 1);
    }

    Refresh();
}
#endif // wxUSE_GRAPHICS_CONTEXT


#if wxUSE_DC_TRANSFORM_MATRIX
#include "wx/valnum.h"

class TransformDataDialog : public wxDialog
{
public:
    TransformDataDialog(wxWindow* parent, wxDouble dx, wxDouble dy, wxDouble scx, wxDouble scy, wxDouble rotAngle)
        : wxDialog(parent, wxID_ANY, "Affine transformation parameters")
        , m_dx(dx)
        , m_dy(dy)
        , m_scx(scx)
        , m_scy(scy)
        , m_rotAngle(rotAngle)
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        const int border = wxSizerFlags::GetDefaultBorder();
        wxFlexGridSizer* paramSizer = new wxFlexGridSizer(2, wxSize(border, border));
        paramSizer->Add(new wxStaticText(this, wxID_ANY, "Translation X:"), wxSizerFlags().CentreVertical());
        wxFloatingPointValidator<wxDouble> val_dx(1, &m_dx, wxNUM_VAL_NO_TRAILING_ZEROES);
        paramSizer->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val_dx), wxSizerFlags().CentreVertical());
        paramSizer->Add(new wxStaticText(this, wxID_ANY, "Translation Y:"), wxSizerFlags().CentreVertical());
        wxFloatingPointValidator<wxDouble> val_dy(1, &m_dy, wxNUM_VAL_NO_TRAILING_ZEROES);
        paramSizer->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val_dy), wxSizerFlags().CentreVertical());
        paramSizer->Add(new wxStaticText(this, wxID_ANY, "Scale X (0.2 - 5):"), wxSizerFlags().CentreVertical());
        wxFloatingPointValidator<wxDouble> val_scx(2, &m_scx, wxNUM_VAL_NO_TRAILING_ZEROES);
        paramSizer->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val_scx), wxSizerFlags().CentreVertical());
        paramSizer->Add(new wxStaticText(this, wxID_ANY, "Scale Y (0.2 - 5):"), wxSizerFlags().CentreVertical());
        wxFloatingPointValidator<wxDouble> val_scy(2, &m_scy, wxNUM_VAL_NO_TRAILING_ZEROES);
        paramSizer->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val_scy), wxSizerFlags().CentreVertical());
        paramSizer->Add(new wxStaticText(this, wxID_ANY, "Rotation angle (deg):"), wxSizerFlags().CentreVertical());
        wxFloatingPointValidator<wxDouble> val_rot(1, &m_rotAngle, wxNUM_VAL_NO_TRAILING_ZEROES);
        paramSizer->Add(new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, val_rot), wxSizerFlags().CentreVertical());
        sizer->Add(paramSizer, wxSizerFlags().DoubleBorder());

        wxSizer *btnSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
        sizer->Add(btnSizer, wxSizerFlags().Expand().Border());

        SetSizerAndFit(sizer);
    }

    virtual bool TransferDataFromWindow() wxOVERRIDE
    {
        if ( !wxDialog::TransferDataFromWindow() )
            return false;

        if ( m_scx < 0.2 || m_scx > 5.0 || m_scy < 0.2 || m_scy > 5.0 )
        {
            if ( !wxValidator::IsSilent() )
                wxBell();

            return false;
        }

        return true;
    }

    void GetTransformationData(wxDouble* dx, wxDouble* dy, wxDouble* scx, wxDouble* scy, wxDouble* rotAngle) const
    {
        if ( dx )
            *dx = m_dx;

        if ( dy )
            *dy = m_dy;

        if ( scx )
            *scx = m_scx;

        if ( scy )
            *scy = m_scy;

        if ( rotAngle )
            *rotAngle = m_rotAngle;
    }

private:
    wxDouble m_dx;
    wxDouble m_dy;
    wxDouble m_scx;
    wxDouble m_scy;
    wxDouble m_rotAngle;
};
#endif // wxUSE_DC_TRANSFORM_MATRIX

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU      (File_Quit,     MyFrame::OnQuit)
    EVT_MENU      (File_About,    MyFrame::OnAbout)
    EVT_MENU      (File_Clip,     MyFrame::OnClip)

#if wxUSE_GRAPHICS_CONTEXT
    EVT_MENU      (File_GC_Default, MyFrame::OnGraphicContextDefault)
    EVT_MENU      (File_DC,         MyFrame::OnGraphicContextNone)
#if wxUSE_CAIRO
    EVT_MENU      (File_GC_Cairo, MyFrame::OnGraphicContextCairo)
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    EVT_MENU      (File_GC_GDIPlus, MyFrame::OnGraphicContextGDIPlus)
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    EVT_MENU      (File_GC_Direct2D, MyFrame::OnGraphicContextDirect2D)
#endif
#endif // __WXMSW__
    EVT_MENU      (File_AntiAliasing, MyFrame::OnAntiAliasing)
    EVT_UPDATE_UI (File_AntiAliasing, MyFrame::OnAntiAliasingUpdateUI)
#endif // wxUSE_GRAPHICS_CONTEXT

    EVT_MENU      (File_Buffer,   MyFrame::OnBuffer)
    EVT_MENU      (File_Copy,     MyFrame::OnCopy)
    EVT_MENU      (File_Save,     MyFrame::OnSave)
    EVT_MENU      (File_BBox,     MyFrame::OnBoundingBox)
    EVT_UPDATE_UI (File_BBox,     MyFrame::OnBoundingBoxUpdateUI)

    EVT_MENU_RANGE(MenuShow_First,   MenuShow_Last,   MyFrame::OnShow)

    EVT_MENU_RANGE(MenuOption_First, MenuOption_Last, MyFrame::OnOption)
wxEND_EVENT_TABLE()

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(sample));

    wxMenu *menuScreen = new wxMenu;
    menuScreen->Append(File_ShowDefault, "&Default screen\tF1");
    menuScreen->Append(File_ShowText, "&Text screen\tF2");
    menuScreen->Append(File_ShowLines, "&Lines screen\tF3");
    menuScreen->Append(File_ShowBrushes, "&Brushes screen\tF4");
    menuScreen->Append(File_ShowPolygons, "&Polygons screen\tF5");
    menuScreen->Append(File_ShowMask, "&Mask screen\tF6");
    menuScreen->Append(File_ShowMaskStretch, "1/&2 scaled mask\tShift-F6");
    menuScreen->Append(File_ShowOps, "&Raster operations screen\tF7");
    menuScreen->Append(File_ShowRegions, "Re&gions screen\tF8");
    menuScreen->Append(File_ShowCircles, "&Circles screen\tF9");
#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    menuScreen->Append(File_ShowAlpha, "&Alpha screen\tF10");
#endif // wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    menuScreen->Append(File_ShowSplines, "Spl&ines screen\tF11");
    menuScreen->Append(File_ShowGradients, "&Gradients screen\tF12");
#if wxUSE_GRAPHICS_CONTEXT
    menuScreen->Append(File_ShowGraphics, "&Graphics screen");
#endif
    menuScreen->Append(File_ShowSystemColours, "System &colours");

    wxMenu *menuFile = new wxMenu;
#if wxUSE_GRAPHICS_CONTEXT
    // Number the different renderer choices consecutively, starting from 0.
    int accel = 0;
    m_menuItemUseDC = menuFile->AppendRadioItem
                      (
                        File_DC,
                        wxString::Format("Use wx&DC\t%d", accel++)
                      );
    menuFile->AppendRadioItem
              (
                File_GC_Default,
                wxString::Format("Use default wx&GraphicContext\t%d", accel++)
              );
#if wxUSE_CAIRO
    menuFile->AppendRadioItem
              (
                File_GC_Cairo,
                wxString::Format("Use &Cairo\t%d", accel++)
              );
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    menuFile->AppendRadioItem
              (
                File_GC_GDIPlus,
                wxString::Format("Use &GDI+\t%d", accel++)
              );
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    menuFile->AppendRadioItem
              (
                File_GC_Direct2D,
                wxString::Format("Use &Direct2D\t%d", accel++)
              );
#endif
#endif // __WXMSW__
#endif // wxUSE_GRAPHICS_CONTEXT
    menuFile->AppendSeparator();
    menuFile->AppendCheckItem(File_BBox, "Show bounding box\tCtrl-E",
                              "Show extents used in drawing operations");
    menuFile->AppendCheckItem(File_Clip, "&Clip\tCtrl-C", "Clip/unclip drawing");
    menuFile->AppendCheckItem(File_Buffer, "&Use wx&BufferedPaintDC\tCtrl-Z", "Buffer painting");
#if wxUSE_GRAPHICS_CONTEXT
    menuFile->AppendCheckItem(File_AntiAliasing,
                              "&Anti-Aliasing in wxGraphicContext\tCtrl-Shift-A",
                              "Enable Anti-Aliasing in wxGraphicContext")
            ->Check();
#endif
    menuFile->AppendSeparator();
#if wxUSE_METAFILE && defined(wxMETAFILE_IS_ENH)
    menuFile->Append(File_Copy, "Copy to clipboard");
#endif
    menuFile->Append(File_Save, "&Save...\tCtrl-S", "Save drawing to file");
    menuFile->AppendSeparator();
    menuFile->Append(File_About, "&About\tCtrl-A", "Show about dialog");
    menuFile->AppendSeparator();
    menuFile->Append(File_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenu *menuMapMode = new wxMenu;
    menuMapMode->Append( MapMode_Text, "&TEXT map mode" );
    menuMapMode->Append( MapMode_Lometric, "&LOMETRIC map mode" );
    menuMapMode->Append( MapMode_Twips, "T&WIPS map mode" );
    menuMapMode->Append( MapMode_Points, "&POINTS map mode" );
    menuMapMode->Append( MapMode_Metric, "&METRIC map mode" );

    wxMenu *menuUserScale = new wxMenu;
    menuUserScale->Append( UserScale_StretchHoriz, "Stretch &horizontally\tCtrl-H" );
    menuUserScale->Append( UserScale_ShrinkHoriz, "Shrin&k horizontally\tCtrl-G" );
    menuUserScale->Append( UserScale_StretchVertic, "Stretch &vertically\tCtrl-V" );
    menuUserScale->Append( UserScale_ShrinkVertic, "&Shrink vertically\tCtrl-W" );
    menuUserScale->AppendSeparator();
    menuUserScale->Append( UserScale_Restore, "&Restore to normal\tCtrl-0" );

    wxMenu *menuAxis = new wxMenu;
    menuAxis->AppendCheckItem( AxisMirror_Horiz, "Mirror horizontally\tCtrl-M" );
    menuAxis->AppendCheckItem( AxisMirror_Vertic, "Mirror vertically\tCtrl-N" );

    wxMenu *menuLogical = new wxMenu;
    menuLogical->Append( LogicalOrigin_MoveDown, "Move &down\tCtrl-D" );
    menuLogical->Append( LogicalOrigin_MoveUp, "Move &up\tCtrl-U" );
    menuLogical->Append( LogicalOrigin_MoveLeft, "Move &right\tCtrl-L" );
    menuLogical->Append( LogicalOrigin_MoveRight, "Move &left\tCtrl-R" );
    menuLogical->AppendSeparator();
    menuLogical->Append( LogicalOrigin_Set, "Set to (&100, 100)\tShift-Ctrl-1" );
    menuLogical->Append( LogicalOrigin_Restore, "&Restore to normal\tShift-Ctrl-0" );

#if wxUSE_DC_TRANSFORM_MATRIX
    wxMenu *menuTransformMatrix = new wxMenu;
    menuTransformMatrix->Append(TransformMatrix_Set, "Set &transformation matrix");
    menuTransformMatrix->AppendSeparator();
    menuTransformMatrix->Append(TransformMatrix_Reset, "Restore to &normal");
#endif // wxUSE_DC_TRANSFORM_MATRIX

    wxMenu *menuColour = new wxMenu;
#if wxUSE_COLOURDLG
    menuColour->Append( Colour_TextForeground, "Text &foreground..." );
    menuColour->Append( Colour_TextBackground, "Text &background..." );
    menuColour->Append( Colour_Background, "Background &colour..." );
#endif // wxUSE_COLOURDLG
    menuColour->AppendCheckItem( Colour_BackgroundMode, "&Opaque/transparent\tCtrl-B" );
    menuColour->AppendCheckItem( Colour_TextureBackgound, "Draw textured back&ground\tCtrl-T" );

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&Drawing");
    menuBar->Append(menuScreen, "Scree&n");
    menuBar->Append(menuMapMode, "&Mode");
    menuBar->Append(menuUserScale, "&Scale");
    menuBar->Append(menuAxis, "&Axis");
    menuBar->Append(menuLogical, "&Origin");
#if wxUSE_DC_TRANSFORM_MATRIX
    menuBar->Append(menuTransformMatrix, "&Transformation");
#endif // wxUSE_DC_TRANSFORM_MATRIX
    menuBar->Append(menuColour, "&Colours");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR

    m_mapMode = wxMM_TEXT;
    m_xUserScale = 1.0;
    m_yUserScale = 1.0;
    m_xLogicalOrigin = 0;
    m_yLogicalOrigin = 0;
    m_xAxisReversed =
    m_yAxisReversed = false;
#if wxUSE_DC_TRANSFORM_MATRIX
    m_transform_dx = 0.0;
    m_transform_dy = 0.0;
    m_transform_scx = 1.0;
    m_transform_scy = 1.0;
    m_transform_rot = 0.0;
#endif // wxUSE_DC_TRANSFORM_MATRIX
    m_backgroundMode = wxBRUSHSTYLE_SOLID;
    m_colourForeground = *wxBLACK;
    m_colourBackground = *wxLIGHT_GREY;
    m_textureBackground = false;

    m_canvas = new MyCanvas( this );
    m_canvas->SetScrollbars( 10, 10, 100, 240 );
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( "This is the about dialog of the drawing sample.\n"
                "This sample tests various primitive drawing functions\n"
                "(without any attempts to prevent flicker).\n"
                "Copyright (c) Robert Roebling 1999"
              );

    wxMessageBox(msg, "About Drawing", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnClip(wxCommandEvent& event)
{
    m_canvas->Clip(event.IsChecked());
}

void MyFrame::OnBuffer(wxCommandEvent& event)
{
    m_canvas->UseBuffer(event.IsChecked());
}

void MyFrame::OnCopy(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_METAFILE && defined(wxMETAFILE_IS_ENH)
    wxMetafileDC dc;
    if (!dc.IsOk())
        return;
    m_canvas->Draw(dc);
    wxMetafile *mf = dc.Close();
    if (!mf)
        return;
    mf->SetClipboard();
    delete mf;
#endif
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
}

void MyFrame::OnShow(wxCommandEvent& event)
{
    const int show = event.GetId();

#if wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    // Make sure we do use a graphics context when selecting one of the screens
    // requiring it.
#if wxDRAWING_DC_SUPPORTS_ALPHA
    // If DC supports drawing with alpha
    // then GC is necessary only for graphics screen.
    if ( show == File_ShowGraphics )
#else // wxUSE_GRAPHICS_CONTEXT
    // DC doesn't support drawing with alpha
    // so GC is necessary both for alpha and graphics screen.
    if ( show == File_ShowAlpha || show == File_ShowGraphics )
#endif // wxDRAWING_DC_SUPPORTS_ALPHA, wxUSE_GRAPHICS_CONTEXT
    {
        if ( !m_canvas->HasRenderer() )
            m_canvas->UseGraphicRenderer(wxGraphicsRenderer::GetDefaultRenderer());
        // Disable selecting wxDC, if necessary.
        m_menuItemUseDC->Enable(!m_canvas->HasRenderer());
    }
    else
    {
        m_menuItemUseDC->Enable(true);
    }
#endif // wxDRAWING_DC_SUPPORTS_ALPHA || wxUSE_GRAPHICS_CONTEXT
    m_canvas->ToShow(show);
}

void MyFrame::OnOption(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case MapMode_Text:
            m_mapMode = wxMM_TEXT;
            break;
        case MapMode_Lometric:
            m_mapMode = wxMM_LOMETRIC;
            break;
        case MapMode_Twips:
            m_mapMode = wxMM_TWIPS;
            break;
        case MapMode_Points:
            m_mapMode = wxMM_POINTS;
            break;
        case MapMode_Metric:
            m_mapMode = wxMM_METRIC;
            break;

        case LogicalOrigin_MoveDown:
            m_yLogicalOrigin += 10;
            break;
        case LogicalOrigin_MoveUp:
            m_yLogicalOrigin -= 10;
            break;
        case LogicalOrigin_MoveLeft:
            m_xLogicalOrigin += 10;
            break;
        case LogicalOrigin_MoveRight:
            m_xLogicalOrigin -= 10;
            break;
        case LogicalOrigin_Set:
            m_xLogicalOrigin =
            m_yLogicalOrigin = -100;
            break;
        case LogicalOrigin_Restore:
            m_xLogicalOrigin =
            m_yLogicalOrigin = 0;
            break;

        case UserScale_StretchHoriz:
            m_xUserScale *= 1.10;
            break;
        case UserScale_ShrinkHoriz:
            m_xUserScale /= 1.10;
            break;
        case UserScale_StretchVertic:
            m_yUserScale *= 1.10;
            break;
        case UserScale_ShrinkVertic:
            m_yUserScale /= 1.10;
            break;
        case UserScale_Restore:
            m_xUserScale =
            m_yUserScale = 1.0;
            break;

        case AxisMirror_Vertic:
            m_yAxisReversed = !m_yAxisReversed;
            break;
        case AxisMirror_Horiz:
            m_xAxisReversed = !m_xAxisReversed;
            break;

#if wxUSE_DC_TRANSFORM_MATRIX
        case TransformMatrix_Set:
            {
                TransformDataDialog dlg(this, m_transform_dx, m_transform_dy,
                    m_transform_scx, m_transform_scy, m_transform_rot);
                if ( dlg.ShowModal() == wxID_OK )
                {
                    dlg.GetTransformationData(&m_transform_dx, &m_transform_dy,
                        &m_transform_scx, &m_transform_scy, &m_transform_rot);
                }
            }
            break;
        case TransformMatrix_Reset:
            m_transform_dx = 0.0;
            m_transform_dy = 0.0;
            m_transform_scx = 1.0;
            m_transform_scy = 1.0;
            m_transform_rot = 0.0;
            break;
#endif // wxUSE_DC_TRANSFORM_MATRIX

#if wxUSE_COLOURDLG
        case Colour_TextForeground:
            m_colourForeground = SelectColour();
            break;
        case Colour_TextBackground:
            m_colourBackground = SelectColour();
            break;
        case Colour_Background:
            {
                wxColour col = SelectColour();
                if ( col.IsOk() )
                {
                    m_backgroundBrush.SetColour(col);
                }
            }
            break;
#endif // wxUSE_COLOURDLG

        case Colour_BackgroundMode:
            m_backgroundMode = m_backgroundMode == wxBRUSHSTYLE_SOLID
                                                 ? wxBRUSHSTYLE_TRANSPARENT
                                                 : wxBRUSHSTYLE_SOLID;
            break;

        case Colour_TextureBackgound:
            m_textureBackground = ! m_textureBackground;
            break;

        default:
            // skip Refresh()
            return;
    }

    m_canvas->Refresh();
}

void MyFrame::OnBoundingBox(wxCommandEvent& evt)
{
    m_canvas->ShowBoundingBox(evt.IsChecked());
}

void MyFrame::OnBoundingBoxUpdateUI(wxUpdateUIEvent& evt)
{
#if wxUSE_GRAPHICS_CONTEXT
    evt.Enable(m_canvas->GetPage() != File_ShowGraphics);
#else
    wxUnusedVar(evt);
#endif // wxUSE_GRAPHICS_CONTEXT / !wxUSE_GRAPHICS_CONTEXT
}

void MyFrame::PrepareDC(wxDC& dc)
{
#if wxUSE_DC_TRANSFORM_MATRIX
    if ( dc.CanUseTransformMatrix() )
    {
        wxAffineMatrix2D mtx;
        mtx.Translate(m_transform_dx, m_transform_dy);
        mtx.Rotate(wxDegToRad(m_transform_rot));
        mtx.Scale(m_transform_scx, m_transform_scy);
        dc.SetTransformMatrix(mtx);
    }
#endif // wxUSE_DC_TRANSFORM_MATRIX
    dc.SetLogicalOrigin( m_xLogicalOrigin, m_yLogicalOrigin );
    dc.SetAxisOrientation( !m_xAxisReversed, m_yAxisReversed );
    dc.SetUserScale( m_xUserScale, m_yUserScale );
    dc.SetMapMode( m_mapMode );
}

#if wxUSE_COLOURDLG
wxColour MyFrame::SelectColour()
{
    wxColour col;
    wxColourData data;
    wxColourDialog dialog(this, &data);

    if ( dialog.ShowModal() == wxID_OK )
    {
        col = dialog.GetColourData().GetColour();
    }

    return col;
}
#endif // wxUSE_COLOURDLG
#endif
