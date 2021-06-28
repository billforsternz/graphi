/****************************************************************************
 * Options for Graphi
 ****************************************************************************/
#ifndef OPTIONS_DIALOG_H
#define OPTIONS_DIALOG_H
#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/filepicker.h"
#include "Repository.h"

// Control identifiers
enum
{
    ID_OPTIONS_DIALOG        = 10000,
    ID_OPTIONS_RESET         = 10001,
    ID_ENABLED               = 10002,
    ID_SUGGEST               = 10003,
    ID_OPTIONS_PICKER        = 10004,
    ID_LIMIT_MOVES           = 10005,
    ID_POST_LIMIT_PERCENT    = 10006,
    ID_COM_PORT,
    ID_Y_MAX,
    ID_Y_MIN
};

// OptionsDialog class declaration
class OptionsDialog: public wxDialog
{    
    DECLARE_CLASS( OptionsDialog )
    DECLARE_EVENT_TABLE()

public:

    // Constructors
    OptionsDialog( );
    OptionsDialog(
      const OptionsConfig &dat,
      wxWindow* parent,
      wxWindowID id = ID_OPTIONS_DIALOG,
      const wxString& caption = wxT("User options"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Member initialisation
    void Init();

    // Creation
    bool Create( wxWindow* parent,
      wxWindowID id = ID_OPTIONS_DIALOG,
      const wxString& caption = wxT("User options"),
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX );

    // Creates the controls and sizers
    void CreateControls();

    // Sets the validators for the dialog controls
    void SetDialogValidators();

    // Sets the help text for the dialog controls
    void SetDialogHelp();

    // OptionsDialog event handler declarations

    // wxEVT_UPDATE_UI event handler for ID_???
    //void On???Update( wxUpdateUIEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RESET
    void OnResetClick( wxCommandEvent& event );

    // wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
    void OnHelpClick( wxCommandEvent& event );

    // OptionsDialog member variables

    // Data members
    OptionsConfig   dat;
};

#endif    // OPTIONS_DIALOG_H
