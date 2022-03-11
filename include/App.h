#pragma once

#ifdef _WIN32
#define wxNEEDS_DECL_BEFORE_TEMPLATE
#ifndef __WXMSW__
#define __WXMSW__
#endif
#ifndef WXUSINGDLL
#define WXUSINGDLL
#endif
#endif

#include <wx/wx.h>

class App : public wxApp {
public:
	typedef App this_type;
	typedef wxApp base_type;

public:
	virtual bool OnInit() override;
};

