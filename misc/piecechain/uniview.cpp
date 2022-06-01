//
//	UniView.cpp
//	
//	single-line text editor using UspLib
//
//	Written by J Brown 2006	Freeware
//
//
#define _WIN32_WINNT 0x501
#define UNICODE
#define _UNICODE

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "resource.h"
#include "sequence.h"
#include "spanview.h"
#include "usplib\usplib.h"

#define UNIVIEWCLASS _T("UNIVIEW")
#define XBORDER   10
#define YBORDER	  10


UniView::UniView(HWND hwnd, sequence *s)
{
	m_hWnd			= hwnd;
	m_seq			= s;
	m_nSelStart		= 0;
	m_nSelEnd		= 0;
	m_nCurPos		= 0;
	m_fMouseDown	= FALSE;
	m_fInsertMode	= TRUE;

	ZeroMemory(m_uspFontList, sizeof(m_uspFontList));
	SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));

	SetCaretPos(XBORDER, YBORDER/2);
}

void UniView::ReposCaret(ULONG offset, BOOL fTrailing)
{
	HDC		hdc = GetDC(m_hWnd);
	int x;

	if(fTrailing)
		UspOffsetToX(GetUspData(hdc), m_nCurPos-1, TRUE, &x);
	else
		UspOffsetToX(GetUspData(hdc), m_nCurPos,   FALSE, &x);

	ReleaseDC(m_hWnd, hdc);
	SetCaretPos(x + XBORDER, YBORDER/2);

	{
		WCHAR buf[100];
		wsprintf(buf, L"%d\n", m_nCurPos);
		OutputDebugString(buf);
	}

}

void UniView::SetFont(HFONT hFont)
{
	HDC hdc = GetDC(m_hWnd);
	RECT rect;

	m_uspFontList[0].hFont = hFont;
	UspInitFont(&m_uspFontList[0], hdc, hFont);
	m_nLineHeight = m_uspFontList[0].tm.tmHeight + m_uspFontList[0].tm.tmExternalLeading;

	ReleaseDC(m_hWnd, hdc);

	GetWindowRect(m_hWnd, &rect);
	SetWindowPos(m_hWnd, 0, 0, 0, rect.right-rect.left, m_nLineHeight + YBORDER*3, SWP_NOMOVE|SWP_NOZORDER);
}

int UniView::ApplyFormatting(WCHAR *wstr, int wlen, ATTR *attrList)
{
	int  i;
	int  s1 = min(m_nSelStart, m_nSelEnd);
	int  s2 = max(m_nSelStart, m_nSelEnd);

	for(i = 0; i < wlen; i++)
	{
		attrList[i].sel = (i >= s1 && i < s2) ? 1 : 0;
		
		attrList[i].fg = 0;
		attrList[i].bg = GetSysColor(COLOR_WINDOW);
		
		if(i >= 2 && i <= 5)
			attrList[i].bg = RGB(220,220,220);

		if(i % 2)
			attrList[i].fg = RGB(200,0,50);
	
		attrList[i].len  = 1;
		attrList[i].font = 0;
	}

	return 0;
}

USPDATA * UniView::GetUspData(HDC hdc)
{
	SCRIPT_CONTROL sc = { 0 };
	SCRIPT_STATE   ss = { 0 };

	WCHAR  wstr[100]; 
	int    wlen = min(100, m_seq->size()); 

	m_seq->render(0, wstr, wlen);
	
	UspAnalyze(m_uspData, hdc, wstr, wlen, 0, 0, m_uspFontList, &sc, &ss, 0);
	UspApplySelection(m_uspData, m_nSelStart, m_nSelEnd);

	return m_uspData;
}

//
//	Map mouse-x coordinate to a character-offset and return x-coord
//	of selected character
//
void UniView::Uniscribe_MouseXToOffset(HWND hwnd, int mouseX, int *charpos, int *snappedToX)
{
	HDC		hdc = GetDC(hwnd);
	UspSnapXToOffset(GetUspData(hdc), mouseX, snappedToX, charpos, 0);
	ReleaseDC(hwnd, hdc);
}

//
//	Display the string
//
void UniView::PaintWnd()
{
	PAINTSTRUCT ps;
	RECT		rect;
	HDC			hdcMem;
	HBITMAP		hbmMem;
	USPDATA		*uspData;

	BeginPaint(m_hWnd, &ps);
	GetClientRect(m_hWnd, &rect);

	//
	//	Create memory-dc for double-buffering
	//	(StringStringOut flickers badly!!)
	//
	hdcMem = CreateCompatibleDC(ps.hdc);
	hbmMem = CreateCompatibleBitmap(ps.hdc, rect.right, rect.bottom);
	SelectObject(hdcMem, hbmMem);

	//	paint the text
	FillRect(hdcMem, &rect, GetSysColorBrush(COLOR_WINDOW));

	uspData = GetUspData(hdcMem);

	UspTextOut(uspData, hdcMem, XBORDER, YBORDER, m_nLineHeight, 0, &rect);

	// copy to window-dc
	BitBlt(ps.hdc, 0, 0, rect.right,rect.bottom, hdcMem, 0, 0, SRCCOPY);

	// cleanup
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);
	EndPaint(m_hWnd, &ps);
}

