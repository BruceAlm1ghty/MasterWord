#include "App.h"
#include "Frame.h"

// WX
#include "wx/splash.h"

// Creates an instance of App and WinMain to call it
wxIMPLEMENT_APP(App);

bool App::OnInit() {
#ifdef _DEBUG
	// dump memory leaks at the end of the program
	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	// Turn On (OR) end-of-program memory check
	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
	// Set the new state for the flag
	_CrtSetDbgFlag(tmpFlag);
#endif
	if(false) {
		// allocate a console
		AllocConsole();
		// reset stdout and stdin so std::cout and std::cin will work
		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
		// remove any errors from the stream
		std::cout.clear();
		std::cin.clear();
	}

	wxImage::AddHandler(new wxPNGHandler);

	Frame *frame = new Frame();
	frame->Show(true);

	return true;
}



#ifdef _WIN32
#ifdef _DEBUG
#if defined(UNICODE) || defined(_UNICODE)
#pragma comment(lib, "wxbase31ud.lib")
#pragma comment(lib, "wxmsw31ud_core.lib")
#pragma comment(lib, "wxbase31ud_net.lib")
//#pragma comment(lib, "wxmsw31ud_gl.lib")
//#pragma comment(lib, "wxregexud.lib")
//#pragma comment(lib, "wxjpegd.lib")
#else
#endif

//#pragma comment(lib, "wxpngd.lib")
//#pragma comment(lib, "wxtiffd.lib")
//#pragma comment(lib, "wxzlibd.lib")

#else  // ! _DEBUG

#if defined(UNICODE) || defined(_UNICODE)
#pragma comment(lib, "wxbase31u.lib")
#pragma comment(lib, "wxbase31ud_net.lib")
#pragma comment(lib, "wxjpeg.lib")
#pragma comment(lib, "wxmsw31u_core.lib")
#pragma comment(lib, "wxmsw31u_gl.lib")
#pragma comment(lib, "wxregexu.lib")
#else
#endif

#pragma comment(lib, "wxpng.lib")
#pragma comment(lib, "wxtiff.lib")
#pragma comment(lib, "wxzlib.lib")

#endif

#endif // _WIN32

