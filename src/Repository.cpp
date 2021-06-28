/****************************************************************************
 * Persistent storage of config information
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#include "wx/wx.h"
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "Repository.h"

Repository::Repository( bool use_defaults )
{
    SetDirectories();

    // Using wxConfig instead of writing wxFileConfig or wxRegConfig enhances
    // portability of the code
    config = new wxConfig( "Graphi", "Triple Happy", ini_filename );

    // Optionally use default values instead of reading from config
    if( !use_defaults )
    {

        // Options
        config->Read( "OptionsComPort", &options.m_com_port );
        config->Read( "OptionsYMax",    &options.m_y_max );
        config->Read( "OptionsYMin",    &options.m_y_min );
	}
}

Repository::~Repository()
{
    // Options
    config->Write( "OptionsComPort", options.m_com_port );
    config->Write( "OptionsYMax",    options.m_y_max );
    config->Write( "OptionsYMin",    options.m_y_min );

    // The changes will be written back automatically
    delete config;
}


#if 0
static void TestStandardPaths()
{
// Results of running this in debug mode during development
//=========================================================
// Config dir (sys):	C:\Documents and Settings\All Users\Application Data\Graphi
// Config dir (user):	C:\Documents and Settings\Bill\Application Data
// Data dir (sys):		c:\Documents and Settings\Bill\My Documents\Chess\wx
// Data dir (sys local):	c:\Documents and Settings\Bill\My Documents\Chess\wx
// Data dir (user):	C:\Documents and Settings\Bill\Application Data\Graphi
// Data dir (user local):	C:\Documents and Settings\Bill\Local Settings\Application Data\Graphi
// Documents dir:		C:\Documents and Settings\Bill\My Documents
// Executable path:	c:\Documents and Settings\Bill\My Documents\Chess\wx\vc_mswd\Graphi.exe
// Plugins dir:		c:\Documents and Settings\Bill\My Documents\Chess\wx
// Resources dir:		c:\Documents and Settings\Bill\My Documents\Chess\wx

    wxStandardPathsBase& stdp = wxStandardPaths::Get();
    printf( "Config dir (sys):\t%s\n", stdp.GetConfigDir().c_str());
    printf("Config dir (user):\t%s\n", stdp.GetUserConfigDir().c_str());
    printf("Data dir (sys):\t\t%s\n", stdp.GetDataDir().c_str());
    printf("Data dir (sys local):\t%s\n", stdp.GetLocalDataDir().c_str());
    printf("Data dir (user):\t%s\n", stdp.GetUserDataDir().c_str());
    printf("Data dir (user local):\t%s\n", stdp.GetUserLocalDataDir().c_str());
    printf("Documents dir:\t\t%s\n", stdp.GetDocumentsDir().c_str());
    printf("Executable path:\t%s\n", stdp.GetExecutablePath().c_str());
    printf("Plugins dir:\t\t%s\n", stdp.GetPluginsDir().c_str());
    printf("Resources dir:\t\t%s\n", stdp.GetResourcesDir().c_str());
}
#endif

// Find the directories we should use
void Repository::SetDirectories()
{
    wxStandardPathsBase& stdp = wxStandardPaths::Get();

    // Defaults to use in case of error
    wxString name = "Graphi";
#ifdef WINDOWS_FIX_LATER
    exe_dir = "C:/Program Files (x86)/Graphi";
#else
    exe_dir = ".";
#endif

    // Find directories we plan to use
    wxString doc = stdp.GetDocumentsDir();              // eg "C:\Documents and Settings\Bill\My Documents"
    wxString without_end;
    printf( "Windows document directory before: %s\n", static_cast<const char *>(doc.c_str()) );
    if( doc.EndsWith("\\",&without_end) )
        doc = without_end;
    printf( "Windows document directory after : %s\n", static_cast<const char *>(doc.c_str()) );
    wxString tmp = stdp.GetExecutablePath();            // eg "C:\Program Files\Graphi\Graphi.exe"
    wxFileName exe(tmp);     
    wxArrayString dirs = exe.GetDirs();
    exe_dir = exe.GetPath();
    if( dirs.Count() > 1 )
        name = dirs[dirs.Count()-1];
    if( name=="vc_mswd" || name=="vc_msw" || name=="vc_mswud" || name=="vc_mswu" ) // during development
        name = "Graphi";
    if( name.Len() == 0 )
        name = "Graphi";
    bool ini_exists = false;
    bool mkdir = false;
    doc_dir = doc + "/" + name;
    ini_filename = exe_dir + "/Graphi.ini";
    wxFileName ini1(ini_filename);
    if( ini1.FileExists() )
    {
        ini_exists = true;
        doc_dir = exe_dir;
    }
    else
    {
        if( !wxDirExists(doc_dir) )
        {
            if( wxMkdir(doc_dir) )
                mkdir = true;
            else
                doc_dir = doc;
        }
        ini_filename = doc_dir + "/Graphi.ini";
        wxFileName ini2(ini_filename);
        if( !mkdir && ini2.FileExists() )
            ini_exists = true;
    }

    // If .ini file doesn't exist, assume new instalation so copy data files
    //  from exe directory
    std::string s(ini_filename.c_str());
    printf( "%s file:%s\n", ini_exists?"true":"false", s.c_str() );
}
