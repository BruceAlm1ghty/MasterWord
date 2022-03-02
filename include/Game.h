#pragma once

#include <random>
#include <map>

//! a WORD puzzLE game
struct Game {
	using this_type = Game;
	//! words keyed by length
	using map_type = std::map<std::size_t, std::vector<std::string>>;
	//! the state of every letter
	using state_type = std::uint8_t;
	//! a guessed word and its scoring
	using guess_type = std::pair<std::string, std::vector<state_type>>;

	//! each letter is
	//! UNUSED -- not yet tried/used
	//! NOT -- not in the word
	//! BELONG -- in the word but in the wrong location
	//! RIGHT -- in its correct location
	enum { UNUSED, NOT, BELONG, RIGHT };

	//! the result from a guess
	enum result_type { NONE, WIN, NOT_WORD, WRONG_LEN };

	static const char* sm_pszDic;

	//! load words from a file
	bool LoadWords(const char* pszFile);
	//! load words from a stream
	bool LoadWords(std::istream&);

	//! initialize a game
	bool Random(std::size_t nLen = 5);
	//! initialize a new game using the given word
	bool Init(const std::string& sz) { return Init(sz, true); }

	//! evaluate a guess
	//! return true if it's a win
	result_type Guess(std::string sz);

	static std::default_random_engine sm_Rand;

	std::size_t Length() const { return m_szWord.length(); }
	const state_type (&State() const)[26] { return  m_vState; }
	const std::vector<guess_type>& Guesses() const { return m_vGuesses; }

	const map_type& Words() const { return m_mapWords; }
	const std::vector<std::string>& Words(std::size_t n) const;

	const std::string& Word() const { return m_szWord; }

	//! remove the word from the dictionary
	bool Remove(std::string sz);

	//! save the words
	bool SaveWords(const char* p) const;

protected:
	//! the state of every letter
	state_type m_vState[26];
	//! the vector of guesses
	std::vector<guess_type> m_vGuesses;
	//! my current word
	std::string m_szWord;
	//! the map of words keyed by length
	map_type m_mapWords;

	bool Init(const std::string& sz, bool bCheck);
};
