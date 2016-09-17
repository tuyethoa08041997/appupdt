#pragma once

#include "zpaq\libzpaq.h"

#include "CRC32.h"

void libzpaq::error(const char* msg) {  // print message and exit
	//fprintf(stderr, "Oops: %s\n", msg);
	exit(1);
}

class Out : public libzpaq::Writer
{
private:
	FILE* file;
public:
	bool open(const char* FileName) { file = fopen(FileName, "wb"); return file != NULL; }
	void put(int c) { fputc(c, file); }
	void write(char* buf, int n) { fwrite(buf, 1, n, file); }
	bool close() { return fclose(file) == 0; }
} out;

class File_In : public libzpaq::Reader {
private:
	unsigned char* buffer;
	unsigned long bufsize;
	unsigned long bufpos;

public:
	File_In() {
		buffer = NULL;
		bufsize = 0;
		bufpos = 0;
	}
	~File_In() {
		if (buffer) VirtualFree(buffer, 0, MEM_RELEASE);
	}
	inline bool eof() { return bufsize == bufpos; }
	inline unsigned long getsize() { return bufsize; }
	inline unsigned long getpos() { return bufpos; }
	inline bool setpos(unsigned long pos) { if (pos < bufsize) { bufpos = pos; return true; } return false; }
	inline unsigned long crc32(unsigned long pos, unsigned long len) {
		return ((pos < bufsize) && (pos + len <= bufsize)) ? crc.crc(&buffer[pos], len) : (-1);
	}
	inline bool open(const char* FileName) {
		HANDLE file = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE) return false; // open file failed

		LARGE_INTEGER size;
		if ((GetFileSizeEx(file, &size) != 0) && (!(size.HighPart != 0) || (size.LowPart > 0x00400000) || (size.LowPart < 0x00001000))) {
			///return false; // file size failed
			///return false; // file too big or too small
			buffer = (unsigned char*)VirtualAlloc(NULL, size.LowPart, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (buffer != NULL) {
				if ((ReadFile(file, buffer, size.LowPart, &bufsize, NULL) != 0) && (bufsize == size.LowPart)) {
					///return false; // file read failed
					bufpos = 0;
					CloseHandle(file);
					return true;
				}
			}
			VirtualFree(buffer, 0, MEM_RELEASE);
			buffer = NULL;
		}
		CloseHandle(file);
		return false;
	}
	int xget() {
		return (bufpos < bufsize) ? (buffer[bufpos++]) : (-1);
	}
	int xread(void* buf, int n) {
		int sz = min(n, bufsize - bufpos);
		for (int i = 0; i < sz; i++) {
			((char*)buf)[i] = buffer[bufpos++];
		}
		return sz;
	}

	// zpaq encrypted by add 0x33, xor 0xAA, sub 0x33
	int get() {
		return (bufpos < bufsize) ? (((buffer[bufpos++] + 0x33) ^ 0xAA) - 0x33) : (-1);
	}
	int read(char* buf, int n) {
		int sz = min(n, bufsize - bufpos);
		for (int i = 0; i < sz; i++) {
			((unsigned char*)buf)[i] = ((buffer[bufpos++] + 0x33) ^ 0xAA) - 0x33;
		}
		return sz;
	}
};

class ReadPNG
{
private:
	void* buffer;
	unsigned long bufsize;
	unsigned long bufpos;

	inline unsigned long _bswap(unsigned long a) {
		return (a >> 24) | (a << 24) | ((a >> 8) & 0x0000FF00) | ((a << 8) & 0x00FF0000);
	}

public:
	bool read(const char* FileName) {
		bool packet_found = false;
		File_In in;
		if (!in.open(FileName)) return false;

		unsigned long buffer[2];

		// http://www.libpng.org/pub/png/spec/1.2/PNG-Structure.html
		// PNG sig
		// 89 50 4E 47 0D 0A 1A 0A
		if ((in.xread(buffer, 8) != 8) || (buffer[0] != 0x474E5089) || (buffer[1] != 0x0A1A0A0D))
			return false; // sig failed - file structure error

		do {
			// read Length & ChunkSIG
			if (in.xread(buffer, 8) != 8) return false; // file structure error

			unsigned long datapos = in.getpos();
			unsigned long datalen = _bswap(buffer[0]);
			if (in.setpos(datapos + datalen) == false) return false; // file structure error


			// read CRC
			if (in.xread(buffer, 4) != 4) return false; // file structure error
			if (_bswap(buffer[0]) != in.crc32(datapos - 4, datalen + 4)) return false; // CRC error - file corrupted

			// my packet sig
			// zpAq 7A 70 41 71
			if ((!packet_found) && (buffer[1] == 0x7141707A)) {
				unsigned long backpos = in.getpos();
				// goto zpaq pos
				in.setpos(datapos);


				out.open("decrypted.raw");
				libzpaq::decompress(&in, &out);
				out.close();

				// go back to current pos
				in.setpos(backpos);
				packet_found = true;
			}
		} while (!in.eof());


	}
};
















