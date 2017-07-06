#include "save.h"

void loadFileToBuf(u8 * buf, u32 size, const char * path)
{
	FILE * fh = fopen(path, "rb");
	fread(buf, size, 1, fh);
	fclose(fh);
}

void saveBufToFile(u8 * buf, u32 size, const char * path)
{
	FILE * fh = fopen(path, "wb");
	fwrite(buf, size, 1, fh);
	fclose(fh);
}
