#pragma once
#include <Windows.h>
#include "SDK/protection/AntiLeak.h"
#include "SDK/globals/globals.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
void MainThread();
void ProtectionThread();