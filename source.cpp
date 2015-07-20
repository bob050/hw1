
#include <tchar.h>
#include "Header.h"

int main()
{

    wchar_t *buf=NULL;
    uint32_t buflen = 0;
    buflen = GetCurrentDirectoryW(buflen, buf);
        if (0 == buflen)
        {
        print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
        return false;
        }
 
        buf = (PWSTR) malloc(sizeof(WCHAR) * buflen);
        if (0 == GetCurrentDirectoryW(buflen, buf))
        {
                print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
                free(buf);
                return false;
        }
 
    // current dir \\ bob.txt 파일명 생성
    wchar_t file_name[260];
    if (!SUCCEEDED(StringCbPrintfW(
                            file_name,
                            sizeof(file_name),
                            L"%ws\\bob.txt",
                            buf)))
    {  
        print("err, can not create file name");
        free(buf);
        return false;
    }
    

	HANDLE file_handle = CreateFileW(
                                file_name,
                                                        GENERIC_WRITE,
                                                        FILE_SHARE_READ,
                                                        NULL,
                                                        CREATE_ALWAYS,
                                                        FILE_ATTRIBUTE_NORMAL,
                                                        NULL); 
        if(file_handle == INVALID_HANDLE_VALUE)
        {                                              
        print("err, CreateFile(path=%ws), gle=0x%08x", file_name, GetLastError());
        return false;
        }
 
	DWORD bytes_written = 0;
	wchar_t string_buf[1024]={0};

  if (!SUCCEEDED(StringCbPrintfW(
                        string_buf,
                        sizeof(string_buf),
                        L"가나다라마바 동ha물gua 백doo산2 마rr고 dar도rok 하nn님e 보woo하sa 우ri나la만se")))
    {
        print("err, can not create data to write.");
        CloseHandle(file_handle);
        return false;
    }
	
   //wide -> multi
   int utf8_string_len = WideCharToMultiByte(CP_UTF8, 0, string_buf, -1, NULL, 0, NULL, NULL);
   char *utf8_string = (char*)malloc(utf8_string_len * sizeof(char));
   RtlZeroMemory(utf8_string, utf8_string_len);
   WideCharToMultiByte(CP_UTF8, 0, string_buf, -1, utf8_string, utf8_string_len, NULL, NULL);
   

    if (!WriteFile(file_handle, utf8_string, utf8_string_len, &bytes_written, NULL))
    {
        printf("err, WriteFile() failed. gle = 0x%08x", GetLastError());
        CloseHandle(file_handle);
        return false;
    }
   
	memset(utf8_string, 0, utf8_string_len);
   
   
	CloseHandle(file_handle);
	
	HANDLE file_handle2 = CreateFileW(L"bob2.txt",
      GENERIC_WRITE,
      FILE_SHARE_READ,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      NULL);

   HANDLE file_handle_read = CreateFileW(L"bob.txt",
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL
      );

   memset(utf8_string, 0, utf8_string_len);
   
   DWORD copy_len;
   ReadFile(file_handle_read, utf8_string, utf8_string_len, &copy_len, NULL);
   
    if (!WriteFile(file_handle2, utf8_string, utf8_string_len, &bytes_written, NULL))
    {
        printf("err, WriteFile() failed. gle = 0x%08x", GetLastError());
        CloseHandle(file_handle);
        return false;
    }



  
 ///////////////////////////////////////////////////////////////////////////
	
    // check file size
    
    LARGE_INTEGER fileSize;
    if (TRUE != GetFileSizeEx(file_handle2, &fileSize))
    {
        print("err, GetFileSizeEx(%ws) failed, gle = %u", file_name, GetLastError());
        CloseHandle(file_handle2);
        return false;
    }
 
    
    
    _ASSERTE(fileSize.HighPart == 0);
        if (fileSize.HighPart > 0)
        {
        print("file size = %I64d (over 4GB) can not handle. use FileIoHelperClass",
            fileSize.QuadPart);
                CloseHandle(file_handle2);
        return false;
        }
 
		CloseHandle(file_handle2);
		
	HANDLE file_handle_memory = CreateFileW(L"bob2.txt",
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL);
		
    DWORD file_size = (DWORD)fileSize.QuadPart;
 
	HANDLE file_map = CreateFileMapping(
                            file_handle_memory,
                            NULL,
                            PAGE_READONLY,
                            0,
                            0,
                            NULL
                            );
    if (NULL == file_map)
    {
        print("err, CreateFileMapping(%ws) failed, gle = %u", file_name, GetLastError());
        CloseHandle(file_handle2);
        return false;
    }
 
    PCHAR file_view = (PCHAR) MapViewOfFile(
                                    file_map,
                                    FILE_MAP_READ,
                                    0,
                                    0,
                                    0
                                    );
    if(file_view == NULL)
    {
        print("err, MapViewOfFile(%ws) failed, gle = %u", file_name, GetLastError());
       
        CloseHandle(file_map);
        CloseHandle(file_handle2);
        return false;
    }  

	
	CloseHandle(file_handle2);
	
	//multibyte->unicode -> asckii
	memset(string_buf, 0, sizeof(string_buf));
	int read_len;
	read_len=MultiByteToWideChar(CP_UTF8,0,file_view,-1,NULL,NULL);
	MultiByteToWideChar(CP_UTF8,0,file_view,-1,string_buf,read_len);
	
	read_len = WideCharToMultiByte(CP_ACP, 0, string_buf, -1, NULL, 0, NULL, NULL);
	
    WideCharToMultiByte(CP_ACP, 0,string_buf, -1, utf8_string, read_len, NULL, NULL);
	
	
	print("Console Outpu: %s \n", utf8_string);
}

