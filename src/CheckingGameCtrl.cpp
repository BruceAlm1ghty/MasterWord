#include "CheckingGameCtrl.h"

#include <wx/webrequest.h>

wxDEFINE_EVENT(Checker::sm_Event, wxCommandEvent);

Checker::Checker(wxEvtHandler* p) : m_p(p), m_bChecking(false) {
	// Bind state event
	Bind(wxEVT_WEBREQUEST_STATE, [this](wxWebRequestEvent& evt) {
		int n = NOT_FOUND;
		switch (evt.GetState()) {
			// Request completed
		case wxWebRequest::State_Completed:
			m_bChecking = false;
			n = FOUND;
			m_szResult = evt.GetResponse().AsString();
			break;
		case wxWebRequest::State_Failed:
			// Request failed
			m_bChecking = false;
			// if the response is not OK the connection itself failed
			n = evt.GetResponse().IsOk() ? evt.GetResponse().GetStatus() : CONN_ERROR;
			break;
		default:
			return;
		}
		if(m_p) {
			auto p = new wxCommandEvent(sm_Event, 0);
			p->SetInt(n);
			p->SetEventObject(this);
			m_p->QueueEvent(p);
		}
	});
}

bool Checker::operator()(const std::string& szWordToCheck) {
	// a request already in progress
	if(m_bChecking)
		return false;

	//	Instances of wxWebRequest are created by using wxWebSession::CreateRequest().
	wxWebRequest request = wxWebSession::GetDefault().CreateRequest(
		this,
		"https://api.dictionaryapi.dev/api/v2/entries/en/" + szWordToCheck
	);
	if(!request.IsOk()) return false;
	m_szResult.clear();
	m_bChecking = true;
	request.Start();
	return true;
}


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

CheckingGameCtrl::CheckingGameCtrl(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name) 
	: m_Checker(this), m_nCheckPlace(-1), m_nCheck(0)
{
	Create(parent, winid, pos, size, style, name);
}

bool CheckingGameCtrl::CheckWord(const std::string& sz) {
	return m_Checker(sz);
	// a request already in progress
	if(m_nCheck < 0)
		return false;

	//	Instances of wxWebRequest are created by using wxWebSession::CreateRequest().
	wxWebRequest request = wxWebSession::GetDefault().CreateRequest(
		this,
		"https://api.dictionaryapi.dev/api/v2/entries/en/" + sz
	);
	if(!request.IsOk()) return false;
	m_nCheck = -1;
	m_szCheck = sz;
	request.Start();
	return true;
}

