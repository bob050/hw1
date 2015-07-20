#include "Header.h"

void print(_In_ const char* fmt, _In_ ...)
{
        char log_buffer[2048];
    va_list args;
 
    va_start(args,fmt);
    HRESULT hRes = StringCbVPrintfA(log_buffer, sizeof(log_buffer), fmt, args);
    if (S_OK != hRes)
    {
        fprintf(
            stderr,
            "%s, StringCbVPrintfA() failed. res = 0x%08x",
            __FUNCTION__,
            hRes
            );
        return;
    }
 
    OutputDebugStringA(log_buffer);
    fprintf(stdout, "%s \n", log_buffer);
}
 