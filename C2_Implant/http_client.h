// Header File for HTTP Functions for Implant

#include <windows.h>

#include <winhttp.h>

#include <stdio.h>

#include "debug.h"

#include <pb_encode.h>
#include <pb_decode.h>
#include "implant.pb.h"

LPBYTE SendToServer(LPCWSTR VERB, LPCWSTR PATH, BYTE *buffer, size_t bytes_written);

