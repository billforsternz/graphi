#ifndef SCALER_H_INCLUDED
#define SCALER_H_INCLUDED

void RedirectIOToConsole();
void scaler_init( double lo, double hi );
void samples_draw( wxDC& pdc,wxWindow *canvas, wxFrame *frame );
void samples_clear( wxDC& pdc,wxWindow *canvas, wxFrame *frame );
bool collect( const char *s );

#endif // SCALER_H_INCLUDED
