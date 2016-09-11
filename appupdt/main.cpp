#include <stdio.h> 
#include <windows.h> 
#include <wininet.h> 

#pragma comment (lib, "wininet.lib") 


char buffer[4096];

int main()
{
	HINTERNET hOpen, hURL;
	hOpen = InternetOpenA("WebReader", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	hURL = InternetOpenUrlA(hOpen, "http://www.google.com", NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);

	DWORD read;
	FILE* fp = fopen("whatever.txt", "wb"); /* Added */
	do
	{
		InternetReadFile(hURL, buffer, sizeof(buffer), &read);
		fwrite(buffer, 1, read, fp);
	} while (read == sizeof(buffer));
	fclose(fp);  /* Added */

	return 0;
}