// Wordle.cpp : Defines the entry point for the application.
//

#include "App.h"
#include "GameCtrl.h"
#include "wx/busyinfo.h"

#include <filesystem>
#include <fstream>
#include <mutex>

#include "wx/filedlg.h"
#include <wx/filename.h>
#include <wx/stdpaths.h>

namespace fs = std::experimental::filesystem;

//////////////////////////////////////////////////////////////////////////
// GameCtrl

// 	enum { CLR_BG, CLR_NOT, CLR_BELONG, CLR_RIGHT, CLR_TEXT, CLR_BRDR, CLR_MAX };
// 	enum { 
const wxColour GameCtrl::sm_DefColors[CLR_MAX] {
	wxColour(255,255,255), // CLR_BG
	wxColour(128+32+16,128+32+16,128+32+16), // CLR_NOT
	wxColour(0xd4,0xaf,0x37), // CLR_BELONG
	wxColour(0,255,0), // CLR_RIGHT
	wxColour(0,0,0), // CLR_TEXT
	wxColour(0,0,0) // CLR_BRDR
};

void GameCtrl::Random(std::size_t nLen, std::size_t nMaxGuesses) {
	if(m_Game.Random(nLen)) {
		m_nMaxGuesses = nMaxGuesses;
		m_szGuess.clear();
		Update();
	} else
		wxMessageBox("Could not initialize game with word length " + std::to_string(nLen));
}

bool GameCtrl::Init(const std::string& sz) {
	m_Game.Init(sz);
	m_szGuess.clear();
	Update();
	return true;
}

