
#include "sequence.h"
#include "UspLib\UspLib.h"


class SpanView
{
public:
	SpanView(HWND hwnd, sequence *s);
	~SpanView();

	void DrawSpan(HDC hdc, int *x, int *y, sequence::span * sptr, SIZE *szOut);
	void DrawSpan2(HDC hdc, int *x, int *y, WCHAR *buf, int len, int type, COLORREF col, SIZE *szOut);
	LONG OnPaint();

	LRESULT SpanViewWndProc(UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI SpanViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND m_hWnd;
	int  m_nWrapWidth;
	sequence *m_seq;

	HFONT m_hFont;
	HFONT m_hFont2;
};


class UniView
{
public:
	UniView(HWND hwnd, sequence *s);
	LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

private:
	USPDATA *GetUspData(HDC hdc);
	int ApplyFormatting(WCHAR *wstr, int wlen, ATTR *attrList);
	void Uniscribe_MouseXToOffset(HWND hwnd, int mouseX, int *charpos, int *snappedToX);

	void PaintWnd();
	void MouseMove(int x, int y);
	void LButtonDown(int x, int y);
	void LButtonDblClick(int x, int y);
	void LButtonUp(int x, int y);
	void KeyDown(UINT nKey, UINT nFlags);
	void CharInput(UINT uChar, DWORD flags);
	void SetFont(HFONT hFont);
	void ReposCaret(ULONG offset, BOOL fTrailing);

	HWND	m_hWnd;

	// selection and cursor positions
	int		m_nSelStart;
	int		m_nSelEnd;
	int		m_nCurPos;
	BOOL	m_fMouseDown;
	BOOL	m_fInsertMode;
	BOOL	m_fTrailing;

	int		m_nLineHeight;
	USPFONT m_uspFontList[1];

	USPDATA *m_uspData;
	ATTR	 m_attrRunList[100];

	sequence *m_seq;
};