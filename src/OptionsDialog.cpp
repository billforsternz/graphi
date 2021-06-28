/****************************************************************************
* Options for Graphi
****************************************************************************/
#include "wx/wx.h"
#include "wx/valtext.h"
#include "wx/valgen.h"
#include "wx/valnum.h"
#include "OptionsDialog.h"

// OptionsDialog type definition
IMPLEMENT_CLASS( OptionsDialog, wxDialog )

// OptionsDialog event table definition
BEGIN_EVENT_TABLE( OptionsDialog, wxDialog )
    EVT_BUTTON( ID_OPTIONS_RESET, OptionsDialog::OnResetClick )
    EVT_BUTTON( wxID_HELP, OptionsDialog::OnHelpClick )
END_EVENT_TABLE()

// OptionsDialog constructors
OptionsDialog::OptionsDialog()
{
    Init();
}

OptionsDialog::OptionsDialog(
  const OptionsConfig &dat,
  wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    this->dat = dat;
    Create(parent, id, caption, pos, size, style);
}

// Initialisation
void OptionsDialog::Init()
{
}

// Dialog create
bool OptionsDialog::Create( wxWindow* parent,
  wxWindowID id, const wxString& caption,
  const wxPoint& pos, const wxSize& size, long style )
{
    bool okay=true;

    // We have to set extra styles before creating the dialog
    SetExtraStyle( wxWS_EX_BLOCK_EVENTS/*|wxDIALOG_EX_CONTEXTHELP*/ );
    if( !wxDialog::Create( parent, id, caption, pos, size, style ) )
        okay = false;
    else
    {

        CreateControls();
        SetDialogHelp();
        SetDialogValidators();

        // This fits the dialog to the minimum size dictated by the sizers
        GetSizer()->Fit(this);
        
        // This ensures that the dialog cannot be sized smaller than the minimum size
        GetSizer()->SetSizeHints(this);

        // Centre the dialog on the parent or (if none) screen
        Centre();
    }
    return okay;
}

#define SMALL 5
#define LARGE 2

