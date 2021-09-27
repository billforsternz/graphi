#include <vector>
#include <deque>
#include <algorithm>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include "scaler.h"

#define NBR_CHANNELS 10
struct LINE {
    unsigned long time=0;
	int nbr_channels;
	double samples[NBR_CHANNELS];
};
static std::deque<LINE> data;
static std::vector<std::string> labels;
static std::vector<double> values;

static double LO;
static double HI;
#define NBR_HORIZ_LINES 40
#define HORIGIN   50
#define HINTERVAL 1
#define MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define HSAMPLES(w) MAX( 0, (((w)-HORIGIN)/HINTERVAL) )

void scaler_init( double lo, double hi )
{
    LO = lo;
    HI = hi;
	int nbr_lines=NBR_HORIZ_LINES;
	char buf[100];
    bool force_zero = (hi>0 && lo<0);
	for( int i=0; i<10; i++ )
	{
		labels.clear();
		values.clear();
		sprintf( buf, "%1.e", (hi-lo)/nbr_lines );
		double gap = atof(buf);
		double base = gap;
		while( base < lo )
			base += gap;
		while( base > lo )
			base -= gap;
		double x = force_zero ? 0.0 : base;
		values.push_back(x);
		while( x < hi )
		{
			x += gap;
			values.push_back(x);
		}
		if( force_zero )
		{
			x = 0.0-gap;
			values.push_back(x);
			while( x >= lo )
			{
				x -= gap;
				values.push_back(x);
			}
		}
		std::sort( values.begin(), values.end() );
		for( double v: values )
		{
			switch(i)
			{
				case 0:	sprintf( buf,  "%.f", v ); break;
				case 1:	sprintf( buf, "%.1f", v ); break;
				case 2:	sprintf( buf, "%.2f", v ); break;
				case 3:	sprintf( buf, "%.3f", v ); break;
				case 4:	sprintf( buf, "%.4f", v ); break;
				case 5:	sprintf( buf, "%.5f", v ); break;
				case 6:	sprintf( buf, "%.6f", v ); break;
				case 7:	sprintf( buf, "%.7f", v ); break;
				case 8:	sprintf( buf, "%.8f", v ); break;
				case 9:	sprintf( buf, "%.9f", v ); break;
			}
			labels.push_back(std::string(buf));
		}
		int j=0;
		/* for( std::string s: labels )
		{
			printf( "%s: (%f)\n", s.c_str(), values[j++] );
		} */
		std::string prev;
		bool reject=false;
		for( std::string s: labels )
		{
			if( s == prev )
				reject = true;	// reject if any 2 labels are the same
			prev = s;
		}
		if( !reject )
			break;
	}
}

static unsigned int w, h;

