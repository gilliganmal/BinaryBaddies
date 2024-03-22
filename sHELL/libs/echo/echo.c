#include "../include/sHELL.h"
#include <windows.h>

const char Name[] = "echo";
const char Help[] = "echo a string back to the terminal. Example:"
                    ">>>echo asdf"
                    ">>>asdf";

LPVOID lpOut = NULL;

void lazy_print(char *szArg) {
  HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD written;
  WriteFile(stdOut, szArg, lstrlenA(szArg), &written, NULL);
}

InternalAPI *core = NULL;

__declspec(dllexport) VOID CommandCleanup() {
  if (lpOut) {
    core->free(lpOut);
    lpOut = NULL;
  }
}

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
  for (int i = 0; i < argc; i++) {
    core->wprintf(L"%S\n", argv[i]);
  }

  // Print the second argument if there are exactly 2 arguments
  if (argc == 2) {
    HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD written;
    WriteFile(stdOut, argv[1], lstrlenA(argv[1]), &written, NULL);
  }

  return (LPVOID)argc;
}



// Optional: Entry point for DLL
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
