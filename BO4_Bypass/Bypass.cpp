// BO4_Bypass.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Bypass.hpp"

#pragma region Member
Bypass* Bypass::Instance;
HANDLE Bypass::hEvent, Bypass::hProcessHandle, Bypass::hThread;

p_NtCreateFile Bypass::o_NtCreateFile;
p_RtlInitUnicodeString Bypass::o_RtlInitUnicodeString;
p_NtQueryVirtualMemory Bypass::o_NtQueryVirtualMemory;
p_EnumWindows Bypass::o_EnumWindows;
p_NtUserQueryWindow Bypass::o_NtUserQueryWindow;
p_NtUserFindWindowEx Bypass::o_NtUserFindWindowEx;
p_NtUserWindowFromPoint Bypass::o_NtUserWindowFromPoint;
p_NtUserGetForegroundWindow Bypass::o_NtUserGetForegroundWindow;
p_NtUserGetThreadState Bypass::o_NtUserGetThreadState;
p_NtOpenProcess Bypass::o_NtOpenProcess;
p_NtQuerySystemInformation Bypass::o_NtQuerySystemInformation;
p_NtQueryInformationThread Bypass::o_NtQueryInformationThread;
p_NtSetInformationThread Bypass::o_NtSetInformationThread;
p_NtReadVirtualMemory Bypass::o_NtReadVirtualMemory;
p_ZwQueryInformationProcess Bypass::o_ZwQueryInformationProcess;
p_ZwQueryInformationProcess Bypass::o_NtWow64QueryInformationProcess64;
p_NtWow64QueryVirtualMemory64 Bypass::o_NtWow64QueryVirtualMemory64;
p_NtGetContextThread Bypass::o_NtGetContextThread;
p_NtOpenThread Bypass::o_NtOpenThread;
p_NtWow64ReadVirtualMemory64 Bypass::o_NtWow64ReadVirtualMemory64;
p_NtReadFile Bypass::o_NtReadFile;
p_Sleep Bypass::o_Sleep;
DWORD_PTR Bypass::hModule;
#pragma endregion Member
Bypass* Bypass::GetInstance()
{
	if (!Instance)
		Instance = new Bypass;
	return Instance;
}

Bypass::Bypass()
{
	Instance = 0;
	hEvent = 0;
	hModule = 0;
	hProcessHandle = 0;
	hThread = 0;
	o_NtCreateFile = 0;
	o_RtlInitUnicodeString = 0;
	o_NtQueryVirtualMemory = 0;
	o_EnumWindows = 0;
	o_NtUserQueryWindow = 0;
	o_NtUserFindWindowEx = 0;
	o_NtUserWindowFromPoint = 0;
	o_NtUserGetForegroundWindow = 0;
	o_NtUserGetThreadState = 0;
	o_NtOpenProcess = 0;
	o_NtQuerySystemInformation = 0;
	o_NtQueryInformationThread = 0;
	o_NtSetInformationThread = 0;
	o_NtReadVirtualMemory = 0;
	o_ZwQueryInformationProcess = 0;
	o_NtWow64QueryInformationProcess64 = 0;
	o_NtWow64QueryVirtualMemory64 = 0;
	o_NtGetContextThread = 0;
	o_NtOpenThread = 0;
	o_NtWow64ReadVirtualMemory64 = 0;
	o_NtReadFile = 0;
	o_Sleep = 0;
}
Bypass::~Bypass()
{
	if (Instance)
		delete Instance;
}