//
//	Left mouse-down handler
//
void UniView::LButtonDown(int x, int y)
{
	Uniscribe_MouseXToOffset(m_hWnd, x - XBORDER, &m_nCurPos, &x);

	m_nSelStart = m_nCurPos;
	m_nSelEnd   = m_nCurPos;

	InvalidateRect(m_hWnd, 0, 0);
	
	SetCaretPos(x + XBORDER, YBORDER/2);

	m_fMouseDown = TRUE;
	SetCapture(m_hWnd);

	PostMessage(GetParent(m_hWnd), WM_USER, 0, 0);
}

void UniView::LButtonDblClick(int x, int y)
{
	m_nSelStart = 0;
	m_nSelEnd = m_seq->size();
	m_nCurPos = m_nSelEnd;

	InvalidateRect(m_hWnd, 0, 0);
	ReposCaret(m_nCurPos, FALSE);
}

//
//	mouse-move handler
//	
void UniView::MouseMove(int x, int y)
{
	if(m_fMouseDown)
	{
		Uniscribe_MouseXToOffset(m_hWnd, x - XBORDER, &m_nCurPos, &x);
		
		m_nSelEnd = m_nCurPos;
		
		InvalidateRect(m_hWnd, 0, 0);
		SetCaretPos(x + XBORDER, YBORDER/2);
		
		PostMessage(GetParent(m_hWnd), WM_USER, 0, 0);
	}
}

//
//	mouse-up handler
//
void UniView::LButtonUp(int x, int y)
{
	ReleaseCapture();
	m_fMouseDown = FALSE;
}

bool IsKeyPressed(UINT uVK)
{
	return (GetKeyState(uVK) & 0x80000000) ? true : false;
}

void UniView::KeyDown(UINT nKey, UINT nFlags)
{
	int s1 = min(m_nSelStart, m_nSelEnd);
	int s2 = max(m_nSelStart, m_nSelEnd);
	//BOOL	fTrailing = FALSE;
	m_fTrailing = FALSE;

	switch(nKey)
	{
	case VK_CONTROL: case VK_SHIFT:
		return;

	case VK_INSERT:
		m_fInsertMode = !m_fInsertMode;
		return;

	case 'z': case 'Z':
		
		if(IsKeyPressed(VK_CONTROL))
		{
			if(m_seq->undo())
			{
				m_nCurPos = m_seq->event_index();
				m_nSelStart = m_nCurPos;
				m_nCurPos += m_seq->event_length();
				m_nSelEnd  = m_nCurPos;
				
				InvalidateRect(m_hWnd, 0, 0);
				PostMessage(GetParent(m_hWnd), WM_USER, 0, 0);
				goto repos;
			}
			
			return;
		}

		break;

	case 'y': case 'Y':
		if(IsKeyPressed(VK_CONTROL))
		{
			if(m_seq->redo())
			{
				//InvalidateRect(hwnd, 0, 0);
				//PostMessage(GetParent(hwnd), WM_USER, 0, 0);
				m_nCurPos = m_seq->event_index();
				m_nSelStart = m_nCurPos;
				m_nCurPos += m_seq->event_length();
				m_nSelEnd  = m_nCurPos;
				InvalidateRect(m_hWnd, 0, 0);
				PostMessage(GetParent(m_hWnd), WM_USER, 0, 0);
				goto repos;
			}
			
			return;
		}

		break;

	case VK_BACK:

		if(s1 != s2)
		{
			m_seq->erase(s1, s2 - s1);
			m_nCurPos = s1;
		}
		else if(m_nCurPos > 0)
		{
			m_nCurPos--;
			m_seq->erase(m_nCurPos, 1);
		}
		
		PostMessage(GetParent(m_hWnd), WM_USER, 0, 0);
		InvalidateRect(m_hWnd, 0, 0);

		break;

	case VK_DELETE:

		if(s1 != s2)
		{
			m_seq->erase(s1, s2 - s1);
			m_nCurPos = s1;
		}
		else
		{
			m_seq->erase(m_nCurPos, 1);
		}
		
		PostMessage(GetParent(m_hWnd), WM_USER, 0, 0);
		InvalidateRect(m_hWnd, 0, 0);
	
		break;

	case VK_LEFT:

		m_fTrailing = FALSE;
		if(m_nCurPos > 0)
			m_nCurPos--;
	
		break;

	case VK_RIGHT:
		m_fTrailing = TRUE;
		m_nCurPos++;
		break;

	case VK_HOME:
		m_nCurPos = 0;
		break;

	case VK_END:
		m_fTrailing = TRUE;
		m_nCurPos = m_seq->size();
		break;

	default:
		return;
	}

	if(IsKeyPressed(VK_SHIFT))
	{
		m_nSelEnd = m_nCurPos;
		InvalidateRect(m_hWnd, 0, 0);
	}
	else
	{
		//if(nSelEnd != nCurPos)
		InvalidateRect(m_hWnd, 0, 0);

		m_nSelStart = m_nCurPos;
		m_nSelEnd   = m_nCurPos;
	}

repos:
	ReposCaret(m_nCurPos, m_fTrailing);	
}

