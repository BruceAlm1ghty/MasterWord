#include <wx/wx.h>

class Frame : public wxFrame {
public:
	typedef Frame this_type;
	typedef wxFrame base_type;

	enum { ID_NEW, ID_LEN, ID_MAX, ID_WORD };
public:
	Frame();

protected:
	std::size_t m_nMax = 6, m_nLen = 5;
};

