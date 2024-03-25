#include "../include/sHELL.h"
#include <shlwapi.h>
#include <windows.h>


const char Name[] = "pkill";
const char Help[] = "Terminate a process by its ID. Example:"
                    ">>>pkill <process_id>";

InternalAPI *core = NULL;

__declspec(dllexport) VOID CommandCleanup() {
    // No cleanup needed for this command
}

__declspec(dllexport) VOID CmmandCleanup() {
    // No cleanup needed for this command
}

__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) {
    core = lpCore;
    return TRUE;
}

__declspec(dllexport) const char *CommandNameA() { return Name; }

__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Function to kill a process by ID
BOOL ProcessKill(DWORD dwProcessId) {
    HANDLE hProcess;

    // Open the process
    hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
    if (hProcess == NULL) {
        core->wprintf(L"Error opening process: %d\n", GetLastError());
        return FALSE;
    }

    // Terminate the process
    if (!TerminateProcess(hProcess, 0)) {
        core->wprintf(L"Error terminating process: %d\n", GetLastError());
        CloseHandle(hProcess);
        return FALSE;
    }

    // Close the handle to the process
    CloseHandle(hProcess);
    return TRUE;
}

__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
    // Example implementation: print arguments and return count
    if (argc != 2) {
        core->wprintf(L"Usage: pkill <process_id>\n");
        return NULL;
    }

    DWORD dwProcessId = StrToIntA(argv[1]);
    if (dwProcessId == 0) {
        core->wprintf(L"Invalid process ID\n");
        return NULL;
    }

    if (ProcessKill(dwProcessId)) {
        core->wprintf(L"Process with ID %d terminated successfully\n", dwProcessId);
        return (LPVOID)TRUE;
    } else {
        core->wprintf(L"Failed to terminate process with ID %d\n", dwProcessId);
        return (LPVOID)FALSE;
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    // Entry point for the DLL
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        // Code to run when the DLL is loaded
        break;
    case DLL_PROCESS_DETACH:
        // Code to run when the DLL is unloaded
        break;
    case DLL_THREAD_ATTACH:
        // Code to run when a thread is created during DLL's existence
        break;
    case DLL_THREAD_DETACH:
        // Code to run when a thread ends normally
        break;
    }
    return TRUE; // Successful
}