// Control creation for OptionsDialog
void OptionsDialog::CreateControls()
{    

    // A top-level sizer
    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(top_sizer);
    
    // A second box sizer to give more space around the controls
    wxBoxSizer* box_sizer = new wxBoxSizer(wxVERTICAL);
    top_sizer->Add(box_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // A friendly message
    wxStaticText* descr = new wxStaticText( this, wxID_STATIC,
        wxT("This panel lets you setup all configurable options."), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(descr, 0, wxALIGN_LEFT|wxALL, 5);

    // Spacer
    box_sizer->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // Label for the COM Port
    wxStaticText* com_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Select COM port number, 1=COM1 etc"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(com_label, 0, wxALL, 5);

    // A spin control for the COM Port
    wxSpinCtrl* com_spin = new wxSpinCtrl ( this, ID_COM_PORT,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 1, 20, 1 );
    box_sizer->Add(com_spin, 0, wxALL, 5);

    // Label for MAX_Y
    wxStaticText* max_y_label = new wxStaticText ( this, wxID_STATIC,
        wxT("MAX Y "), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(max_y_label, 0, wxALIGN_LEFT|wxALL, SMALL);

    // Text control MAX_Y
    wxTextCtrl *max_y_txt_ctrl = new wxTextCtrl ( this, ID_Y_MAX,  wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(max_y_txt_ctrl, 0, wxGROW|wxALL, SMALL);

    // Label for MIN_Y
    wxStaticText* min_y_label = new wxStaticText ( this, wxID_STATIC,
        wxT("MIN Y "), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(min_y_label, 0, wxALIGN_LEFT|wxALL, SMALL);

    // Text control MIN_Y
    wxTextCtrl *min_y_txt_ctrl = new wxTextCtrl ( this, ID_Y_MIN,  wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(min_y_txt_ctrl, 0, wxGROW|wxALL, SMALL);

#if 0
    // Label for file
    wxStaticText* file_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&File "), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(file_label, 0, wxALIGN_LEFT|wxALL, 5);

    // File picker control
    wxString path = dat.m_file;
    wxFilePickerCtrl *picker = new wxFilePickerCtrl( this, ID_OPTIONS_PICKER, path, wxT("Select .pgn file for options"),
        "*.pgn", wxDefaultPosition, wxDefaultSize, 
        wxFLP_USE_TEXTCTRL|wxFLP_OPEN|wxFLP_FILE_MUST_EXIST );    
    box_sizer->Add(picker, 1, wxALIGN_LEFT|wxEXPAND|wxALL, 5);

    // File enabled
    wxCheckBox* enabled_box = new wxCheckBox( this, ID_ENABLED,
       wxT("&Enabled"), wxDefaultPosition, wxDefaultSize, 0 );
    enabled_box->SetValue( dat.m_enabled );
    box_sizer->Add( enabled_box, 0,
        wxALL, 5);

    // Label for the moves limit
    wxStaticText* limit_moves_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&If available always play options moves up until move:"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(limit_moves_label, 0, wxALL, 5);

    // A spin control for the moves limit
    wxSpinCtrl* limit_moves_spin = new wxSpinCtrl ( this, ID_LIMIT_MOVES,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 120, 25 );
    box_sizer->Add(limit_moves_spin, 0, wxALL, 5);

    // Label for the post limit percentage
    wxStaticText* post_limit_percent_label = new wxStaticText ( this, wxID_STATIC,
        wxT("&Then if available play options moves this percentage of the time:"), wxDefaultPosition, wxDefaultSize, 0 );
    box_sizer->Add(post_limit_percent_label, 0, wxALL, 5);

    // A spin control for the post limit percentage
    wxSpinCtrl* post_limit_percent_spin = new wxSpinCtrl ( this, ID_POST_LIMIT_PERCENT,
        wxEmptyString, wxDefaultPosition, wxSize(60, -1),
        wxSP_ARROW_KEYS, 0, 100, 50 );
    box_sizer->Add(post_limit_percent_spin, 0, wxALL, 5);

    // Suggest options moves
    wxCheckBox* suggest_box = new wxCheckBox( this, ID_SUGGEST,
       wxT("&Suggest options moves"), wxDefaultPosition, wxDefaultSize, 0 );
    suggest_box->SetValue( dat.m_suggest );
    box_sizer->Add( suggest_box, 0,
        wxALL, 5);
#endif

    // A dividing line before the OK and Cancel buttons
    wxStaticLine* line = new wxStaticLine ( this, wxID_STATIC,
        wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    box_sizer->Add(line, 0, wxGROW|wxALL, 5);

    // A horizontal box sizer to contain Reset, OK, Cancel and Help
    wxBoxSizer* okCancelBox = new wxBoxSizer(wxHORIZONTAL);
    box_sizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15);
/*
    // The Reset button
    wxButton* reset = new wxButton( this, ID_OPTIONS_RESET, wxT("&Reset"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(reset, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
 */
    // The OK button
    wxButton* ok = new wxButton ( this, wxID_OK, wxT("&OK"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Cancel button
    wxButton* cancel = new wxButton ( this, wxID_CANCEL,
        wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // The Help button
    wxButton* help = new wxButton( this, wxID_HELP, wxT("&Help"),
        wxDefaultPosition, wxDefaultSize, 0 );
    okCancelBox->Add(help, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

// Set the validators for the dialog controls
void OptionsDialog::SetDialogValidators()
{
    FindWindow(ID_COM_PORT)->SetValidator(
        wxGenericValidator(& dat.m_com_port));
    FindWindow(ID_Y_MAX)->SetValidator(
        wxFloatingPointValidator<float>(& dat.m_y_max));
    FindWindow(ID_Y_MIN)->SetValidator(
        wxFloatingPointValidator<float>(& dat.m_y_min));
}

// Sets the help text for the dialog controls
void OptionsDialog::SetDialogHelp()
{
    wxString com_port_help  = wxT("Select COM port number, eg 4 for COM4");
    wxString y_max_help     = wxT("Set the maximum value on the Y axis");
    wxString y_min_help     = wxT("Set the minimum value on the Y axis");

    FindWindow(ID_COM_PORT)->SetHelpText(com_port_help);
    FindWindow(ID_Y_MAX)->SetToolTip(y_max_help);
    FindWindow(ID_Y_MIN)->SetHelpText(y_min_help);
}

/*
// wxEVT_UPDATE_UI event handler for ID_CHECKBOX
void OptionsDialog::OnVoteUpdate( wxUpdateUIEvent& event )
{
    wxSpinCtrl* ageCtrl = (wxSpinCtrl*) FindWindow(ID_AGE);
    if (ageCtrl->GetValue() < 18)
    {
        event.Enable(false);
        event.Check(false);
    }
    else
        event.Enable(true);
}
*/

// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_OPTIONS_RESET
void OptionsDialog::OnResetClick( wxCommandEvent& WXUNUSED(event) )
{
    Init();
    TransferDataToWindow();
}

// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
void OptionsDialog::OnHelpClick( wxCommandEvent& WXUNUSED(event) )
{
    // Normally we would wish to display proper online help.
    // For this example, we're just using a message box.
    /*
    wxGetApp().GetHelpController().DisplaySection(wxT("Personal record dialog"));
     */

    wxString helpText =
      wxT("Use this panel to set the COM port and the max/min Y axis values\n");

    wxMessageBox(helpText,
      wxT("Options Dialog Help"),
      wxOK|wxICON_INFORMATION, this);
}

