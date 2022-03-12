#pragma once

#include "GameCtrl.h"

struct Checker : public wxEvtHandler {
	enum { FOUND = 0, NOT_FOUND = 404, CONN_ERROR = -1 };

	static const wxEventTypeTag<wxCommandEvent> sm_Event;

	bool operator()(const std::string& szWordToCheck);

	Checker(wxEvtHandler* p);

	bool IsChecking() const { return m_bChecking; }
	const std::string& Result() const { return m_szResult; }

protected:
	wxEvtHandler* m_p;
	bool m_bChecking;
	std::string m_szResult;
};

//! adds checking a word by request to the GameCtrl
struct CheckingGameCtrl : public GameCtrl {
	using base_type = GameCtrl;
	using this_type = CheckingGameCtrl;

	CheckingGameCtrl(wxWindow *parent,
		wxWindowID winid = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxString& name = wxASCII_STR(wxPanelNameStr));

	bool Create(wxWindow *parent,
		wxWindowID winid = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxString& name = wxASCII_STR(wxPanelNameStr));

	bool CheckWord(const std::string& sz);
	void CheckDictionary(std::size_t nLen = 5);

	void Random(std::size_t nLen = 5) { Random(nLen, m_nMaxGuesses); }
	virtual void Random(std::size_t nLen, std::size_t nMaxGuesses) override;
	bool Init(const std::string& sz);

protected:
	Checker m_Checker;
	void CheckNext(bool bErase);
	std::size_t m_nCheckPlace;
	std::string m_szCheck;
	int m_nCheck;

	void OnChar(wxKeyEvent&);
};