bool collect( const char *s )
{
	static enum {START_OF_LINE,IN_VALUE1,IN_VALUE2,IN_VALUE3,BETWEEN_VALUE,EXPECT_EOL,NORMAL} state=START_OF_LINE;
	static int channel, idx;
	static char buf[256];
	static LINE line;
	bool new_data = false;
	while( *s )
	{
		char c = *s++;
		if( c=='\r' || c=='\n' )
		{
			if( state==EXPECT_EOL )
			{
				line.nbr_channels = channel;
				data.push_back( line );
				if( w > 0 )
				{
					int hsamples = HSAMPLES(w);
					while( data.size() > hsamples )
						data.pop_front();
				}
				new_data = true;
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
					if( c == '$' )
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
					else if( c == ' ' || c==';' || c=='#' )
					{
						buf[idx++] = '\0';
                        if( c == '#' && channel==0 )
                            line.time = atol( buf );
                        else
                            line.samples[channel++] = atof( buf );
						idx = 0;
                        state = (c==';'?EXPECT_EOL:BETWEEN_VALUE);
					}
					else
						state = NORMAL;
					break;
				}
				case IN_VALUE3:
				{
					if( '0'<=c && c<='9' )
						buf[idx++] = c;
					else if( c == ' ' || c==';' || c=='#' )
					{
						buf[idx++] = '\0';
                        if( c == '#' && channel==0 )
                            line.time = atol( buf );
                        else
                            line.samples[channel++] = atof( buf );
						idx = 0;
						state = (c==';'?EXPECT_EOL:BETWEEN_VALUE);
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
	return new_data;
}

#define Y(y) (  (h-h/32) - (   (double)(h-h/16) * ((double)((y)-(LO)))  / (double)((HI)-(LO))  )   )

void samples_draw(wxDC& pdc, wxWindow *canvas, wxFrame *frame )
{
    wxDC &dc = pdc ;
    canvas->PrepareDC(dc);
	wxSize sz = canvas->GetClientSize();
	w = sz.GetWidth();
	h = sz.GetHeight();
    frame->PrepareDC(dc);
    dc.Clear();

	// Draw horizontal frame
	dc.SetPen(*wxLIGHT_GREY_PEN);
    for( int i = 0; i < labels.size(); i++ )
	{
		int y = Y(values[i]);
        dc.DrawLine(50,y, w-50, y);
        wxSize sz2= dc.GetTextExtent(labels[i]);
		dc.DrawText(labels[i],50-sz2.x-2,y-sz2.y/2);
        dc.DrawText(labels[i],w-50,y-sz2.y/2);
    }

    // Vertical horizontal frame
    dc.SetPen(*wxLIGHT_GREY_PEN);
    for( int x = 50; x <= w-50; x+=50 )
    {
        int y1 = Y(LO);
        int y2 = Y(HI);
        dc.DrawLine(x,y1, x, y2);
        char buf[40];
        sprintf( buf, "%.1fms", (x-50)/50*1.6 );
        std::string label(buf);
        wxSize sz3= dc.GetTextExtent(label);
        dc.DrawText(label,x-sz3.x/2,y1);
        sprintf( buf, "%.3fm", (x-50)/50*0.2744 );
        std::string label2(buf);
        dc.DrawText(label2,x-sz3.x/2,y2-sz3.y);
    }

    // Draw channels
	int nbr_samples1 = static_cast<int>(data.size());
	for( int channel=0; channel<3; channel++ )
	{
		switch(channel)
		{
			default:
			case 0: dc.SetPen(*wxBLACK_PEN); break;
			case 1: dc.SetPen(*wxBLUE_PEN);  break;
			case 2: dc.SetPen(*wxRED_PEN);   break;
		}
        int accum = 0;
		int nbr_samples2 = HSAMPLES(w-50);
		for( int i=1; i<nbr_samples1 && i<nbr_samples2; i++ )
		{
			LINE *p = &data[i];
            LINE *q = &data[i-1];
            if( channel >= p->nbr_channels  )
				break;
            long delta = p->time - q->time;
            if( delta<1 )
            {
                delta = 1;
                p->time = q->time + delta;
            }
            else if( delta > 16 )
            {
                delta = 16;
                p->time = q->time + delta;
            }
            int x1 = HORIGIN + (accum/2)*HINTERVAL;
            accum += delta;
            int x2 = HORIGIN + (accum/2)*HINTERVAL;
			double y2 = p->samples[channel];
			if( LO<=y2 && y2<=HI )
			{
				double y1 = q->samples[channel];
				if( LO<=y1 && y1<=HI )
					dc.DrawLine(x1,Y(y1),x2,Y(y2));
			}
			if( x2 > w )
				break;
		}
	}
}


void samples_clear(wxDC& pdc, wxWindow *canvas, wxFrame *frame )
{
    data.clear();
    wxDC &dc = pdc ;
    canvas->PrepareDC(dc);
    wxSize sz = canvas->GetClientSize();
    w = sz.GetWidth();
    h = sz.GetHeight();
    frame->PrepareDC(dc);
    dc.Clear();

    // Draw frame
    dc.SetPen(*wxLIGHT_GREY_PEN);
    for( int i = 0; i < labels.size(); i++ )
    {
        int y = Y(values[i]);
        dc.DrawLine(50,y, w-50, y);
        wxSize sz2= dc.GetTextExtent(labels[i]);
        dc.DrawText(labels[i],50-sz2.x-2,y-sz2.y/2);
        dc.DrawText(labels[i],w-50,y-sz2.y/2);
    }

    // Vertical horizontal frame
    dc.SetPen(*wxLIGHT_GREY_PEN);
    for( int x = 50; x <= w-50; x+=50 )
    {
        int y1 = Y(LO);
        int y2 = Y(HI);
        dc.DrawLine(x,y1, x, y2);
        char buf[40];
        sprintf( buf, "%.1fms", (x-50)/50*0.8 );
        std::string label(buf);
        wxSize sz3= dc.GetTextExtent(label);
        dc.DrawText(label,x-sz3.x/2,y1);
        sprintf( buf, "%.3fm", (x-50)/50*0.1372 );
        std::string label2(buf);
        dc.DrawText(label2,x-sz3.x/2,y2-sz3.y);
    }
}
