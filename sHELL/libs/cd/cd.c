#include "../include/sHELL.h"
#include <windows.h>

const char Name[] = "cd";
const char Help[] = "Change the shell's current directory.\n"
                    "Usage:\n"
                    "    cd <path>";

InternalAPI *core = NULL;

LPVOID lpOut = NULL;
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
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
  if (argc != 2) {
        core->wprintf(L"Usage: %S\n", Help);
        return NULL;
    }

    // Attempt to change the current working directory
    if (!SetCurrentDirectoryA(argv[1])) {
        core->wprintf(L"Failed to change directory to '%S'. Error code: %d\n", argv[1], GetLastError());
        return NULL; // Failure
    }

    return (LPVOID)1; // Success
}

// Entrypoint for the DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    break;
  }
  return TRUE; // Successful
}
