#include "SINRGE2.h"

using namespace Sin;

BOOL APIENTRY DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
		SetRubyLibHandle(dll);

    return TRUE;
}