bool Bypass::Init(HMODULE hDll)
{
	VirtualizerStart();
#if (LOG_STATE == 1)
	DbgLog::Log("============================== Call of Duty Black Ops 4 BYPASS ===============================");
	DbgLog::Log("| LOADING SETTINGS |");
	DbgLog::Log("DISABLE_DLLDETECTION = %d", DISABLE_DLLDETECTION);
	DbgLog::Log("DISABLE_THREADDETECTION = %d", DISABLE_THREADDETECTION);
	DbgLog::Log("DISABLE_PROCESSDETECTION = %d", DISABLE_PROCESSDETECTION);
	DbgLog::Log("DISABLE_ANTIDEBUGGER = %d", DISABLE_ANTIDEBUGGER);
	DbgLog::Log("DISABLE_READFILE = %d", DISABLE_READFILE);
	DbgLog::Log("LOG_STATE = %d", LOG_STATE);
	DbgLog::Log("LOG_CREATESEMPAPHORE = %d", LOG_CREATESEMPAPHORE);
	DbgLog::Log("LOG_NTQUERYVIRTUALMEMORY = %d", LOG_NTQUERYVIRTUALMEMORY);
	DbgLog::Log("LOG_NTWOW64QUERYVIRTUALMEMORY64 = %d", LOG_NTWOW64QUERYVIRTUALMEMORY64);
	DbgLog::Log("LOG_NTOPENPROCESS = %d", LOG_NTOPENPROCESS);
	DbgLog::Log("LOG_NTQUERYWINDOW = %d", LOG_NTQUERYWINDOW);
	DbgLog::Log("LOG_NTQUERYSYSTEMINFORMATION = %d", LOG_NTQUERYSYSTEMINFORMATION);
	DbgLog::Log("LOG_NTGETCONTEXTTHREAD = %d", LOG_NTGETCONTEXTTHREAD);
	DbgLog::Log("LOG_NTOPENTHREAD = %d", LOG_NTOPENTHREAD);
	DbgLog::Log("USE_VMPROTECT = %d", USE_VMPROTECT);
	DbgLog::Log("| SETTINGS LOADED |");
	DbgLog::Log("| STARTING BYPASS |");
	DbgLog::Log("Bypass::Init START 0x%X", hDll);
#endif
	hEvent = CreateEventA(0, 0, 0, 0);
	hProcessHandle = (OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId()));
	o_RtlInitUnicodeString = o_RtlInitUnicodeString ? o_RtlInitUnicodeString : reinterpret_cast<p_RtlInitUnicodeString>(reinterpret_cast<DWORD_PTR>(GetProcAddress(GetModuleHandle("ntdll.dll"), "RtlInitUnicodeString")));
	o_NtQueryInformationThread = o_NtQueryInformationThread ? o_NtQueryInformationThread : reinterpret_cast<p_NtQueryInformationThread>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryInformationThread"));
	o_NtWow64ReadVirtualMemory64 = o_NtWow64ReadVirtualMemory64 ? o_NtWow64ReadVirtualMemory64 : reinterpret_cast<p_NtWow64ReadVirtualMemory64>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtWow64ReadVirtualMemory64"));
	hModule = reinterpret_cast<DWORD_PTR>(hDll);
	if (!hProcessHandle || hProcessHandle == INVALID_HANDLE_VALUE ||
		!o_NtQueryInformationThread ||
		!hEvent ||
		!detour_Functions(true))
	{
#if (LOG_STATE == 1)
		DbgLog::Log("Bypass::Init FAIL 0x0 = FALSE");
		DbgLog::Log("| END BYPASS |");
#endif
		return false;
	}

#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::Init END");
	DbgLog::Log("| END BYPASS |");
	DbgLog::Log("============================== Call of Duty Black Ops 4 BYPASS ===============================");
#endif
	VirtualizerEnd();
	return true;
}

bool Bypass::Uninit()
{
	VirtualizerStart();
#if (LOG_STATE == 1)
	DbgLog::Log("============================== Call of Duty Black Ops 4 BYPASS ===============================");
	DbgLog::Log("Bypass::Uninit START");
#endif
	CloseHandle(hProcessHandle);
	if (hEvent)
	{
		SetEvent(hEvent);
		CloseHandle(hEvent);
	}
	if (hThread)
	{
		TerminateThread(hThread, 0);
		CloseHandle(hThread);
	}

	if (!detour_Functions(false))
	{
#if (LOG_STATE == 1)
		DbgLog::Log("Bypass::Uninit FAIL 0x0 = FALSE");
		return false;
#endif
	}
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::Uninit END");
	DbgLog::Log("============================== Call of Duty Black Ops 4 BYPASS ===============================");
#endif
	VirtualizerEnd();
	CloseHandle(hProcessHandle);
	return true;
}

#pragma region Hooks

