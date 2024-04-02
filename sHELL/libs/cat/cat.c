#include <windows.h>

#include "../include/opcodes.h"
#include "../include/sHELL.h"
#include <shellapi.h>

#include "../include/readf.h"

const char Name[] = "cat";
const char Help[] = "Concatenate and print files to the standard output.\n"
                    "Usage:\n"
                    "    cat <file1> <file2> ...\n";

// Declare dependencies
#define INDEX_cat_readf 0
CommandDependency deps[] = {DECLARE_DEP(OPCODE_readf), DECLARE_DEP(0)};
InternalAPI *core = NULL;
CommandA *readf = NULL;

// Function to free memory allocated for command output
void FreeCommandOut(LPVOID lpCmdOut) {
    CommandOut_readf *out = (CommandOut_readf *)lpCmdOut;
    if (out) {
        if (out->lpBuffer) {
            core->free(out->lpBuffer);
        }
        core->free(out);
    }
}

// Function to execute the readf command
LPVOID ExecuteReadf(int argc, char **argv) {
    return readf->fnRun(argc, argv);
}

// Function to read the contents of a file and print them
BOOL PrintFileContents(const char *fileName) {
    int argc;
    LPSTR *argvA = CommandLineToArgvA(GetCommandLineA(), &argc);
    if (!argvA) {
        core->wprintf(L"Failed to parse command line arguments.\n");
        return FALSE;
    }

    // Execute readf command to read the file contents
    LPVOID readfOut = ExecuteReadf(argc, argvA);
    LocalFree(argvA); // Free memory allocated for command line arguments

    if (!readfOut) {
        core->wprintf(L"Failed to read file: %S\n", fileName);
        return FALSE;
    }

    // Cast readf output to CommandOut_readf structure
    CommandOut_readf *out = (CommandOut_readf *)readfOut;

    // Get standard output handle
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) {
        core->wprintf(L"Failed to get standard output handle.\n");
        return FALSE;
    }

    // Write file contents to standard output
    DWORD bytesWritten;
    if (!WriteFile(hStdOut, out->lpBuffer, out->qwFileSize.QuadPart, &bytesWritten, NULL)) {
        core->wprintf(L"Failed to write file contents to standard output.\n");
        FreeCommandOut(readfOut);
        return FALSE;
    }

    // Free memory allocated for readf output
    FreeCommandOut(readfOut);

    return TRUE;
}



// Command cleanup
__declspec(dllexport) VOID CommandCleanup() {
    for (int i = 0; deps[i].lpCmd != NULL; i++) {
        deps[i].lpCmd->fnCleanup();
    }
}

// Initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) {
    core = lpCore;
    if (!core->ResolveCommandDependnecies(deps)) {
        core->wprintf(L"Dependency failed!\n");
        return FALSE;
    }
    readf = deps[INDEX_cat_readf].lpCmd;
    return TRUE;
}

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
    if (argc < 2) {
        core->wprintf(L"Invalid arguments.\n%S", CommandHelpA());
        return NULL; // Error code for invalid arguments
    }

    // Print contents of each file
    for (int i = 1; i < argc; i++) {
        if (!PrintFileContents(argv[i])) {
            return NULL; // Error reading file
        }
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

