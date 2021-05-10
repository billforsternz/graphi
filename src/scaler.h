#include <vector>
#include <string>

void RedirectIOToConsole();
void scaler_init();
void scaler( double lo, double hi, int nbr_lines ); //, std::vector<std::string> &labels, std::vector<double> &values );
void samples_draw(wxDC& pdc,wxWindow *canvas, wxFrame *frame );