bool Bypass::detour_Functions(bool Status)
{
	INT bStatus = 0;
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::detour_Functions START %d", Status);
#endif

#if (DISABLE_THREADDETECTION == 1)
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::DISABLE_THREADDETECTION START %d", Status);
#endif
	VirtualizerStart();
	o_NtGetContextThread = o_NtGetContextThread ? o_NtGetContextThread : reinterpret_cast<p_NtGetContextThread>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtGetContextThread"));
	if (!o_NtGetContextThread)
		bStatus = 4;
	o_NtOpenThread = o_NtOpenThread ? o_NtOpenThread : reinterpret_cast<p_NtOpenThread>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtOpenThread"));
	if (!o_NtOpenThread)
		bStatus = 5;
	if (!o_NtGetContextThread ||
		!o_NtOpenThread ||
		DetourTransactionBegin() != NO_ERROR ||
		DetourUpdateThread(GetCurrentThread()) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtGetContextThread, NtGetContextThread_Hook) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtOpenThread, NtOpenThread_Hook) != NO_ERROR ||
		DetourTransactionCommit() != NO_ERROR)
		if (!bStatus)
			bStatus = 6;
	VirtualizerEnd();
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::DISABLE_THREADDETECTION END %d", bStatus);
#endif
#endif

#if (DISABLE_DLLDETECTION == 1)
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::DISABLE_DLLDETECTION START %d", Status);
#endif
	VirtualizerStart();
	o_NtQueryVirtualMemory = o_NtQueryVirtualMemory ? o_NtQueryVirtualMemory : reinterpret_cast<p_NtQueryVirtualMemory>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryVirtualMemory"));
	if (!o_NtQueryVirtualMemory)
		bStatus = 10;
	o_NtWow64QueryVirtualMemory64 = o_NtWow64QueryVirtualMemory64 ? o_NtWow64QueryVirtualMemory64 : reinterpret_cast<p_NtWow64QueryVirtualMemory64>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtWow64QueryVirtualMemory64"));
	if (o_NtWow64QueryVirtualMemory64)
	{
		if (DetourTransactionBegin() != NO_ERROR ||
			DetourUpdateThread(GetCurrentThread()) != NO_ERROR ||
			(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtWow64QueryVirtualMemory64, NtWow64QueryVirtualMemory64_Hook) != NO_ERROR ||
			DetourTransactionCommit() != NO_ERROR)
			if (!bStatus)
				bStatus = 11;
	}
	if (!o_NtQueryVirtualMemory ||
		DetourTransactionBegin() != NO_ERROR ||
		DetourUpdateThread(GetCurrentThread()) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtQueryVirtualMemory, NtQueryVirtualMemory_Hook) != NO_ERROR ||
		DetourTransactionCommit() != NO_ERROR)
		if (!bStatus)
			bStatus = 12;
	VirtualizerEnd();
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::DISABLE_DLLDETECTION END %d", bStatus);
#endif
#endif

