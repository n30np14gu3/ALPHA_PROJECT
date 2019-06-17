#pragma once
#include <Windows.h>

#pragma pack(1)

namespace AntiLeak
{
#define JUNK_CODE_ONE        \
    __asm{push eax}            \
    __asm{xor eax, eax}        \
    __asm{setpo al}            \
    __asm{push edx}            \
    __asm{xor edx, eax}        \
    __asm{sal edx, 2}        \
    __asm{xchg eax, edx}    \
    __asm{pop edx}            \
    __asm{or eax, ecx}        \
    __asm{pop eax}

#pragma pack(1)
	typedef struct _PROCESS_BASIC_INFORMATION {
		PVOID Reserved1;
		void* PebBaseAddress;
		PVOID Reserved2[2];
		ULONG_PTR UniqueProcessId;
		ULONG_PTR ParentProcessId;
	} PROCESS_BASIC_INFORMATION;

	typedef struct _LSA_UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR Buffer;
	} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING,
		UNICODE_STRING, *PUNICODE_STRING;


	typedef struct _OBJECT_TYPE_INFORMATION {
		UNICODE_STRING TypeName;
		ULONG TotalNumberOfHandles;
		ULONG TotalNumberOfObjects;
	}OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

	typedef struct _OBJECT_ALL_INFORMATION {
		ULONG NumberOfObjects;
		OBJECT_TYPE_INFORMATION ObjectTypeInformation[1];
	}OBJECT_ALL_INFORMATION, *POBJECT_ALL_INFORMATION;
#pragma pack()

	bool CheckDbgPresentCloseHandle();
	void DebugSelf();
	bool HideThread(HANDLE hThread);
	bool DebugObjectCheck();
	bool CheckProcessDebugFlags();
	bool Int2DCheck();
	bool IsDbgPresentPrefixCheck();
	void ErasePEHeaderFromMemory();
	void ChangeSizeOfImage(DWORD NewSize);

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
}