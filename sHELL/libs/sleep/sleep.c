

#include "../include/sHELL.h"
#include <shlwapi.h>
#include <windows.h>

const char Name[] = "sleep";
const char Help[] = "Pause execution (sleep) for the current thread."
                    "Example Sleeping for 1 second:"
                    ">>>sleep 1000 "
                    ">>>";

InternalAPI *core = NULL;

LPVOID lpOut = NULL;
__declspec(dllexport) VOID CommandCleanup() {
  if (lpOut) {
    core->free(lpOut);
    lpOut = NULL;
  }
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
  if (argc != 2) {
    core->wprintf(L"Invalid input:\n %S\n", Help);
    return NULL;
  }

  //need to parse sleep duration command string into integer
  //implemented sign changes for future possible requirments(not required)
  int custom_parsing_atoi(const char *str) {
    int result = 0;
    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    }
    while (*str != '\0') {
        if (*str >= '0' && *str <= '9') {
            result = result * 10 + (*str - '0');
        } else {
            // Handle invalid characters
            return 0;
        }
        str++;
    }
    return sign * result;
}
  
  int duration = custom_parsing_atoi(argv[1]);
  if (duration <= 0) {
    core->wprintf(L"Invalid duration specified.\n");
    return NULL;
  }
  
  // Sleep for the specified duration
  Sleep(duration);
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
