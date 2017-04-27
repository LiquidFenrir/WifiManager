#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define WORKDIR "/3ds/WifiManager/"

#define CFG_WIFI_BLKID (u32 )0x00080000
#define CFG_WIFI_SLOTS 3
#define CFG_WIFI_SLOT_SIZE (u32 )0xC00

Result ret;

void loadFileToBuf(u8 * buf, u32 size, const char * path)
{
	printf("Load %s to buf\n", path);
	
	FILE * fh = fopen(path, "rb");
	
	fread(buf, size, 1, fh);
	
	fclose(fh);
	
	puts("done");
}

void saveBufToFile(u8 * buf, u32 size, const char * path)
{
	printf("Saving buf to %s\n", path);
	
	FILE * fh = fopen(path, "wb");
	
	fwrite(buf, size, 1, fh);
	
	fclose(fh);
	
	puts("done");
}

int main() {
	
	chdir(WORKDIR);
	
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	
	puts("init");
	
	ret = cfguInit();
	if (ret) printf("cfguInit\nresult: 0x%08x\n", (unsigned int)ret);
	
	puts("done");
	
	puts("mallocing");
	
	u8 * wifiblk = malloc(CFG_WIFI_SLOT_SIZE);
	memset(wifiblk, 0x0, CFG_WIFI_SLOT_SIZE);
	
	puts("done");
	
	u32 selected_slot = 0;
	
	puts("Press A to save current slot to file");
	puts("Press B to restore current slot from file");
	puts("Press START to quit");
	
	while (aptMainLoop()) {
		
		hidScanInput();
		
		if (hidKeysDown() & KEY_START) break;
		else if (hidKeysDown() & KEY_A)
		{	
			puts("getting");
			
			ret = CFG_GetConfigInfoBlk8(CFG_WIFI_SLOT_SIZE, CFG_WIFI_BLKID+selected_slot, wifiblk);
			if (ret) printf("CFG_GetConfigInfoBlk8\nresult: 0x%08x\n", (unsigned int)ret);
			else {
				
				printf("Slot %lu SSID is '%s' and password is '%s'\n", selected_slot, wifiblk+0x8, wifiblk+0x2C);
				
				puts("saving");
				
				char * filename = NULL;
				asprintf(&filename, "wifiblk_%lu.bin", selected_slot);
				saveBufToFile(wifiblk, CFG_WIFI_SLOT_SIZE, filename);
				free(filename);
				memset(wifiblk, 0x0, CFG_WIFI_SLOT_SIZE);
			}
			
			puts("done");
		}
		else if (hidKeysDown() & KEY_B)
		{	
			puts("loading");
			
			char * filename = NULL;
			asprintf(&filename, "wifiblk_%lu.bin", selected_slot);
			loadFileToBuf(wifiblk, CFG_WIFI_SLOT_SIZE, filename);
			free(filename);
			
			printf("Slot %lu SSID is '%s' and password is '%s'\n", selected_slot, wifiblk+0x8, wifiblk+0x2C);
			
			puts("setting");
			
			ret = CFG_SetConfigInfoBlk8(CFG_WIFI_SLOT_SIZE, CFG_WIFI_BLKID+selected_slot, wifiblk);
			if (ret) printf("CFG_GetConfigInfoBlk8\nresult: 0x%08x\n", (unsigned int)ret);
			
			memset(wifiblk, 0x0, CFG_WIFI_SLOT_SIZE);
			
			puts("done");
		}
		else if (hidKeysDown() & KEY_UP)
		{
			selected_slot++;
			if (selected_slot >= CFG_WIFI_SLOTS) selected_slot = CFG_WIFI_SLOTS-1;
			printf("Selected slot: %lu\n", selected_slot);
		}
		else if (hidKeysDown() & KEY_DOWN)
		{
			selected_slot--;
			if (selected_slot >= CFG_WIFI_SLOTS) selected_slot = 0;
			printf("Selected slot: %lu\n", selected_slot);
		}
		
		gfxFlushBuffers();
		gfxSwapBuffers();
		
		gspWaitForVBlank();
	}
	
	free(wifiblk);
	
	cfguExit();
	gfxExit();
	return 0;
}
