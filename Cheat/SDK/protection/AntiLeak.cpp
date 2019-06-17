#include "AntiLeak.h"
#include "../crypto/XorStr.h"

namespace AntiLeak
{
	bool CheckDbgPresentCloseHandle()
	{
		HANDLE Handle = reinterpret_cast<HANDLE>(0x8000);
		__try
		{
			CloseHandle(Handle);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return true;
		}

		return false;
	}

	void DebugSelf()
	{
		HANDLE hProcess = NULL;
		DEBUG_EVENT de;
		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&si, sizeof(STARTUPINFO));
		ZeroMemory(&de, sizeof(DEBUG_EVENT));

		GetStartupInfo(&si);

		// Create the copy of ourself
		CreateProcess(NULL, GetCommandLine(), NULL, NULL, FALSE,
			DEBUG_PROCESS, NULL, NULL, &si, &pi);

		// Continue execution
		ContinueDebugEvent(pi.dwProcessId, pi.dwThreadId, DBG_CONTINUE);

		// Wait for an event
		WaitForDebugEvent(&de, INFINITE);
	}

	bool HideThread(HANDLE hThread)
	{
		typedef NTSTATUS(NTAPI *pNtSetInformationThread)
			(HANDLE, UINT, PVOID, ULONG);

		NTSTATUS Status;

		// Get NtSetInformationThread
		pNtSetInformationThread NtSIT = (pNtSetInformationThread)
			GetProcAddress(GetModuleHandle(TEXT(XorStr("ntdll.dll"))), XorStr("NtSetInformationThread"));
		// Shouldn't fail
		if (NtSIT == NULL)
			return false;

		// Set the thread info
		if (hThread == NULL)
			Status = NtSIT(GetCurrentThread(),
				0x11, //ThreadHideFromDebugger
				0, 0);
		else
			Status = NtSIT(hThread, 0x11, 0, 0);

		if (Status != 0x00000000)
			return false;
		return true;
	}

	bool DebugObjectCheck()
	{
		typedef NTSTATUS(WINAPI *pNtQueryInformationProcess)
			(HANDLE, UINT, PVOID, ULONG, PULONG);

		HANDLE hDebugObject = NULL;
		NTSTATUS Status;

		// Get NtQueryInformationProcess
		pNtQueryInformationProcess NtQIP = (pNtQueryInformationProcess)
			GetProcAddress(
				GetModuleHandle(TEXT(XorStr("ntdll.dll"))), XorStr("NtQueryInformationProcess"));

		Status = NtQIP(GetCurrentProcess(),
			0x1e, // ProcessDebugObjectHandle
			&hDebugObject, 4, NULL);

		if (Status != 0x00000000)
			return false;

		return hDebugObject != nullptr;
	}

	bool CheckProcessDebugFlags()
	{
		typedef NTSTATUS(WINAPI *pNtQueryInformationProcess)
			(HANDLE, UINT, PVOID, ULONG, PULONG);

		DWORD NoDebugInherit = 0;
		NTSTATUS Status;

		// Get NtQueryInformationProcess
		pNtQueryInformationProcess NtQIP = (pNtQueryInformationProcess)
			GetProcAddress(
				GetModuleHandle(TEXT(XorStr("ntdll.dll"))), XorStr("NtQueryInformationProcess"));

		Status = NtQIP(GetCurrentProcess(),
			0x1f, // ProcessDebugFlags
			&NoDebugInherit, 4, NULL);

		if (Status != 0x00000000)
			return false;

		return NoDebugInherit == FALSE;
	}

	bool Int2DCheck()
	{
		__try
		{
			__asm
			{
				int 0x2d
				xor eax, eax
				add eax, 2
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}

		return true;
	}

	inline void PushPopSS()
	{
		__asm
		{
			push ss
			pop ss
			mov eax, 9 // This line executes but is stepped over
			xor edx, edx // This is where the debugger will step to
		}
	}

	bool IsDbgPresentPrefixCheck()
	{
		__try
		{
			__asm __emit 0xF3 // 0xF3 0x64 disassembles as PREFIX REP:
			__asm __emit 0x64
			__asm __emit 0xF1 // One byte INT 1
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}

		return true;
	}
}