bool CheckingGameCtrl::Create(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name) {
	if(!base_type::Create(parent, winid, pos, size, style, name)) return false;

	Bind(wxEVT_CHAR, &CheckingGameCtrl::OnChar, this);

	Bind(Checker::sm_Event, [this](wxCommandEvent& evt) {
		switch(evt.GetInt()) {
		case Checker::FOUND:
			if(m_nCheck == -2)
				this->SetCursor(*wxStockGDI::GetCursor(wxStockGDI::CURSOR_STANDARD));
			else if(m_nCheckPlace < m_Game.Words(m_szCheck.size()).size()) {
				++m_nCheckPlace;
				this->CallAfter(&CheckingGameCtrl::CheckNext, false);
			} else
				wxMessageBox(static_cast<Checker*>(evt.GetEventObject())->Result());
			m_nCheck = 1;
			break;
		case Checker::CONN_ERROR:
			if(m_nCheck == -2)
				this->SetCursor(*wxStockGDI::GetCursor(wxStockGDI::CURSOR_STANDARD));
			else 
				wxMessageBox("Web Request Failed");
			m_nCheck = 1;
			break;
		case Checker::NOT_FOUND:
			if(m_nCheck == -2) {
				this->CallAfter(static_cast<void (CheckingGameCtrl::*)(std::size_t)>(&CheckingGameCtrl::Random), m_Game.Word().size());
				if(m_Game.Remove(m_Game.Word()))
					m_Game.SaveWords(m_szPath.c_str());
			} else {
				m_nCheck = 0;
				if(m_nCheckPlace < m_Game.Words(m_szCheck.size()).size()) {
					this->CallAfter(&CheckingGameCtrl::CheckNext, true);
				} else
					wxMessageBox(m_szCheck + " is not a word");
			}
			break;
			// our site returned some other error
		default:
			if(m_nCheck == -2)
				this->SetCursor(*wxStockGDI::GetCursor(wxStockGDI::CURSOR_STANDARD));
			m_nCheck = 1;
			wxMessageBox("Site returned error " + std::to_string(evt.GetInt()));
		}
	});
#if false
	// Bind state event
	Bind(wxEVT_WEBREQUEST_STATE, [this](wxWebRequestEvent& evt) {
		switch (evt.GetState()) {
			// Request completed
		case wxWebRequest::State_Completed:
			m_nCheck = 1;
			if(m_nCheckPlace < m_Game.Words(m_szCheck.size()).size()) {
				++m_nCheckPlace;
				this->CallAfter(&CheckingGameCtrl::CheckNext, false);
			} else
				wxMessageBox(evt.GetResponse().AsString());
			break;
		case wxWebRequest::State_Failed:
			// Request failed
			m_nCheck = 0;
			// if the response is not OK the connection itself failed
			if(evt.GetResponse().IsOk()) {
				if(evt.GetResponse().GetStatus() == 404) {
					// our site returned 404 Not Found, it's not a word
					if(m_nCheckPlace < m_Game.Words(m_szCheck.size()).size()) {
						//				if(wxYES == wxMessageBox("Remove " + m_Game.Words(m_nCheckLen).at(m_nCheckPlace), "Remove", wxYES_NO))
						this->CallAfter(&CheckingGameCtrl::CheckNext, true);
					} else
						wxMessageBox(m_szCheck + " is not a word");
				} else
					// our site returned some other error
					wxMessageBox("Site returned error " + std::to_string(evt.GetResponse().GetStatus()));
			} else
				// the connection failed
				wxMessageBox("Web Request Failed");
			break;
		}
	});
#endif
	return true;
}

void CheckingGameCtrl::CheckDictionary(std::size_t n) {
	m_nCheckPlace = 0;
	auto& w = m_Game.Words(n);
	if(w.size()) m_szCheck = w.front();
	CheckNext(false);
}

void CheckingGameCtrl::CheckNext(bool bErase) {
	auto& w = m_Game.Words(m_szCheck.size());
	if(m_nCheckPlace < w.size()) {
//		std::string sz = w.at(m_nCheckPlace);
//		if(bErase && sz != m_szCheck) wxMessageBox(sz + " " + m_szCheck);
		if(bErase)
			if(m_Game.Remove(w.at(m_nCheckPlace))) {
				wxBusyCursor bc;
				m_Game.SaveWords(m_szPath.c_str());
			}
		if(m_nCheckPlace < w.size()) {
			auto& sz = w.at(m_nCheckPlace);
			CheckWord(sz);
			wxWindow* pW = this->GetParent();
			wxFrame* pF;
			while(pW && (pF = dynamic_cast<wxFrame*>(pW)) == nullptr)
				pW = pW->GetParent();
			if(pF) pF->SetStatusText(sz, 1);
		}
	}
}

void CheckingGameCtrl::Random(std::size_t nLen, std::size_t nMaxGuesses) {
	m_nCheckPlace = -1;
	base_type::Random(nLen, nMaxGuesses);
	CheckWord(m_Game.Word());
	m_nCheck = -2;
	this->SetCursor(*wxStockGDI::GetCursor(wxStockGDI::CURSOR_HOURGLASS));
}

bool CheckingGameCtrl::Init(const std::string& sz) {
	return base_type::Init(sz);
}


void CheckingGameCtrl::OnChar(wxKeyEvent& e) {
	if(m_nCheck != -2) base_type::OnChar(e);
}
