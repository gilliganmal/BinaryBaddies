#include "readf.h"

#define DEBUG

const char Name[] = "readf";
const char Help[] =
    "Read a file into memory, and print its VirtualAddress and file size\n"
    "It makes no assumptions on the file size. I.e., >4GB is possibe"
    "output here is a pointer to a struct that contains "
    "Example:\n"
    "    >>>readf file.txt"
    "    >>>file.txt has been loaded at %p and is of size %llu";

InternalAPI *core = NULL;
CommandOut_readf *lpOut = NULL;

__declspec(dllexport) VOID CommandCleanup() {
  if (lpOut) {
    if (lpOut->lpBuffer) {
      debug_wprintf(L"Freeing read buffer\n");
      core->free(lpOut->lpBuffer);
    }
    debug_wprintf(L"freeing CommandOut_readf struct\n");
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

BOOL LoadFileA(char *filePath) {
    HANDLE hFile = INVALID_HANDLE_VALUE;
    LARGE_INTEGER fileSize;
    
    // Open the file for reading
    hFile = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        debug_wprintf(L"Failed to open file %s\n", filePath);
        return FALSE;
    }

    // Get the size of the file
    if (!GetFileSizeEx(hFile, &fileSize)) {
        debug_wprintf(L"Failed to get file size for %s\n", filePath);
        CloseHandle(hFile);
        return FALSE;
    }

    // Allocate memory to hold the file contents
    lpOut->lpBuffer = core->realloc(lpOut->lpBuffer, fileSize.QuadPart);
    if (!lpOut->lpBuffer) {
        debug_wprintf(L"Failed to allocate memory for file contents\n");
        CloseHandle(hFile);
        return FALSE;
    }

    // Read the file contents into memory
    DWORD bytesRead;
    if (!ReadFile(hFile, lpOut->lpBuffer, (DWORD)fileSize.QuadPart, &bytesRead, NULL)) {
        debug_wprintf(L"Failed to read file %s\n", filePath);
        CloseHandle(hFile);
        return FALSE;
    }

    // Close the file handle
    CloseHandle(hFile);

    // Set the file size in the output struct
    lpOut->qwFileSize = fileSize;

    return TRUE;
}


// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
  // Example implementation: print arguments and return count
  if (argc != 2) {
    core->wprintf(L"Invalid args: %S\n", Help);
    return NULL;
  }

  if (lpOut == NULL) {
    debug_wprintf(L"readf-> initializing lpOut\n");
    // only allocate a new out struct if it is null
    // this allows us to reuse the same one for multiple files
    // HINT for cat:...^
    lpOut = core->malloc(sizeof(CommandOut_readf));
    core->memset(lpOut, 0, sizeof(CommandOut_readf));
  }
  if (LoadFileA(argv[1])) {
    core->wprintf(L"File %S was loaded into VA %p of size %llu\n", argv[1],
                  lpOut->lpBuffer, lpOut->qwFileSize.QuadPart);
    return (LPVOID)lpOut;
  }
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
