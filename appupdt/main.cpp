#include <stdio.h> 
#include <windows.h> 
#include <wininet.h> 
#pragma comment (lib, "wininet.lib")

#include "pngpacket.h"

int main(int argc, char* argv[]) 
{
	ReadPNG png;
	png.read("new2.png");
	return 0;
}





/*
char buffer[4096];

int main()
{
	HINTERNET hOpen, hURL;
	hOpen = InternetOpenA("WebReader", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	hURL = InternetOpenUrlA(hOpen, "http://www.google.com", NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);

	DWORD read;
	FILE* fp = fopen("whatever.txt", "wb");
	do
	{
		if (InternetReadFile(hURL, buffer, sizeof(buffer), &read) && (read == 0)) break;
		fwrite(buffer, 1, read, fp);
	} while (1);
	fclose(fp);

	return 0;
}
*/