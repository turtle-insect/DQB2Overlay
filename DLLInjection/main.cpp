#include <vector>
#include <Windows.h>
#include <shlwapi.h>
#include <Psapi.h>

typedef std::vector<const TCHAR*> vecTchar;

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "psapi.lib")

void GetProcessName(DWORD processID, TCHAR* name, DWORD name_size)
{
    HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (handle == nullptr) return;
    
    HMODULE module;
    DWORD cbNeeded;
    if (EnumProcessModulesEx(handle, &module, sizeof(module), &cbNeeded, LIST_MODULES_64BIT))
    {
        GetModuleBaseName(handle, module, name, name_size);
    }
    CloseHandle(handle);
}

DWORD FindProcessID(vecTchar& find_names)
{
    DWORD process[1024];
    DWORD cbNeeded;
    if (! EnumProcesses(process, sizeof(process), &cbNeeded))
    {
        return 0;
    }

    DWORD size = cbNeeded / sizeof(DWORD);
    for (DWORD index = 0; index < size; index++)
    {
        DWORD processID = process[index];
        if (processID == 0) continue;

        TCHAR name[MAX_PATH]{ 0 };
        GetProcessName(processID, name, MAX_PATH);
        for (auto find_name : find_names)
        {
            if (ua_lstrcmp(name, find_name) == 0) return processID;
        }
    }

    return 0;
}

FARPROC GetLoadLibraryAddress()
{
    FARPROC address = nullptr;

    HMODULE module = GetModuleHandle(TEXT("kernel32.dll"));
    if (module)
    {
        address = GetProcAddress(module, "LoadLibraryW");
    }
    
    return address;
}

BOOL DLLInjection(DWORD processID)
{
    BOOL isInjection = FALSE;
    HANDLE handle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, processID);
    if (handle == nullptr) return isInjection;

    TCHAR current_path[MAX_PATH] = { 0 };
    GetCurrentDirectory(MAX_PATH, current_path);
    TCHAR dll_file[MAX_PATH] = { 0 };
    PathCombine(dll_file, current_path, TEXT("DQB2Overlay.dll"));

    LPVOID address = VirtualAllocEx(handle, nullptr, sizeof(dll_file), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (address)
    {
        SIZE_T written;
        if(
            WriteProcessMemory(handle, address, dll_file, sizeof(dll_file), &written) &&
            CreateRemoteThread(handle, nullptr, 0, (LPTHREAD_START_ROUTINE)GetLoadLibraryAddress(), address, 0, nullptr)
            )
        {
            isInjection = TRUE;
            //VirtualFreeEx(handle, address, 0, MEM_RELEASE);
        }
    }
    
    CloseHandle(handle);
    return isInjection;
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
    vecTchar find_names = { TEXT("DQB2.exe"), TEXT("DQB2_EU.exe"), TEXT("DQB2_AS.exe") };

    for (;;)
    {
        DWORD processID = FindProcessID(find_names);
        if (processID)
        {
            if (DLLInjection(processID))
            {
                break;
            }
        }
        Sleep(2000);
    }

    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    HANDLE handle = CreateThread(nullptr, 0, MainThread, hInstance, 0, nullptr);
    if (handle == nullptr) return 0;

    WaitForSingleObject(handle, INFINITE);
    return 0;
}
