// Header File for HTTP Functions for Implant

#include <windows.h>

#include <winhttp.h>

#include <stdio.h>

#include "debug.h"

#include <pb_encode.h>
#include <pb_decode.h>
#include "implant.pb.h"

BYTE *EncodeRegisterImplant(RegisterImplant *ri, size_t *bufferSize);
