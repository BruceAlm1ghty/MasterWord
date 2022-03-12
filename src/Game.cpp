// Game.cpp : Defines the entry point for the application.
//

#include "Game.h"

#include <fstream>
#include <random>
#include <algorithm>

#ifdef __linux__
#include <experimental/filesystem>
#else
#include <filesystem>
#endif
namespace fs = std::experimental::filesystem;

// Seed with a real random value, if available
std::random_device r;

//////////////////////////////////////////////////////////////////////////
// Game
std::default_random_engine Game::sm_Rand(r());

const char* Game::sm_pszDic = "MWdic.txt";

bool Game::LoadWords(const char* pszFile) {
	std::ifstream f(pszFile);
	return f.is_open() ? LoadWords(f) : false;
}

bool Game::LoadWords(std::istream& f) {
	map_type vW;
	std::string sz;
	while(std::getline(f, sz)) {
		// trim leading and trailing white space
		int nB = sz.find_first_not_of(" \t"), nE = sz.find_last_not_of(" \t");
		int nL = nE - nB + 1;
		if(nL > 0) {
			vW[nL].push_back(sz);
			// convert to upper case
			for(auto& x : vW[nL].back())
				x &= ~32;
		}
	}
	if(vW.size()) {
		m_mapWords = std::move(vW);
		return true;
	}
	return false;
}

bool Game::Random(std::size_t nLen) {
	auto i = m_mapWords.find(nLen);
	if(i == m_mapWords.end()) return false;
	std::uniform_int_distribution<int> uniform_dist(1, i->second.size());
	return Init(i->second[uniform_dist(sm_Rand) - 1], false);
}

bool Game::Init(std::string sz) { 
	for(auto& x : sz) x &= ~32;
	return Init(sz, true); 
}

bool Game::Init(const std::string& sz, bool bCheck) {
	if(bCheck) {
		auto& v = Words(sz.length());
		if(v.end() == std::find(v.begin(), v.end(), sz))
			return false;
	}
	m_vGuesses.clear();
	std::fill_n(m_vState, 26, UNUSED);
	m_szWord = sz;
	return true;
}

Game::result_type Game::Guess(std::string sz) {
	auto nLen = Length();
	if(sz.length() != nLen) return WRONG_LEN;
	// get the list of words
	auto& v = Words(nLen);
	// convert to upper case
	for(auto& x : sz) x &= ~32;
	// find it
	auto iW = std::find(v.begin(), v.end(), sz);
	if(iW == v.end()) return NOT_WORD;
	// our results vector for this guess
	std::vector<std::uint8_t> ev(nLen, NOT);
	// track the letters we've used
	std::uint32_t vUsed = 0;
	// first pass check for correct
	std::size_t nRight = 0;
	for(std::size_t j = 0, msk = 1; j < nLen; ++j, msk <<= 1)
		if(sz[j] == m_szWord[j]) {
			++nRight;
			ev[j] = RIGHT;
			// flag it used
			vUsed |= msk;
			// update the state vector
			m_vState[sz[j] - 'A'] = RIGHT;
		}
	// now check for belong
	for(std::size_t j = 0; j < nLen; ++j)
		if(ev[j] != RIGHT)
			// check the current guess letter against every available letter in the word
			for(std::size_t k = 0, msk = 1; k < nLen; ++k, msk <<= 1)
				if(!(vUsed & msk) && sz[j] == m_szWord[k]) {
					ev[j] = BELONG;
					// flag it used
					vUsed |= msk;
					// update the state vector
					auto& s = m_vState[sz[j] - 'A'];
					// don't downgrade from RIGHT
					if(s == UNUSED) s = BELONG;
				}

	// update any UNUSED to NOT in the state vector
	for(std::size_t j = 0; j < nLen; ++j)
		// we can't just use ev[j] == NOT here
		// consider the case where the player enters a double letter, e.g. 'JELLY'
		// but only one 'L' is in the word, the second 'L' will show up as 'NOT'
		// but the 'L' is not NOT
		if(ev[j] == NOT && m_vState[sz[j] - 'A'] == UNUSED) 
			m_vState[sz[j] - 'A'] = NOT;
	// add it to our guesses vector
	m_vGuesses.emplace_back(sz, ev);
	return nLen == nRight ? WIN : NONE;
}


const std::vector<std::string>& Game::Words(std::size_t n) const {
	static const std::vector<std::string> x;
	auto i = m_mapWords.find(n);
	return i == m_mapWords.end() ? x : i->second;
}


bool Game::Remove(std::string sz) {
	auto iW = m_mapWords.find(sz.length());
	if(iW != m_mapWords.end()) {
		// convert to upper
		for(auto& x : sz) x &= ~32;
		auto i = std::find(iW->second.begin(), iW->second.end(), sz);
		if(i != iW->second.end()) {
			iW->second.erase(i);
			return true;
		}
	}
	return false;
}

//! save the words
bool Game::SaveWords(const char* p) const {
	std::ofstream f(p);
	if(f.is_open()) {
		for(auto& x : m_mapWords)
			for(auto& y : x.second)
				f << y << '\n';
		return f.good();
	}
	return false;
}
