#include "../include/sHELL.h"
#include <windows.h>

const char Name[] = "exec";
const char Help[] = "Execute a command. Example: >>>exec notepad.exe";

InternalAPI *core = NULL;

// Command cleanup
__declspec(dllexport) VOID CommandCleanup() {
    // No cleanup needed for this command
}

// Initialization code
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
    // Example implementation: print arguments and return count
    if (argc < 2) {
        core->wprintf(L"Usage: %S\n", Help);
        return NULL;
    }

    // Concatenate all arguments into a single command string
    char commandLine[4096] = "";
    for (int i = 1; i < argc; ++i) {
        if (i > 1) {
            lstrcatA(commandLine, " ");
        }
        lstrcatA(commandLine, argv[i]);
    }

    // Create process
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    int check = CreateProcessA(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (!check) {
        core->wprintf(L"Failed to execute command '%S'. Error code: %d\n", commandLine, GetLastError());
        return NULL;
    }

    // Wait for the process to exit
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (LPVOID)1; // Success
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
