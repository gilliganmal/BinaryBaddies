#include "../include/sHELL.h"
#include <windows.h>
#include <tlhelp32.h>

const char Name[] = "ps";
const char Help[] = "List all running processes with their IDs and names.";

InternalAPI *core = NULL;

LPVOID lpOut = NULL;
__declspec(dllexport) VOID CommandCleanup() {
    // No cleanup needed for this command
}

// Function to list all running processes
void ListProcesses() {
    HANDLE hSnapshot;
    PROCESSENTRY32 pe32;

    // Create a snapshot of the current processes
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        core->wprintf(L"Error creating process snapshot\n");
        return;
    }

    // Set the size of the structure before using it
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process
    if (!Process32First(hSnapshot, &pe32)) {
        core->wprintf(L"Error retrieving process information\n");
        CloseHandle(hSnapshot);
        return;
    }

    // Print process information
    core->wprintf(L"PID\t\tName\n");
    do {
        core->wprintf(L"%d\t\t%S\n", pe32.th32ProcessID, pe32.szExeFile);
    } while (Process32Next(hSnapshot, &pe32));

    // Close the handle to the snapshot
    CloseHandle(hSnapshot);
}

// initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) {
    core = lpCore;
    return TRUE;
}

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
    ListProcesses();
    return NULL;
}

// Entrypoint for the DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
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
