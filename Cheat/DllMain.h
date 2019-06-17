#pragma once
#include <Windows.h>
#include "SDK/protection/AntiLeak.h"
#include "SDK/globals/globals.h"
#include "SDK/network/sokets/local_client.h"
#include "SDK/license/license_manager.h"
#include "SDK/crypto/XorStr.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
void MainThread();
void LoaderConnect();
void ProtectionThread();