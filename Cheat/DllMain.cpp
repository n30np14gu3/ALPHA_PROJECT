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
	if (!client.generate_key(1024))
		ExitProcess(0);

	if (!client.data_exchange())
		ExitProcess(0);

	if (!license_manager::getModulesInfo())
		ExitProcess(0);
}

void MainThread()
{
	//AntiLeak::HideThread(GetCurrentThread());
	//globals::initGlobals();
	//LoaderConnect();

	try {
		interfaces::initialize();

		printf("[setup] interfaces initialized!\n");

		hooks::initialize();

		printf("[setup] hooks initialized!\n");

		render.setup_fonts();

		printf("[setup] render initialized!\n");

		utilities::material_setup();

		printf("[setup] materials initialized!\n");

		config_system.run("aristois");

		printf("[setup] config initialized!\n");

		events.setup();

		printf("[setup] events initialized!\n");

		kit_parser.setup();

		printf("[setup] kit parser initialized!\n");
	}
	catch (const std::runtime_error & err) {
		printf(err.what());
	}

	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	hooks::shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), nullptr, 0, nullptr);
#if NDEBUG
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(ProtectionThread), nullptr, 0, nullptr);
		AntiLeak::ErasePEHeaderFromMemory();
#endif
		break;
	default:
		break;
	}
	return TRUE;
}