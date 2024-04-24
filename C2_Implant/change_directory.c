
#include "change_directory.h"
#define BUFSIZE MAX_PATH
char* changedir(char* Dir)
{
    char* oBuffer = (char*)malloc(2000);

    DWORD ret = SetCurrentDirectoryA(Dir);

    DEBUG_PRINTF("DIR is = %s\n", Dir);
    DEBUG_PRINTF("SET CURRENT DIRECTORY OUTPUT = %ld\n", ret);

    if (!SetCurrentDirectoryA(Dir)) {
        sprintf(oBuffer, "Failed to change directory to '%s'. Error code: %ld\n", Dir, GetLastError());
        return oBuffer;
    } else {
	    
	    TCHAR Buffer[BUFSIZE];
	    DWORD dwRet;
	    dwRet = GetCurrentDirectory(BUFSIZ, Buffer);
	    DEBUG_PRINTF("Current directory = %ld\n", dwRet);
	    sprintf(oBuffer, "Current directory = %s\n", Buffer);
	    return oBuffer;
    }
}
