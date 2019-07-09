#include "dependencies/common_includes.hpp"
#include "core/features/misc/misc.hpp"
#include "core/menu/config/config.hpp"
#include "core/features/misc/events.hpp"
#include "core/features/skinchanger/parser.hpp"

#include "DllMain.h"
#include "core/features/skinchanger/knifehook.hpp"

void LoaderConnect()
{
#if !NDEBUG
	MessageBox(nullptr, "DEBUG BUILD!", "", MB_OK);
#endif
	Sleep(1000);

#if !NDEBUG
	MessageBox(nullptr, "Connection...", "", MB_OK);
#endif
	local_client client(XorStr("127.0.0.1"), 4980);
	if (!client.NoError)
	{
		TerminateProcess(GetCurrentProcess(), 0);
	}

#if !NDEBUG
	MessageBox(nullptr, "Data Exchange...", "", MB_OK);
#endif
	if (!client.data_exchange())
	{
		TerminateProcess(GetCurrentProcess(), 0);
	}
	Sleep(500);

	if (!license_manager::getModulesInfo())
	{
		TerminateProcess(GetCurrentProcess(), 0);
	}
#if !NDEBUG
	MessageBox(nullptr, (std::string("Get modules info...") + std::to_string(globals::user_modules.modules_count)).c_str(), "", MB_OK);
#endif
	Sleep(500);
}

void CheckModulesActive()
{
	while (true)
	{
		if (license_manager::allModulesExpired())
			ExitProcess(0);

		Sleep(1000);
	}
}

void MainThread()
{
#if !NDEBUG
	MessageBox(nullptr, "Injected", "", MB_OK);
#endif

	globals::initGlobals();
#if NDEBUG
	AntiLeak::HideThread(GetCurrentThread());

	LoaderConnect();
#endif
	try 
	{
		config_system.run(XorStr("alpha project"));
		interfaces::initialize();
		hooks::initialize();
		utilities::material_setup();
		knife_hook.knife_animation();
		Beep(600, 600);

#if NDEBUG
		//CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(ProtectionThread), nullptr, 0, nullptr);
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(CheckModulesActive), nullptr, 0, nullptr);
#endif
	

		while (!GetAsyncKeyState(VK_END))
			std::this_thread::sleep_for(std::chrono::milliseconds(50));

		hooks::shutdown();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
#if NDEBUG
		DisableThreadLibraryCalls(hinstDLL);
#endif

		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), nullptr, 0, nullptr);
		break;
	default:
		break;
	}
	return TRUE;
}