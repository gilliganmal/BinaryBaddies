
#include "../include/sHELL.h"
#include <windows.h>

const char Name[] = "help";
const char Help[] =
    "Print help messages for commands.\n"
    "Passed with no command names, it prints all avaiable commands help message"
    ">>>help echo"
    "echo a string back to the terminal. Example:...\n";

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
  // Example implementation: print arguments and return count
  // // your answer here
  const char *command = (const char*) argv[1];
  for(int i=0; i< *(core->gModuleCount); i++){
    //debug_wprintf(L"commandname is %S\n", core->gaCommandsA[i].fnName());
    if(core->strcmp(core->gaCommandsA[i].fnName(), command) == 0){
      //debug_wprintf(L"command is a match %S : %S \n", core->gaCommandsA[i].fnName(), command);
      /*print help*/
      core->wprintf(L"%S\n", core->gaCommandsA[i].fnHelp());
      return lpOut;
    }
  }
  debug_wprintf(L"command not found \n");
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