bool GameCtrl::LoadWords() {
	auto sz = wxFileSelectorEx("Select Word File", wxEmptyString, wxEmptyString, nullptr, "Text Files (*.txt)|*.txt|All Files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	return sz.empty() ? false : m_Game.LoadWords((m_szPath = sz.ToStdString()).c_str());
}

GameCtrl::GameCtrl() : m_pFont(nullptr), m_nMaxGuesses(6) {
	std::copy(sm_DefColors, sm_DefColors + CLR_MAX, m_Colors);
}

bool GameCtrl::Create(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name) {
	if(!base_type::Create(parent, winid, pos, size, style | wxBORDER_NONE | wxWANTS_CHARS, name)) return false;

	m_pFont = wxTheFontList->FindOrCreateFont(24, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	//need to include <wx/filename.h> and <wx/stdpaths.h>

	wxFileName f(wxStandardPaths::Get().GetExecutablePath());
	fs::path pth = f.GetPath().ToStdString();
	{
		wxBusyInfo b("Loading Dictionary");
		// first the current directory
		if(!m_Game.LoadWords((m_szPath = Game::sm_pszDic).c_str()))
		// the program directory
			if(!m_Game.LoadWords((m_szPath = (pth /= Game::sm_pszDic).string()).c_str()))
				// now ask
				LoadWords();
	}

	SetMinClientSize( { 350, 400 });
	Random();

	Bind(wxEVT_PAINT, &GameCtrl::OnPaint, this);
	Bind(wxEVT_CHAR, &GameCtrl::OnChar, this);
	Bind(wxEVT_SIZE, [this](wxSizeEvent& e) { Update(); e.Skip(); });
	return true;
}

void GameCtrl::OnDone(const std::string& szCap, const std::string& szMesg) {
	wxMessageDialog dlg(this, szMesg + "New Game?", szCap, wxYES_NO | wxCANCEL);
	// we're changing the labels since pressing escape is the same as cancel
	dlg.SetYesNoCancelLabels("Yes", "Remove", "No");
	auto n = dlg.ShowModal();
	if(wxID_NO == n) {
		// remove the word
		if(m_Game.Remove(m_Game.Word())) {
			wxBusyCursor bc;
			m_Game.SaveWords(m_szPath.c_str());
		}
	}
	if(wxID_CANCEL != n)
		// yes or remove causes a new game (if you removed the word, you won't be able to guess it)
		Random();
}

void GameCtrl::OnWin() {
	OnDone("You Win!");
}

void GameCtrl::OnLose() {
	OnDone("You Suck!", "The Word was " + m_Game.Word() + "\n");
}

void GameCtrl::OnChar(wxKeyEvent& e) {
	wxChar uc = e.GetUnicodeKey();
	if(uc != WXK_NONE) {
		auto nLen = Length();
		if((uc >= 'A' && uc <= 'Z') || (uc >= 'a' && uc <= 'z')) {
			// regular key
			if(m_szGuess.size() < nLen) {
				m_szGuess.push_back(uc & ~32);
				Update();
			}
		} else if(uc == WXK_RETURN) {
			if(m_szGuess.size() == nLen && m_Game.Guesses().size() < m_nMaxGuesses)
				switch(m_Game.Guess(m_szGuess)) {
				case Game::WIN:
					this->CallAfter(&GameCtrl::OnWin);
					m_szGuess.clear();
					Update();
					break;
				case Game::NONE:
					// a valid but non-winnning guess
					if(m_Game.Guesses().size() == m_nMaxGuesses)
						this->CallAfter(&GameCtrl::OnLose);
					m_szGuess.clear();
					Update(); 
					break;
				case Game::NOT_WORD:
					wxMessageBox("Dumbass", "Not a word");
				}
		} else if(uc == WXK_BACK) {
			if(m_szGuess.size()) {
				m_szGuess.pop_back();
				Update();
			}
		} else e.Skip();
	} else e.Skip();
}

void GameCtrl::OnPaint(wxPaintEvent& event) {
	wxPaintDC dc(this);
	if(!m_bitmap.Ok() || m_bitmap.GetSize() != dc.GetSize()) {
		m_bUpdate = true;
		m_bitmap.Create(dc.GetSize());
	}
	if(m_bUpdate) {
		m_bUpdate = false;
		wxMemoryDC dc(m_bitmap);
		Draw(dc);
	}
	dc.DrawBitmap(m_bitmap, 0, 0);
}

void GameCtrl::Draw(wxDC& dc) {
	if(m_pFont)
		dc.SetFont(*m_pFont);
	
//	dc.SetBackground(wxBrush(m_Colors[CLR_BG]));
	dc.SetBackground(wxBrush(this->GetBackgroundColour()));
	dc.Clear();
	dc.SetTextForeground(m_Colors[CLR_TEXT]);

	auto r = GetClientRect();
	wxSize ext = dc.GetTextExtent("H");
	ext.IncBy(10,0);
	int dy = ext.GetHeight() + 5;
	int dx = ext.GetWidth() + 5;

	auto nLen = Length();
	// width of the game board
	int dWidth = dx * (nLen - 1) + ext.GetWidth();
	int nOffset = (dc.GetSize().GetWidth() - dWidth) / 2;
	auto& vGuesses = m_Game.Guesses();

	int x = r.GetLeft(), y = r.GetTop() + 4;
	dc.SetPen(wxPen(m_Colors[CLR_BRDR]));
	for(std::size_t i = 0; i < m_nMaxGuesses; ++i, y += dy) {
		// draw the boxes
		for(std::size_t j = 0; j < nLen; ++j) {
			// set the brush color by the letter state
			dc.SetBrush(wxBrush(i < vGuesses.size() ? Color(vGuesses[i].second[j]) : m_Colors[CLR_BG]));
			dc.DrawRectangle(x + j * dx + nOffset, y, ext.GetWidth(), ext.GetHeight());
			if(i < vGuesses.size())
				dc.DrawText(vGuesses[i].first[j], x + j * dx + 3 + nOffset, y);
			else if(i == vGuesses.size() && j < m_szGuess.size())
				dc.DrawText(m_szGuess[j], x + j * dx + 3 + nOffset, y);
		}
	}

	const static char* psz[] { "QWERTYUIOP", "ASDFGHJKL","ZXCVBNM", nullptr };
	y += 12;
	nOffset = (dc.GetSize().GetWidth() - (dx * (strlen(psz[0]) - 1) + ext.GetWidth())) / 2;

	auto& vState = m_Game.State();
	for(std::size_t i = 0; psz[i]; ++i, y += dy)
		for(std::size_t j = 0, je = strlen(psz[i]); j < je; ++j) {
			dc.SetBrush(wxBrush(m_Colors[vState[psz[i][j] - 'A']]));
			dc.DrawRectangle(x + j * dx + i * dx / 2 + nOffset, y, ext.GetWidth(), ext.GetHeight());
			dc.DrawText(psz[i][j], x + j * dx + 3 + i * dx / 2 + nOffset, y);
		}
}

std::size_t GameCtrl::Max() const {
	auto x = m_Game.Words();
	return x.size() ? std::prev(x.end())->first : 0;
}

void GameCtrl::MaxGuesses(std::size_t n) {
	m_nMaxGuesses = n;
	Update();
}
