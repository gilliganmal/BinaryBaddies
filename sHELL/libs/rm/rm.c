#include "../include/sHELL.h"
#include <windows.h>

const char Name[] = "rm";
const char Help[] = "Delete a file. Example: >>>rm file.txt";

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
    if (argc != 2) {
        core->wprintf(L"Usage: %S\n", Help);
        return NULL;
    }

    // Attempt to delete the file
    if (DeleteFileA(argv[1])) {
        core->wprintf(L"File '%S' deleted successfully.\n", argv[1]);
        return (LPVOID)1; // Success
    } else {
        core->wprintf(L"Failed to delete file '%S'. Error code: %d\n", argv[1], GetLastError());
        return NULL; // Failure
    }
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
