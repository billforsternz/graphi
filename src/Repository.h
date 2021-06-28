/****************************************************************************
 * Persistent storage of config information
 *  Author:  Bill Forster
 *  License: MIT license. Full text of license is in associated file LICENSE
 *  Copyright 2010-2014, Bill Forster <billforsternz at gmail dot com>
 ****************************************************************************/
#ifndef REPOSITORY_H
#define REPOSITORY_H
#include "wx/wx.h"
#include "wx/utils.h"

#if defined __WXMSW__
   #include "wx/config.h"      //(to let wxWidgets choose a wxConfig class for your platform)
   #include "wx/confbase.h"    //(base config class)
   #include "wx/fileconf.h"    //(wxFileConfig class)
#else
   #ifdef __VISUALC__
     #pragma warning ( disable : 4005 )  // refinition of wxUSE_CONFIG_NATIVE is okay
   #endif
   #undef  wxUSE_CONFIG_NATIVE
   #define wxUSE_CONFIG_NATIVE 0
   #include "wx/config.h"      //(to let wxWidgets choose a wxConfig class for your platform)
   #include "wx/confbase.h"    //(base config class)
   #include "wx/fileconf.h"    //(wxFileConfig class)
   #include "wx/msw/regconf.h" //(wxRegConfig class)
   #define wxUSE_CONFIG_NATIVE 0
   #ifdef __VISUALC__
       #pragma warning ( default : 4005 )
   #endif
#endif

struct OptionsConfig
{
    int      m_com_port;
    float    m_y_max;
    float    m_y_min;
    OptionsConfig()
    {
        m_com_port = 4;
        m_y_max = 2.0;
        m_y_min = -2.0;
    }
};


class Repository
{
public:
    Repository( bool use_defaults=false );
    ~Repository();
	wxConfig *GetConfig() { return config; }
    OptionsConfig      options;

private:
    wxString exe_dir;
    wxString doc_dir;
    wxString ini_filename;
    void SetDirectories();
    wxConfig *config;
    void ReadBool( const char *key, bool &value )
    {
        int temp;
        if( config->Read( key, &temp ) )
            value = (temp?true:false);
    }
};

#endif