void UniView::CharInput(UINT uChar, DWORD flags)
{
	int s1 = min(m_nSelStart, m_nSelEnd);
	int s2 = max(m_nSelStart, m_nSelEnd);
	WCHAR ch = uChar;

	if(uChar < 32)
		return;

	BOOL fReplaceSelection = (m_nSelStart == m_nSelEnd) ? FALSE : TRUE;
	
	if(fReplaceSelection)
	{
		//if(!m_fInsertMode) s2--;
		m_seq->group();
		m_seq->erase(s1, s2-s1);
		m_nCurPos = s1;
	}

	if(m_fInsertMode)// && !fReplaceSelection)
	{
		m_seq->insert(m_nCurPos, ch);
	}
	else
	{
		m_seq->replace(m_nCurPos, ch);
	}

	if(fReplaceSelection)
		m_seq->ungroup();

	m_nCurPos ++;
	m_nSelStart = m_nCurPos;
	m_nSelEnd   = m_nCurPos;

	/*HDC		hdc = GetDC(m_hWnd);
	int x;
	UspOffsetToX(GetUspData(hdc), m_nCurPos, FALSE, &x);
	ReleaseDC(m_hWnd, hdc);
	SetCaretPos(x + XBORDER, YBORDER/2);*/
	ReposCaret(m_nCurPos, TRUE);

	InvalidateRect(m_hWnd, 0, 0);
	PostMessage(GetParent(m_hWnd), WM_USER, 0, 0);
}

//
//	Main window procedure
//
LRESULT UniView::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		m_uspData = UspAllocate();
		return 0;

	case WM_DESTROY:
		UspFree(m_uspData);
		return 0;

	case WM_SETFONT:
		SetFont((HFONT)wParam);
		return 0;

	case WM_PAINT:
		PaintWnd();
		return 0;

	case WM_RBUTTONDOWN:
		InvalidateRect(m_hWnd, 0, 0);
		return 0;

	case WM_LBUTTONDBLCLK:
		LButtonDblClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_LBUTTONDOWN:
		LButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_MOUSEMOVE:
		MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_LBUTTONUP:
		LButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_KEYDOWN:
		KeyDown(wParam, lParam);
		return 0;

	case WM_CHAR:
		CharInput(wParam, lParam);
		return 0;

	case WM_KILLFOCUS:
		HideCaret(m_hWnd);
		DestroyCaret();
		return 0;

	case WM_SETFOCUS:
		CreateCaret(m_hWnd, NULL, 2, m_nLineHeight + YBORDER);
		ShowCaret(m_hWnd);
		return 0;
	}

	return DefWindowProc(m_hWnd, msg, wParam, lParam);
}

LRESULT WINAPI UniViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UniView *uvptr = (UniView *)GetWindowLongPtr(hwnd, 0);

	switch(msg)
	{
	case WM_NCCREATE:
		if((uvptr = new UniView(hwnd, (sequence *)((CREATESTRUCT *)lParam)->lpCreateParams)) == 0)
			return FALSE;

		SetWindowLongPtr(hwnd, 0, (LONG_PTR)uvptr);
		return TRUE;

	case WM_NCDESTROY:
		delete uvptr;
		return 0;

	default:
		if(uvptr)
			return uvptr->WndProc(msg, wParam, lParam);
		else
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

//
//	Initialize the window-class
//
ATOM InitUniView()
{
	WNDCLASSEX wc;

	wc.cbSize			= sizeof(wc);
	wc.style			= CS_DBLCLKS;
	wc.lpfnWndProc		= UniViewWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(UniView *);
	wc.hInstance		= GetModuleHandle(0);
	wc.hIcon			= LoadIcon(0, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.hCursor			= LoadCursor (NULL, IDC_IBEAM);
	wc.hbrBackground	= 0;
	wc.lpszMenuName		= MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName	= UNIVIEWCLASS;
	wc.hIconSm			= LoadIcon (NULL, IDI_APPLICATION);
	
	return RegisterClassEx(&wc);
}

//
//	Create the unicode viewer
//
HWND CreateUniView(HWND hwndParent, sequence *seq)
{
	InitUniView();

	return CreateWindowEx(WS_EX_CLIENTEDGE,
			UNIVIEWCLASS, 0,
			WS_VISIBLE|WS_CHILD,
			0, 0, 0, 0, hwndParent, 0, 0, seq);
}		
