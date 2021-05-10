#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <algorithm>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "scaler.h"

#define NBR_CHANNELS 10
struct LINE {
	int nbr_channels;
	double samples[NBR_CHANNELS];
};
std::deque<LINE> data;

#define LO -4
#define HI +4
#define WAVFILE_SAMPLES_PER_SECOND 44100
const int NUM_SAMPLES = WAVFILE_SAMPLES_PER_SECOND*2;
double waveform[NUM_SAMPLES];
double frequency = 440.0;
int volume = 4;
static bool redraw;

static std::vector<std::string> labels;
static std::vector<double> values;

static const WORD MAX_CONSOLE_LINES = 500;      // maximum mumber of lines the output console should have
#define NBR_HORIZ_LINES 20

void scaler_init()
{
	scaler( LO, HI, 20 );
    for( int i=0; i<NUM_SAMPLES;i++ )
    {
        double t = (double) i / WAVFILE_SAMPLES_PER_SECOND;
        waveform[i] = volume*sin(frequency*t*2*M_PI);
    }
}

void scaler( double lo, double hi, int nbr_lines ) //, std::vector<std::string> &labels, std::vector<double> &values )
{
	labels.clear();
	values.clear();
	char buf[100];
    bool force_zero = (hi>0 && lo<0);
	sprintf( buf, "%2.e", (hi-lo)/nbr_lines );
    double gap = atof(buf);
	double base = gap;
	while( base < lo )
		base += gap;
	while( base > lo )
		base -= gap;
    double x = force_zero ? 0.0 : base;
	while( x <= hi )
	{
		values.push_back(x);
		x += gap;
	}
    if( force_zero )
    {
        x = 0.0-gap;
	    while( x >= lo )
	    {
		    values.push_back(x);
		    x -= gap;
	    }
    }
    std::sort( values.begin(), values.end() );
    for( double v: values )
    {
		sprintf(buf,"%.2f",v);
		labels.push_back(std::string(buf));
    }
	int i=0;
    for( std::string s: labels )
    {
        printf( "%s: (%f)\n", s.c_str(), values[i++] );
    }
}

void collect( char c )
{
	static enum {START_OF_LINE,IN_VALUE1,IN_VALUE2,IN_VALUE3,BETWEEN_VALUE,EXPECT_EOL,NORMAL} state;
	static int channel, idx;
	static char buf[256];
	static LINE line;
	if( c=='\r' || c=='\n' )
	{
		if( state==EXPECT_EOL )
		{
			line.nbr_channels = channel;
			data.push_back( line );
			redraw = true;
		}
		state = START_OF_LINE;
		channel = 0;
		idx = 0;
	}
	else
	{
		if( idx > sizeof(buf)-2 || channel>=NBR_CHANNELS )
			state = NORMAL;
		switch( state )
		{
			case START_OF_LINE:
			{
				if( c == ';' )
					state = IN_VALUE1;
				else
					state = NORMAL;
				break;
			}
			case BETWEEN_VALUE:
			case IN_VALUE1:
			{
				if( c=='-' || ('0'<=c&&c<='9') )
				{
					buf[idx++] = c;
					state = IN_VALUE2;
				}
				else
					state = NORMAL;
				break;
			}
			case IN_VALUE2:
			{
				if( '0'<=c && c<='9' )
					buf[idx++] = c;
				else if( c == '.')
				{
					buf[idx++] = c;
					state = IN_VALUE3;
				}
				else if( c == ' ' || c==';' )
				{
					buf[idx++] = '\0';
					line.samples[channel++] = atof(buf);
					idx = 0;
					state = (c==' '?BETWEEN_VALUE:EXPECT_EOL);
				}
				else
					state = NORMAL;
				break;
			}
			case IN_VALUE3:
			{
				if( '0'<=c && c<='9' )
					buf[idx++] = c;
				else if( c == ' ' || c==';' )
				{
					buf[idx++] = '\0';
					line.samples[channel++] = atof(buf);
					idx = 0;
					state = (c==' '?BETWEEN_VALUE:EXPECT_EOL);
				}
				else
					state = NORMAL;
				break;
			}
			case EXPECT_EOL:
			{
				state = NORMAL;
				break;
			}
		}
	}
}

static unsigned int w, h;
#define Y(y) (  (h-h/64) - (   (double)(h-h/32) * ((double)((y)-(LO)))  / (double)((HI)-(LO))  )   )

void samples_draw(wxDC& pdc, wxWindow *canvas, wxFrame *frame )
{
    wxDC &dc = pdc ;
    canvas->PrepareDC(dc);
	wxSize sz = canvas->GetClientSize();
	w = sz.GetWidth();
	h = sz.GetHeight();
    frame->PrepareDC(dc);
    dc.Clear();


    dc.SetPen(*wxLIGHT_GREY_PEN);
    for( int i = 0; i < labels.size(); i++ )
	{
		int y = Y(values[i]);
        dc.DrawLine(50,y, w, y);
        wxSize sz2= dc.GetTextExtent(labels[i]);
		dc.DrawText(labels[i],50-sz2.x-2,y-sz2.y/2);
	}
    dc.SetPen(*wxRED_PEN);
    int x1 = 50;
    for( int i = 0; i < 100; i++ )
    {
        int x2 = x1+10;
        double y1 = waveform[i];
        if( y1 > HI )
            y1 = HI;
        else if( y1 < LO )
            y1 = LO;
        double y2 = waveform[i+1];
        if( y2 > HI )
            y2 = HI;
        else if( y2 < LO )
            y2 = LO;
        dc.DrawLine(x1,Y(y1),x2,Y(y2));
        x1 = x2;
    }
}


void RedirectIOToConsole()
{
    CONSOLE_SCREEN_BUFFER_INFO coninfo;

    // allocate a console for this app
    AllocConsole();

	// set the screen buffer to be big enough to let us scroll text
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(handle, &coninfo);

	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(handle, coninfo.dwSize);

	short height = 60;
	short width = 80;

	_SMALL_RECT rect;
	rect.Top = 0;
	rect.Left = 0;
	rect.Bottom = height - 1;
	rect.Right = width - 1;

	SetConsoleWindowInfo(handle, TRUE, &rect);            // Set Window Size

	// Redirect the CRT standard input, output, and error handles to the console
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	//Clear the error state for each of the C++ standard stream objects. We need to do this, as
	//attempts to access the standard streams before they refer to a valid target will cause the
	//iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
	//to always occur during startup regardless of whether anything has been read from or written to
	//the console or not.
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();
}


