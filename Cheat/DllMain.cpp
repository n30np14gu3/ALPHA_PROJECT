#include "DllMain.h"

void MainThread()
{
	AntiLeak::HideThread(GetCurrentThread());
	globals::initGlobals();	
}

void ProtectionThread()
{
	AntiLeak::HideThread(GetCurrentThread());
	while(true)
	{
		if(AntiLeak::CheckDbgPresentCloseHandle())
			break;

		if (AntiLeak::Int2DCheck())
			break;

		if (AntiLeak::DebugObjectCheck())
			break;

		if (AntiLeak::IsDbgPresentPrefixCheck())
			break;

		Sleep(3000);

	}
	TerminateProcess(GetCurrentProcess(), 0);
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason,LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(ProtectionThread), nullptr, 0, nullptr);
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), nullptr, 0, nullptr);
		break;
	default:
		break;
	}
	return TRUE;
}