#include "../include/sHELL.h"
#include <windows.h>

const char Name[] = "exec";
const char Help[] = "Execute a command. Example: >>>exec notepad.exe";

InternalAPI *core = NULL;

LPVOID lpOut = NULL;

void *memset(void *dest, int c, size_t n) {
    char *p = dest;
    while (n--) {
        *p++ = c;
    }
    return dest;
}

__declspec(dllexport) VOID CommandCleanup() {
    if (lpOut) {
        core->free(lpOut);
        lpOut = NULL;
    }
}

// Initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) {
    core = lpCore;
    return TRUE;
}

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { 
    return Name; 
}

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { 
    return Help; 
}

// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
    if (argc < 2) {
        core->wprintf(L"Usage: exec <command> [args]\n");
        return NULL;
    }

    // Convert command line arguments to a single string
    LPWSTR lpCommandLine = GetCommandLineW();

    // Create process
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessW(NULL,   // No module name (use command line)
        lpCommandLine,          // Command line
        NULL,                   // Process handle not inheritable
        NULL,                   // Thread handle not inheritable
        FALSE,                  // Set handle inheritance to FALSE
        0,                      // No creation flags
        NULL,                   // Use parent's environment block
        NULL,                   // Use parent's starting directory 
        &si,                    // Pointer to STARTUPINFO structure
        &pi))                   // Pointer to PROCESS_INFORMATION structure
    {
        core->wprintf(L"CreateProcess failed (%lu).\n", GetLastError());
        return NULL;
    }

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (LPVOID)1;
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
