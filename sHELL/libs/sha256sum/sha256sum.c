#include "../include/sHELL.h"
#include <windows.h>
#include <bcrypt.h>

const char Name[] = "sha256sum";
const char Help[] = "Calculate the SHA-256 hash of a string. Example:"
                    ">>>sha256sum string";

InternalAPI *core = NULL;

__declspec(dllexport) VOID CommandCleanup() {
    // No cleanup needed for this command
}

__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) {
    core = lpCore;
    return TRUE;
}

__declspec(dllexport) const char *CommandNameA() { return Name; }

__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Function to calculate SHA-256 hash of a string
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
    // Example implementation: calculate SHA-256 hash of a string
    if (argc != 2) {
        core->wprintf(L"Usage: sha256sum <string>\n");
        return NULL;
    }

    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    DWORD cbData = 0,
          cbHash = 0,
          cbHashObject = 0;
    PBYTE pbHashObject = NULL,
          pbHash = NULL;
    NTSTATUS status;

    // Open an algorithm handle
    status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0);
    if (!BCRYPT_SUCCESS(status)) {
        core->wprintf(L"Error: BCryptOpenAlgorithmProvider failed with status 0x%x\n", status);
        goto cleanup;
    }

    // Calculate the size of the buffer to hold the hash object
    status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD), &cbData, 0);
    if (!BCRYPT_SUCCESS(status)) {
        core->wprintf(L"Error: BCryptGetProperty failed with status 0x%x\n", status);
        goto cleanup;
    }

    // Allocate memory for the hash object
    pbHashObject = (PBYTE)core->malloc(cbHashObject);
    if (pbHashObject == NULL) {
        core->wprintf(L"Error: Memory allocation failed\n");
        goto cleanup;
    }

    // Create a hash object
    status = BCryptCreateHash(hAlg, &hHash, pbHashObject, cbHashObject, NULL, 0, 0);
    if (!BCRYPT_SUCCESS(status)) {
        core->wprintf(L"Error: BCryptCreateHash failed with status 0x%x\n", status);
        goto cleanup;
    }

    // Hash the input string
    status = BCryptHashData(hHash, (PBYTE)argv[1], (ULONG)lstrlenA(argv[1]), 0);
    if (!BCRYPT_SUCCESS(status)) {
        core->wprintf(L"Error: BCryptHashData failed with status 0x%x\n", status);
        goto cleanup;
    }

    // Calculate the size of the hash
    status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&cbHash, sizeof(DWORD), &cbData, 0);
    if (!BCRYPT_SUCCESS(status)) {
        core->wprintf(L"Error: BCryptGetProperty failed with status 0x%x\n", status);
        goto cleanup;
    }

    // Allocate memory for the hash
    pbHash = (PBYTE)core->malloc(cbHash);
    if (pbHash == NULL) {
        core->wprintf(L"Error: Memory allocation failed\n");
        goto cleanup;
    }

    // Retrieve the hash
    status = BCryptFinishHash(hHash, pbHash, cbHash, 0);
    if (!BCRYPT_SUCCESS(status)) {
        core->wprintf(L"Error: BCryptFinishHash failed with status 0x%x\n", status);
        goto cleanup;
    }

    // Print the hash
    core->wprintf(L"SHA-256 hash of '%S': ", argv[1]);
    for (DWORD i = 0; i < cbHash; i++) {
        core->wprintf(L"%02x", pbHash[i]);
    }
    core->wprintf(L"\n");

cleanup:
    if (pbHash) {
        core->free(pbHash);
    }
    if (hHash) {
        BCryptDestroyHash(hHash);
    }
    if (pbHashObject) {
        core->free(pbHashObject);
    }
    if (hAlg) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
    }
    return NULL;
}

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
