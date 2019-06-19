#include "dependencies/common_includes.hpp"
#include "core/features/misc/misc.hpp"
#include "core/menu/config/config.hpp"
#include "core/features/misc/events.hpp"
#include "core/features/skinchanger/parser.hpp"

#include "DllMain.h"

void LoaderConnect()
{

#if NDEBUG
	JUNK_CODE_ONE
	AntiLeak::PushPopSS();
#endif

	local_client client(XorStr("127.0.0.1"), 4980);
	if (!client.NoError)
		ExitProcess(0);
	if (!client.generate_key(1024))
		ExitProcess(0);

	if (!client.data_exchange())
		ExitProcess(0);

	if (!license_manager::getModulesInfo())
		ExitProcess(0);
}

void MainThread()
{
	AntiLeak::HideThread(GetCurrentThread());
	//globals::initGlobals();
	//LoaderConnect();

	try {
		interfaces::initialize();
		hooks::initialize();
		render.setup_fonts();
		utilities::material_setup();
		config_system.run(XorStr("alpha project"));
		events.setup();
		kit_parser.setup();
	}
	catch (const std::runtime_error & err) 
	{

	}

	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	hooks::shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void ProtectionThread()
{
	Sleep(5000);
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
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), nullptr, 0, nullptr);
#if NDEBUG
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(ProtectionThread), nullptr, 0, nullptr);
#endif
		break;
	default:
		break;
	}
	return TRUE;
}