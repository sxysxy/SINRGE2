#include "ruby.h"
#include "ruby/encoding.h"
#include "RbInput.h"
#include "nge_graphics.h"

static VALUE mInput;

static int		mouseButton			= 0;
static short	mouseWheel			= 0;
static int		onFocus				= 1;
static int		mouseMove			= 0;
static int		hideMouse			= 0;

int OnFocus()
{
	return onFocus;
}

int MouseWheel()
{   
	return mouseWheel;
}

int MouseDblClk(int iKey)
{
	return mouseButton & (iKey<<6);
}

int GetMouseMove()
{
	return mouseMove;
}

void HideMouse(int hide)
{
	hideMouse = hide;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//mouseButton = 0;
	mouseMove = 0;
	
	switch(message)
	{
        case WM_KILLFOCUS:
			onFocus = 0;
			break;
        case WM_SETFOCUS:
			onFocus = 1;
			break;
	}

	switch (message)
	{
		case WM_ACTIVATEAPP:
			return 0;
		case WM_SETCURSOR:
			if (LOWORD(lParam)==HTCLIENT && hideMouse) SetCursor(NULL);
			else SetCursor(LoadCursor(NULL, IDC_ARROW));
			return 0;

		case WM_CREATE:
			return 0;
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

        case WM_LBUTTONDBLCLK:
			mouseButton += 64;
			return 0;
        case WM_RBUTTONDBLCLK:
			mouseButton += 128;
			return 0;
        case WM_MBUTTONDBLCLK:
			mouseButton += 256;
			return 0;

		case WM_MOUSEMOVE:
			mouseMove = 1;
			return 0;
        case WM_MOUSEWHEEL:
            mouseWheel = (short)HIWORD(wParam);
			return 0;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
