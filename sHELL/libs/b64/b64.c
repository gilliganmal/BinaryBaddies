#include <windows.h>

#include "../include/sHELL.h"

#include <wincrypt.h>

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
  return TRUE;
}

// Exported function - Name
__declspec(dllexport) const char *CommandNameA() { return Name; }

// Exported function - Help
__declspec(dllexport) const char *CommandHelpA() { return Help; }

// Clenaup
__declspec(dllexport) VOID CommandCleanup() {
  if (lpOut) {
    core->free(lpOut);
    lpOut = NULL;
  }
}

// Utility function to encode a string to base64
BOOL Base64Encode(const char *input, char **output, DWORD *outputSize) {
  // [in] pbBinary -- A pointer to the array of bytes to be converted into a string.
  const BYTE *pbBinary = (BYTE *) input;

  // [in] pbBinary -- A pointer to the array of bytes to be converted into a string.
  DWORD cbBinary = sizeof(input) / sizeof(char);

  // [in] dwFlags -- Specifies the format of the resulting formatted string.
  // CRYPT_STRING_BASE64HEADER - 0x00000000

  // [out, optional] pszString
  // A pointer to a buffer that receives the converted string. To calculate the number of characters that must be allocated to hold the returned string, set this parameter to NULL. The function will place the required number of characters, including the terminating NULL character, in the value pointed to by pcchString.
  LPSTR pszString = *output;

  // [in, out] pcchString
  // A pointer to a DWORD variable that contains the size, in TCHARs, of the pszString buffer. If pszString is NULL, the function calculates the length of the return string (including the terminating null character) in TCHARs and returns it in this parameter. If pszString is not NULL and big enough, the function converts the binary data into a specified string format including the terminating null character, but pcchString receives the length in TCHARs, not including the terminating null character.
  DWORD *pcchString = outputSize;

  // The CryptBinaryToString function converts an array of bytes into a formatted string.
  BOOL result = CryptBinaryToStringA(pbBinary, cbBinary, CRYPT_STRING_BASE64HEADER, pszString, pcchString);

  // If the function succeeds, the function returns nonzero (TRUE).
  // If the function fails, it returns zero (FALSE).
  return result;
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
      return (LPVOID)NULL;
    }
  } else if (core->strcmp(argv[1], "decode") == 0) {
    BYTE *decodedBytes = NULL;
    DWORD decodedSize = 0;
    if (Base64Decode(argv[2], &decodedBytes, &decodedSize)) {
      // Assuming decoded output is also an ANSI string
      core->wprintf(L"%.*S\n", decodedSize, decodedBytes);
      // core->free(decodedBytes);
      lpOut = (LPVOID)decodedBytes;
    } else {
      core->wprintf(L"Error decoding string.\n");
      return NULL;
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
