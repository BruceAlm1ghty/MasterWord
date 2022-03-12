#pragma once

#include "Game.h"
#include "App.h"

//! a WORD (puzz)LE control
struct GameCtrl : public wxWindow {
	using this_type = GameCtrl;
	using base_type = wxWindow;

	//! our colors, first the letter backgrounds, then text and box border
	enum { CLR_BG, CLR_NOT, CLR_BELONG, CLR_RIGHT, CLR_TEXT, CLR_BRDR, CLR_MAX };

	//! our default color set
	static const wxColour sm_DefColors[CLR_MAX];

	GameCtrl();
	GameCtrl(wxWindow *parent,
		wxWindowID winid = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxString& name = wxASCII_STR(wxPanelNameStr)) : GameCtrl() 
	{
		Create(parent, winid, pos, size, style, name);
	}

	bool Create(wxWindow *parent,
		wxWindowID winid = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxString& name = wxASCII_STR(wxPanelNameStr));

	bool LoadWords();

	const wxColour& Color(std::uint16_t n) const { return m_Colors[n < CLR_MAX ? n : 0]; }

	void Random(std::size_t nLen = 5) { Random(nLen, m_nMaxGuesses); }
	virtual void Random(std::size_t nLen, std::size_t nMaxGuesses);
	bool Init(const std::string& sz);

	std::size_t Length() const { return m_Game.Length(); }

	//! return the maximum word length
	std::size_t Max() const;

	//! change the maximum guesses of the current game
	void MaxGuesses(std::size_t n);

	const Game& Get() const { return m_Game; }

protected:
	//! our game
	Game m_Game;
	//! our font
	wxFont* m_pFont;
	//! our colors
	wxColour m_Colors[CLR_MAX];
	//! the maximum number of guesses
	std::size_t m_nMaxGuesses;
	//! the current guess
	std::string m_szGuess;
	//! path to the word file
	std::string m_szPath;

	//! we will draw to a bitmap to stop flicker
	wxBitmap m_bitmap;
	//! track if our bitmap needs updating
	bool m_bUpdate;

	void Update() { m_bUpdate = true; Refresh(); }

	void OnPaint(wxPaintEvent& event);
	void OnChar(wxKeyEvent&);
	void Draw(wxDC&);

	void OnWin();
	void OnLose();
	void OnDone(const std::string& szCap, const std::string& szMesg = std::string());
};
