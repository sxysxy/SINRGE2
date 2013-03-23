#ifndef __SINRGE2_H__
#define __SINRGE2_H__
#include <Windows.h>

namespace Sin
{
	HWND	GetHwnd();
	void	SINRGE2Initialize();
	int		SINRGE2Eval(const char* script);
	int		SINRGE2Entry(const char* rubyfile);
	void	SetRubyLibHandle(HINSTANCE hModule);
}

#endif	//	__SINRGE2_H__