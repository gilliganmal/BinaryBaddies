#include <windows.h>
#include "../include/sHELL.h"

const char Name[] = "b64";
const char Help[] =
    "Base64 encode or decode input.\n"
    "Usage:\n"
    "    b64 encode <input> - Encodes the input string\n"
    "    b64 decode <input> - Decodes the base64 encoded string";

InternalAPI *core = NULL;

LPVOID lpOut = NULL;

// Initialization code
__declspec(dllexport) BOOL CommandInit(InternalAPI *lpCore) { 
    core = lpCore; 
    return TRUE; // Initialization successful
}

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Cleanup
__declspec(dllexport) VOID CommandCleanup() {
  if (lpOut) {
    core->free(lpOut);
    lpOut = NULL;
  }
}

// Utility function to encode a string to base64
BOOL Base64Encode(const char *input, char **output, DWORD *outputSize) {
  DWORD cbData = lstrlenA(input);
  DWORD cchEncoded = 0;

  // Calculate the size of the encoded string
  if (!CryptBinaryToStringA((const BYTE*)input, cbData, CRYPT_STRING_BASE64, NULL, &cchEncoded))
    return FALSE;

  // Allocate memory for the encoded string
  *output = (char*)core->malloc(cchEncoded);
  if (!*output)
    return FALSE;

  // Encode the string
  if (!CryptBinaryToStringA((const BYTE*)input, cbData, CRYPT_STRING_BASE64, *output, &cchEncoded)) {
    core->free(*output);
    return FALSE;
  }

  *outputSize = cchEncoded;
  return TRUE;
}

// Utility function to decode a base64 string
BOOL Base64Decode(const char *input, BYTE **output, DWORD *outputSize) {
  DWORD cchData = lstrlenA(input);
  DWORD cbDecoded = 0;

  // Calculate the size of the decoded data
  if (!CryptStringToBinaryA(input, cchData, CRYPT_STRING_BASE64, NULL, &cbDecoded, NULL, NULL))
    return FALSE;

  // Allocate memory for the decoded data
  *output = (BYTE*)core->malloc(cbDecoded);
  if (!*output)
    return FALSE;

  // Decode the data
  if (!CryptStringToBinaryA(input, cchData, CRYPT_STRING_BASE64, *output, &cbDecoded, NULL, NULL)) {
    core->free(*output);
    return FALSE;
  }

  *outputSize = cbDecoded;
  return TRUE;
}

// Utility function to decode a base64 string
BOOL Base64Decode(const char *input, BYTE **output, DWORD *outputSize) {
  // // your answer here
  return TRUE;
}

// Exported function - Run
__declspec(dllexport) LPVOID CommandRunA(int argc, char **argv) {
  if (argc != 3) {
    core->wprintf(L"Invalid arguments.\n%s", CommandHelpA());
    return NULL; // Error code for invalid arguments
  }

  if (core->strcmp(argv[1], "encode") == 0) {
    char *encodedString = NULL;
    DWORD encodedSize = 0;
    if (Base64Encode(argv[2], &encodedString, &encodedSize)) {
      core->wprintf(L"%S\n", encodedString);
      lpOut = (LPVOID)encodedString;
    } else {
      core->wprintf(L"Error encoding string.\n");
      return NULL; // Error code for encoding failure
    }
  } else if (core->strcmp(argv[1], "decode") == 0) {
    BYTE *decodedBytes = NULL;
    DWORD decodedSize = 0;
    if (Base64Decode(argv[2], &decodedBytes, &decodedSize)) {
      core->wprintf(L"%.*S\n", decodedSize, decodedBytes);
      lpOut = (LPVOID)decodedBytes;
    } else {
      core->wprintf(L"Error decoding string.\n");
      return NULL; // Error code for decoding failure
    }
  } else {
    core->wprintf(L"Invalid command. Use 'encode' or 'decode'.\n");
    return NULL; // Error code for invalid command
  }

  return lpOut; // Success
}

// Entrypoint for the DLL
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  switch (fdwReason) {
  case DLL_PROCESS_ATTACH:
    // Initialization code for when the DLL is loaded
    break;
  case DLL_PROCESS_DETACH:
    // Cleanup code for when the DLL is unloaded
    break;
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
    // Thread-specific initialization or cleanup
    break;
  }
  return TRUE; // Indicate successful DLL process attachment or detachment
}
