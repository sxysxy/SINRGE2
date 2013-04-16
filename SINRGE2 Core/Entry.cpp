extern "C"
{
#include "ruby.h"
}
#include "sin_app.h"

BOOL APIENTRY DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
		rb_set_lib_handle(dll);

    return TRUE;
}

int SINRGE2Entry()
{
	return CApplication().Run();
}