#if (DISABLE_PROCESSDETECTION == 1)
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::DISABLE_PROCESSDETECTION START %d", Status);
#endif
	VirtualizerStart();

	o_EnumWindows = o_EnumWindows ? o_EnumWindows : reinterpret_cast<p_EnumWindows>(GetProcAddress(GetModuleHandle("user32.dll"), "EnumWindows"));
	if (!o_EnumWindows)
		bStatus = 15;
	o_NtUserQueryWindow = o_NtUserQueryWindow ? o_NtUserQueryWindow : reinterpret_cast<p_NtUserQueryWindow>(GetProcAddress(GetModuleHandle("win32u.dll"), "NtUserQueryWindow"));
	if (!o_NtUserQueryWindow)
		bStatus = 16;
	o_NtUserFindWindowEx = o_NtUserFindWindowEx ? o_NtUserFindWindowEx : reinterpret_cast<p_NtUserFindWindowEx>(GetProcAddress(GetModuleHandle("win32u.dll"), "NtUserFindWindowEx"));
	if (!o_NtUserFindWindowEx)
		bStatus = 17;
	o_NtUserWindowFromPoint = o_NtUserWindowFromPoint ? o_NtUserWindowFromPoint : reinterpret_cast<p_NtUserWindowFromPoint>(GetProcAddress(GetModuleHandle("win32u.dll"), "NtUserWindowFromPoint"));
	if (!o_NtUserWindowFromPoint)
		bStatus = 18;
	o_NtUserGetForegroundWindow = o_NtUserGetForegroundWindow ? o_NtUserGetForegroundWindow : reinterpret_cast<p_NtUserGetForegroundWindow>(GetProcAddress(GetModuleHandle("win32u.dll"), "NtUserGetForegroundWindow"));
	if (!o_NtUserGetForegroundWindow)
		bStatus = 19;
	o_NtUserGetThreadState = o_NtUserGetThreadState ? o_NtUserGetThreadState : reinterpret_cast<p_NtUserGetThreadState>(GetProcAddress(GetModuleHandle("win32u.dll"), "NtUserGetThreadState"));
	if (!o_NtUserGetThreadState)
		bStatus = 20;
	o_NtOpenProcess = o_NtOpenProcess ? o_NtOpenProcess : reinterpret_cast<p_NtOpenProcess>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtOpenProcess"));
	if (!o_NtOpenProcess)
		bStatus = 21;
	o_NtQuerySystemInformation = o_NtQuerySystemInformation ? o_NtQuerySystemInformation : reinterpret_cast<p_NtQuerySystemInformation>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation"));
	if (!o_NtQuerySystemInformation)
		bStatus = 22;
	o_NtReadVirtualMemory = o_NtReadVirtualMemory ? o_NtReadVirtualMemory : reinterpret_cast<p_NtReadVirtualMemory>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtReadVirtualMemory"));
	if (!o_NtReadVirtualMemory)
		bStatus = 23;
	if (!o_NtOpenProcess ||
		!o_NtQuerySystemInformation ||
		!o_NtReadVirtualMemory ||
		!o_NtUserQueryWindow ||
		DetourTransactionBegin() != NO_ERROR ||
		DetourUpdateThread(GetCurrentThread()) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_EnumWindows, EnumWindows_Hook) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtUserQueryWindow, NtUserQueryWindow_Hook) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtUserFindWindowEx, NtUserFindWindowEx_Hook) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtUserWindowFromPoint, NtUserWindowFromPoint_Hook) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtUserGetForegroundWindow, NtUserGetForegroundWindow_Hook) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtUserGetThreadState, NtUserGetThreadState_Hook) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtOpenProcess, NtOpenProcess_Hook) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtQuerySystemInformation, NtQuerySystemInformation_Hook) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtReadVirtualMemory, NtReadVirtualMemory_Hook) != NO_ERROR ||
		DetourTransactionCommit() != NO_ERROR)
		if (!bStatus)
			bStatus = 23;
	VirtualizerEnd();
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::DISABLE_PROCESSDETECTION END %d", bStatus);
#endif
#endif

#if (DISABLE_ANTIDEBUGGER == 1)
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::DISABLE_ANTIDEBUGGER START %d", Status);
#endif
	o_NtSetInformationThread = o_NtSetInformationThread ? o_NtSetInformationThread : reinterpret_cast<p_NtSetInformationThread>(reinterpret_cast<DWORD_PTR>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtSetInformationThread")));
	if (!o_NtSetInformationThread)
		bStatus = 25;
	if (DetourTransactionBegin() != NO_ERROR ||
		DetourUpdateThread(GetCurrentThread()) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtSetInformationThread, NtSetInformationThread_Hook) != NO_ERROR ||
		DetourTransactionCommit() != NO_ERROR)
		if (!bStatus)
			bStatus = 26;
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::DISABLE_ANTIDEBUGGER END %d", bStatus);
#endif
#endif


#if (DISABLE_READFILE == 1)
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::DISABLE_READFILE START %d", Status);
#endif
	o_NtCreateFile = o_NtCreateFile ? o_NtCreateFile : reinterpret_cast<p_NtCreateFile>(reinterpret_cast<DWORD_PTR>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtCreateFile")));
	if (!o_NtCreateFile)
		bStatus = 30;
	o_Sleep = o_Sleep ? o_Sleep : reinterpret_cast<p_Sleep>(reinterpret_cast<DWORD_PTR>(GetProcAddress(GetModuleHandle("kernel32.dll"), "Sleep")));
	if (!o_Sleep)
		bStatus = 31;
	if (DetourTransactionBegin() != NO_ERROR ||
		DetourUpdateThread(GetCurrentThread()) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_NtCreateFile, NtCreateFile_Hook) != NO_ERROR ||
		(Status ? DetourAttach : DetourDetach)(&(PVOID&)o_Sleep, Sleep_Hook) != NO_ERROR ||
		DetourTransactionCommit() != NO_ERROR)
		if (!bStatus)
			bStatus = 32;
#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::DISABLE_READFILE END %d", bStatus);
#endif
#endif


#if (LOG_STATE == 1)
	DbgLog::Log("Bypass::detour_Functions END %d", bStatus);
#endif
	return bStatus ? false : true;
}

