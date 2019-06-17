#pragma once
#include <Windows.h>

#pragma pack(1)

namespace AntiLeak
{
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
	inline void PushPopSS();
}