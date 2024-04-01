
#include <windows.h>

#include "../include/opcodes.h"
#include "../include/sHELL.h"

#include "../include/readf.h"

const char Name[] = "cat";
const char Help[] = "Concatenate and print files to the standard output.\n"
                    "Usage:\n"
                    "    cat <file1> <file2> ...\n";

// declue dependenceies
// always ensure null terminated
// this command depends on readf

#define INDEX_cat_readf 0

// note that order of dependencies matters
CommandDependency deps[] = {DECLARE_DEP(OPCODE_readf), DECLARE_DEP(0)};
InternalAPI *core = NULL;

LPVOID lpOut = NULL;
CommandA *readf = NULL;

char **FileNameToArgv(int *argc, char *lpFileName) {

  char fmt[] = "readf %s";
  // TODO migrate to macro
  char *argvBuff =
      (char *)core->malloc(core->strlen(lpFileName) + core->strlen(fmt) - 1);
  core->sprintf(argvBuff, fmt, lpFileName);
  char **readfArgv = core->CommandLineToArgvA(argvBuff, argc);
  // free templated string
  core->free(argvBuff);
  return readfArgv;
}

__declspec(dllexport) VOID CommandCleanup() {

  for (int i = 0; deps[i].lpCmd != NULL; i++) {
    debug_wprintf(L"Cleaning up %i:%lu\n", i, deps[i].hash);
    deps[i].lpCmd->fnCleanup();
  }
  if (lpOut) {
    core->free(lpOut);
    lpOut = NULL;
  }
}

// Initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) {
  core = lpCore;
  debug_wprintf(L"[+] Initializing %S\n", Name);
  if (!core->ResolveCommandDependnecies(deps)) {
    core->wprintf(L"Dependency failed!\n");
    return FALSE;
  }
  debug_wprintf(L"Setting readf to lpCMD...\n");
  readf = deps[INDEX_cat_readf].lpCmd;
  debug_wprintf(L"Found readf: %p\n", (void *)readf);
  return TRUE;
}

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

#define DEBUG
// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
    if (argc < 2) {
        core->wprintf(L"Invalid arguments.\n%S", CommandHelpA());
        return NULL; // Error code for invalid arguments
    }

    // Allocate memory to hold the concatenated file contents
    size_t totalSize = 0;
    for (int i = 1; i < argc; i++) {
        int readfArgc;
        char **readfArgv = FileNameToArgv(&readfArgc, argv[i]);
        LPVOID readfOut = readf->fnRunA(readfArgc, readfArgv);
        if (readfOut) {
            CommandOut_readf *readfResult = (CommandOut_readf *)readfOut;
            // Print the file contents to the standard output
            core->printf("%s", readfResult->lpBuffer);
            totalSize += readfResult->qwFileSize.QuadPart;
            core->free(readfResult->lpBuffer);
            core->free(readfResult);
        }
        core->free(readfArgv);
    }

    if (totalSize == 0) {
        core->wprintf(L"No files to concatenate.\n");
        return NULL;
    }

    // Allocate memory to hold the concatenated file contents
    char *concatenatedContents = (char *)core->malloc(totalSize + 1);
    if (!concatenatedContents) {
        core->wprintf(L"Memory allocation failed.\n");
        return NULL;
    }
    core->memset(concatenatedContents, 0, totalSize + 1);

    return concatenatedContents;
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