NTSTATUS NTAPI Bypass::NtSetInformationThread_Hook(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength) {
	VirtualizerStart();
	NTSTATUS Status = 0;
#if (LOG_NTSETINFORMATIONTHREAD == 1)
	DbgLog::Log("ThreadId: 0x%-8x, NtSetInformationThread - ThreadHandle: 0x%08X, Class: %d", GetCurrentThreadId(), ThreadHandle, ThreadInformationClass);
#endif
	if (ThreadInformationClass == ThreadHideFromDebugger) {
		// make sure the handle is valid
		// anti debug may send invalid handles to make sure its not patched to always return success
		if (WaitForSingleObject(ThreadHandle, 0) != WAIT_FAILED)
		{
			return STATUS_SUCCESS;
		}
	}
	Status = o_NtSetInformationThread(ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength);
	VirtualizerEnd();
	return Status;
}

NTSTATUS NTAPI Bypass::NtQueryVirtualMemory_Hook(HANDLE ProcessHandle, PVOID BaseAddress, int MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength)
{
	VirtualizerStart();
	auto IsValidHeader = [](PVOID BaseAddress)->BOOL
	{
		BYTE bd[3];
		ZeroMemory(bd, 3);
		if (!o_NtReadVirtualMemory(GetCurrentProcess(), reinterpret_cast<PVOID>(BaseAddress), &bd, 3, 0) &&
			bd[0] == 0x4D &&
			bd[1] == 0x5A &&
			bd[2] == 0x90)
			return true;
		return false;
	};

	PMEMORY_BASIC_INFORMATION pbi = reinterpret_cast<PMEMORY_BASIC_INFORMATION>(MemoryInformation);

	NTSTATUS Status = o_NtQueryVirtualMemory(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
#if (LOG_NTQUERYVIRTUALMEMORY == 1)
	DbgLog::Log("ThreadId: 0x%-8x, NtQueryVirtualMemory - BaseAddress: 0x%016llX", GetCurrentThreadId(), BaseAddress);
#endif
	// TerminateThread(GetCurrentThread(), 0);
	if (MemoryInformationClass == 0 && pbi)
	{
		pbi->AllocationProtect = PAGE_NOACCESS;
		pbi->RegionSize = 0;
		pbi->State = MEM_FREE;
		pbi->Protect = 0;
		pbi->Type = MEM_PRIVATE;
		return 0;
	}
	VirtualizerEnd();
	return Status;
};

NTSTATUS NTAPI Bypass::NtWow64QueryVirtualMemory64_Hook(HANDLE ProcessHandle, PVOID64 BaseAddress, int MemoryInformationClass, PVOID MemoryInformation, ULONGLONG MemoryInformationLength, PULONGLONG ReturnLength)
{
	VirtualizerStart();
	auto IsValidHeader = [](ULONGLONG BaseAddress)->BOOL
	{
		BYTE bd[3];
		ZeroMemory(bd, 3);
		if (!o_NtWow64ReadVirtualMemory64(GetCurrentProcess(), reinterpret_cast<PVOID64>(BaseAddress), &bd, 3, 0) &&
			bd[0] == 0x4D &&
			bd[1] == 0x5A &&
			bd[2] == 0x90)
			return true;
		return false;
	};
	PMEMORY_BASIC_INFORMATION64 pbi = reinterpret_cast<PMEMORY_BASIC_INFORMATION64>(MemoryInformation);

	NTSTATUS Status = 0;
#if (LOG_NTWOW64QUERYVIRTUALMEMORY64 == 1)
	DbgLog::Log("ThreadId: 0x%-8x, NtWow64QueryVirtualMemory64 - BaseAddress: 0x%08X", GetCurrentThreadId(), BaseAddress);
#endif
	if (MemoryInformationClass == 0 && pbi)
	{
		pbi->AllocationProtect = PAGE_NOACCESS;
		pbi->RegionSize = 0;
		pbi->State = MEM_FREE;
		pbi->Protect = 0;
		pbi->Type = MEM_PRIVATE;
		return 0;
	}
	VirtualizerEnd();
	if (o_NtWow64QueryVirtualMemory64)
		return o_NtWow64QueryVirtualMemory64(ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength);
	return 0;
};

NTSTATUS NTAPI Bypass::NtOpenProcess_Hook(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PVOID Inject_GameId)
{
	VirtualizerStart();

	typedef struct _CLIENT_ID {
		HANDLE UniqueProcess;
		HANDLE UniqueThread;
	} CLIENT_ID, *PCLIENT_ID;

	NTSTATUS Status = 0;
	DWORD ProcessId = 0;
	PCLIENT_ID Inject_GameID = reinterpret_cast<PCLIENT_ID>(Inject_GameId);

	if (Inject_GameID->UniqueProcess != reinterpret_cast<HANDLE>((ULONG64)GetCurrentProcessId()))
	{
#if (LOG_NTOPENPROCESS == 1)
		DbgLog::Log("ThreadId: 0x%-8x, NtOpenProcess - ProcessId: %d", GetCurrentThreadId(), Inject_GameID->UniqueProcess);
#endif
		*ProcessHandle = GetCurrentProcess();
		return STATUS_SUCCESS;
	}
	if (Inject_GameID->UniqueProcess == reinterpret_cast<HANDLE>((ULONG64)GetCurrentProcessId()))
	{
#if (LOG_NTOPENPROCESS == 1)
		DbgLog::Log("ThreadId: 0x%-8x, NtOpenProcess - ProcessId: %d", GetCurrentThreadId(), Inject_GameID->UniqueProcess);
#endif
	}

	VirtualizerEnd();
	Status = o_NtOpenProcess(ProcessHandle, DesiredAccess, ObjectAttributes, Inject_GameId);
	return Status;
};

BOOL CALLBACK Bypass::EnumWindows_Hook_EnumWindowsProc(HWND hWnd, LPARAM lParam) {

	DWORD_PTR dwProcessId = o_NtUserQueryWindow(hWnd, QUERY_WINDOW_UNIQUE_PROCESS_ID);
	if (dwProcessId != GetCurrentProcessId()) {
		// return TRUE; // 这里会校验回调数目
		hWnd = (HWND)-1;
	}
	return (*reinterpret_cast<WNDENUMPROC*>(reinterpret_cast<LPVOID*>(lParam)))
		(hWnd, *reinterpret_cast<LPARAM*>(reinterpret_cast<LPVOID*>(lParam) + 1));
}

BOOL WINAPI Bypass::EnumWindows_Hook(WNDENUMPROC lpEnumFunc, LPARAM lParam) {
	VirtualizerStart();
	BOOL ReturnStatus = false;

	LPVOID lParamTemp[2] = { 0 };
	*reinterpret_cast<WNDENUMPROC*>(lParamTemp) = lpEnumFunc;
	*reinterpret_cast<LPARAM*>(lParamTemp + 1) = lParam;

#if (LOG_ENUMWINDOWS == 1)
	DbgLog::Log("ThreadId: 0x%-8x, EnumWindows - Proc: %016llX", GetCurrentThreadId(), (DWORD_PTR)lpEnumFunc);
#endif
	ReturnStatus = o_EnumWindows(EnumWindows_Hook_EnumWindowsProc, (LPARAM)lParamTemp);
	VirtualizerEnd();
	return ReturnStatus;
}

DWORD_PTR NTAPI Bypass::NtUserQueryWindow_Hook(HWND hWnd, DWORD_PTR Index)
{
	VirtualizerStart();
	DWORD_PTR Result = 0;
	// 游戏会检测前台窗口的进程ID
	/*
	switch (Index)
	{
	case QUERY_WINDOW_UNIQUE_PROCESS_ID: // 0
	case QUERY_WINDOW_ACTIVE: // 2
		Result = (DWORD_PTR)GetCurrentProcessId();
		break;
	case QUERY_WINDOW_UNIQUE_THREAD_ID:
	case QUERY_WINDOW_FOCUS:
	case QUERY_WINDOW_ISHUNG:
	case QUERY_WINDOW_REAL_ID:
	case QUERY_WINDOW_FOREGROUND:
	default:
		Result = o_NtUserQueryWindow(hWnd, Index);
		break;
	}
	*/
#if (LOG_NTQUERYWINDOW == 1)
	DbgLog::Log("ThreadId: 0x%-8x, NtUserQueryWindow - hWnd: %d", GetCurrentThreadId(), hWnd);
#endif
	VirtualizerEnd();
	return Result;
};

HWND NTAPI Bypass::NtUserFindWindowEx_Hook(IN HWND hwndParent, IN HWND hwndChild, IN PUNICODE_STRING pstrClassName OPTIONAL, IN PUNICODE_STRING pstrWindowName OPTIONAL, IN DWORD dwType)
{
	VirtualizerStart();
	HWND Status = 0;
	Status = o_NtUserFindWindowEx(hwndParent, hwndChild, pstrClassName, pstrWindowName, dwType);
#if (LOG_NTUSERWINDOWFUNCTION == 1)
	DbgLog::Log("ThreadId: 0x%-8x, NtUserFindWindowEx - ClassName: %ls, WindowName: %ls, Result: %d", GetCurrentThreadId(),
		pstrClassName ? pstrClassName->Buffer : L"",
		pstrWindowName ? pstrWindowName->Buffer : L"",
		Status);
#endif
	VirtualizerEnd();
	return Status;
}

HWND APIENTRY Bypass::NtUserWindowFromPoint_Hook(LONG X, LONG Y)
{
	VirtualizerStart();
	HWND Status = 0;
	Status = o_NtUserWindowFromPoint(X, Y);
#if (LOG_NTUSERWINDOWFUNCTION == 1)
	DbgLog::Log("ThreadId: 0x%-8x, NtUserWindowFromPoint - X: %d, Y: %d, Result: %d", GetCurrentThreadId(), X, Y, Status);
#endif
	VirtualizerEnd();
	return Status;
}

HWND APIENTRY Bypass::NtUserGetForegroundWindow_Hook()
{
	VirtualizerStart();
	HWND Status = 0;
	Status = o_NtUserGetForegroundWindow();
#if (LOG_NTUSERWINDOWFUNCTION == 1)
	DbgLog::Log("ThreadId: 0x%-8x, NtUserGetForegroundWindow - Result: %d", GetCurrentThreadId(), Status);
#endif
	VirtualizerEnd();
	return Status;
}

DWORD_PTR APIENTRY Bypass::NtUserGetThreadState_Hook(USERTHREADSTATECLASS ThreadState)
{
	VirtualizerStart();
	DWORD_PTR Status = 0;
	Status = o_NtUserGetThreadState(ThreadState);
#if (LOG_NTUSERWINDOWFUNCTION == 1)
	DbgLog::Log("ThreadId: 0x%-8x, NtUserGetThreadState - Class: %d, Result: %d", GetCurrentThreadId(), ThreadState, Status);
#endif
	VirtualizerEnd();
	return Status;
}

NTSTATUS NTAPI Bypass::NtQuerySystemInformation_Hook(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
{
	VirtualizerStart();
	typedef struct _MY_SYSTEM_PROCESS_INFORMATION
	{
		ULONG                   NextEntryOffset;
		ULONG                   NumberOfThreads;
		LARGE_INTEGER           Reserved[3];
		LARGE_INTEGER           CreateTime;
		LARGE_INTEGER           UserTime;
		LARGE_INTEGER           KernelTime;
		UNICODE_STRING          ImageName;
		ULONG                   BasePriority;
		HANDLE                  ProcessId;
		HANDLE                  InheritedFromProcessId;
	} MY_SYSTEM_PROCESS_INFORMATION, *PMY_SYSTEM_PROCESS_INFORMATION;

	typedef struct _SYSTEM_HANDLE
	{
		ULONG ProcessId;
		BYTE ObjectTypeNumber;
		BYTE Flags;
		USHORT Handle;
		PVOID Object;
		ACCESS_MASK GrantedAccess;
	} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

	typedef struct _SYSTEM_HANDLE_INFORMATION
	{
		ULONG HandleCount;
		SYSTEM_HANDLE Handles[1];
	} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

	DWORD Old = 0;
	NTSTATUS Status = -1;
	PMY_SYSTEM_PROCESS_INFORMATION ppi = 0;
	PSYSTEM_HANDLE_INFORMATION psi = 0;

#if (LOG_NTQUERYSYSTEMINFORMATION == 1)
	DbgLog::Log("ThreadId: 0x%-8x, NtQuerySystemInformation - Class: %d", GetCurrentThreadId(), SystemInformationClass);
#endif
	switch (SystemInformationClass)
	{
	case SystemProcessInformation: // 5
	case SystemHandleInformation:  // 16
		*ReturnLength = 0;
		return STATUS_SUCCESS;
	}

	Status = o_NtQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
	if (NT_SUCCESS(Status))
	{
		switch (SystemInformationClass)
		{
		case SystemBasicInformation: // 0
			break;
		case SystemProcessorInformation: // 1
			break;
		case SystemProcessInformation: // 5
			break;
		case SystemHandleInformation:  // 16
			// scan process for the handle to game
			psi = reinterpret_cast<PSYSTEM_HANDLE_INFORMATION>(SystemInformation);
			if (psi)
			{
				for (unsigned int i = 0; i < psi->HandleCount; i++)
				{
					psi->Handles[i].GrantedAccess = 0;
					psi->Handles[i].ProcessId = GetCurrentProcessId();
					psi->Handles[i].Flags = 0;
					psi->Handles[i].Handle = 0;
					psi->Handles[i].Object = 0;
					psi->Handles[i].ObjectTypeNumber = 0;
				}
			}
			break;
		case SystemBasicPerformanceInformation: // 123
			break;
		case SystemPolicyInformation: // 134
			break;
		default:
			break;
		}
	}
	VirtualizerEnd();
	return Status;
}

NTSTATUS NTAPI Bypass::NtReadVirtualMemory_Hook(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesReaded)
{
	VirtualizerStart();
	CloseHandle(ProcessHandle);
	VirtualizerEnd();
	return true;
}

NTSTATUS NTAPI Bypass::NtGetContextThread_Hook(HANDLE ThreadHandle, PCONTEXT pContext)
{
	VirtualizerStart();
	HMODULE wModule = 0;
	NTSTATUS Status = o_NtGetContextThread(ThreadHandle, pContext);
	if (NT_SUCCESS(Status) && pContext)
	{
#if (LOG_NTGETCONTEXTTHREAD == 1)
		DbgLog::Log("ThreadId: 0x%-8x, NtGetContextThread - ThreadId: 0x%x, Rip: 0x%x", GetCurrentThreadId(), GetThreadId(ThreadHandle), pContext->Rip);
#endif
		if (pContext->Dr0)
			pContext->Dr0 = 0;
		if (pContext->Dr1)
			pContext->Dr1 = 0;
		if (pContext->Dr2)
			pContext->Dr2 = 0;
		if (pContext->Dr3)
			pContext->Dr3 = 0;
		if (pContext->Dr7)
			pContext->Dr7 = 0;
	}
	VirtualizerEnd();
	return Status;
}

NTSTATUS NTAPI Bypass::NtOpenThread_Hook(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PVOID Inject_GameId)
{
	VirtualizerStart();
	typedef struct _CLIENT_ID {
		HANDLE UniqueProcess;
		HANDLE UniqueThread;
	} CLIENT_ID, *PCLIENT_ID;

	HMODULE wModule = 0;
	PCLIENT_ID Inject_GameID = reinterpret_cast<PCLIENT_ID>(Inject_GameId);
#if (LOG_NTOPENTHREAD == 1)
	DbgLog::Log("ThreadId: 0x%-8x, NtOpenThread - ThreadId: 0x%x", GetCurrentThreadId(), Inject_GameID->UniqueThread);
#endif
	DesiredAccess = THREAD_TERMINATE;
	VirtualizerEnd();
	return o_NtOpenThread(ThreadHandle, DesiredAccess, ObjectAttributes, Inject_GameId);
}

NTSTATUS NTAPI Bypass::NtCreateFile_Hook(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength)
{
	VirtualizerStart();
	DWORD dwWritten = 0;
	NTSTATUS Status = -1;
	if (ObjectAttributes &&
		ObjectAttributes->ObjectName &&
		ObjectAttributes->ObjectName->Buffer)
	{
#if (LOG_NTCREATEFILE == 1)
		DbgLog::Log("ThreadId: 0x%-8x, NtCreateFile - File: %ls", GetCurrentThreadId(), ObjectAttributes->ObjectName->Buffer);
#endif
		if (wcsstr(ObjectAttributes->ObjectName->Buffer, L"Bypass")) {
			o_RtlInitUnicodeString(ObjectAttributes->ObjectName, L"");
			NTSTATUS Status = o_NtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
			return Status;
		}

	}
	VirtualizerEnd();
	return o_NtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);

}

VOID __stdcall Bypass::Sleep_Hook(DWORD dwMilliseconds)
{
	if (dwMilliseconds == 500) {
		dwMilliseconds = 5000;
	}
	DbgLog::Log("ThreadId: 0x%-8x, Sleep - dwMilliseconds: %d", GetCurrentThreadId(), dwMilliseconds);
	return o_Sleep(dwMilliseconds);
}


#pragma endregion
