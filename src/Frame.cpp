#include "App.h"
#include "Frame.h"
#include "CheckingGameCtrl.h"

#include <wx/spinctrl.h>
#include <wx/notebook.h>
#include <wx/tglbtn.h>
#include <wx/dataview.h>
#include <wx/numdlg.h> 

Frame::Frame() : wxFrame(NULL, wxID_ANY, "Master Word") {
    wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_NEW, "&Random\tCtrl-R", "Start a New Random Word Game");
	menuFile->Append(ID_LEN, "Word &Length...\tCtrl-L", "Change the Word Length");
	menuFile->Append(ID_MAX, "Max &Guesses...\tCtrl-G", "Change the Maximum Guesses");
	menuFile->Append(ID_WORD, "&Word...\tCtrl-W", "Create a Game with the given Word");
	menuFile->Append(ID_CHECK, "&Check a word...\tCtrl-C", "Check a given Word");
	menuFile->Append(ID_DIC, "&Check a dictionary...\tCtrl-D", "Check a Dictionary");

//    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
 //                    "Help string shown in status bar for this menu item");
 //   menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&Game");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar( menuBar );
    CreateStatusBar(3);
    SetStatusText("Ready");

	wxBoxSizer* p = new wxBoxSizer(wxVERTICAL);
	auto pW = new CheckingGameCtrl(this);
	p->Add(pW, 1, wxEXPAND);
	SetSizerAndFit(p);
	Bind(wxEVT_MENU, [this](wxCommandEvent& event) { 
		
	}, wxID_EXIT);
	Bind(wxEVT_MENU, [this,pW](wxCommandEvent& event) { 
		switch(event.GetId()) {
		case ID_LEN:
			{
				long l = wxGetNumberFromUser(wxEmptyString, "New Word Length", "Change Word Length", m_nLen, 1, pW->Max(), this);
				if(l < 0 || l == m_nLen) break;
				m_nLen = l;
			}
			// fall through
		case ID_NEW:
			pW->Random(m_nLen, m_nMax);
			break;
		case ID_MAX:
			{
				long l = wxGetNumberFromUser(wxEmptyString, "Max Guesses", "Maximum Number of Guesses", m_nMax, 1, 99, this);
				if(l > 0) pW->MaxGuesses(m_nMax = l);
			}
			break;
		case ID_WORD:
			{
				auto wx = wxGetPasswordFromUser("Enter Word", "Enter the Word for this Game");
				if(!wx.empty()) 
					if(!pW->Init(wx.ToStdString()))
						wxMessageBox("The supplied word is not in the dictionary", "Not a Valid Word");
			}
			break;
		case wxID_EXIT:
			this->Close(true);
			break;
		case ID_CHECK:
		{
//			pW->CheckDictionary();
			auto wx = wxGetTextFromUser("Enter Word", "Enter word to check");
			if(!wx.empty())
				pW->CheckWord(wx.ToStdString());
		}
		break;
		case ID_DIC:
		{
			long l = wxGetNumberFromUser(wxEmptyString, "Max Guesses", "Maximum Number of Guesses", 5, 1, 99, this);
			if(l > 0) pW->CheckDictionary(l);
		}
		break;
		}
	});
}
