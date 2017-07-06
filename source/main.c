#include "blocks.h"
#include "save.h"
#include "stringutils.h"
#include "filebrowser/filebrowser.h"

#define WORKING_DIR "/3ds/WifiManager/"

int main() {
	
	chdir(WORKING_DIR);
	
	gfxInitDefault();
	PrintConsole topScreen, bottomScreen;
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	Result ret = cfguInit();
	if (ret) printf("cfguInit\nresult: 0x%08x\n", (unsigned int)ret);
	
	int selected_slot = 0;
	
	puts("Press A to save current slot to file");
	puts("Press B to restore current slot from file");
	puts("Press START to quit (reboot)");
	printf("Selected slot: %i\n", selected_slot);
	
	while (aptMainLoop()) {
		
		hidScanInput();
		
		if (hidKeysDown() & KEY_START) break;
		else if (hidKeysDown() & KEY_A)
		{
			printf("Saving WiFi slot %i...\n", selected_slot);
			wifiBlock slotData;
			ret = getWifiSlot(selected_slot, &slotData);
			
			char * suffix = "_wifislot.bin";
			char filename[0x20+strlen(suffix)+1];
			strcpy(filename, "Empty slot");
			
			if (slotData.network.use) strncpy(filename, slotData.network.SSID, 0x20);
			else if (slotData.network_WPS.use) strncpy(filename, slotData.network_WPS.SSID, 0x20);
			
			strcat(filename, suffix);
			cleanPath(filename);
			printf("Saving slot to %s%s...", WORKING_DIR, filename);
			saveBufToFile((u8*)&slotData, CFG_WIFI_SLOT_SIZE, filename);
			printf("Done!\n");
		}
		else if (hidKeysDown() & KEY_B)
		{
			printf("Restoring WiFi slot %i...\n", selected_slot);
			
			consoleSelect(&topScreen);
			char * filepath = filebrowser(WORKING_DIR);
			consoleSelect(&bottomScreen);
			if (filepath == NULL) {
				printf("Cancelling restore...\n");
				continue;
			}
			
			printf("Loading data from:\n%s\nto WiFi slot %i\n", filepath, selected_slot);
			wifiBlock slotData;
			loadFileToBuf((u8*)&slotData, CFG_WIFI_SLOT_SIZE, filepath);
			free(filepath);
			
			fixSlotCRC(&slotData);
			ret = setWifiSlot(selected_slot, &slotData);
			printf("Saving config savegame...\n");
			ret = CFG_UpdateConfigNANDSavegame();
			if (ret) printf("CFG_UpdateConfigNANDSavegame\nresult: 0x%.8lx\n", ret);
			printf("Done!\n");
		}
		else if (hidKeysDown() & KEY_UP)
		{
			selected_slot++;
			if (selected_slot >= CFG_WIFI_SLOTS) selected_slot = CFG_WIFI_SLOTS-1;
			printf("Selected slot: %i\n", selected_slot);
		}
		else if (hidKeysDown() & KEY_DOWN)
		{
			selected_slot--;
			if (selected_slot < 0) selected_slot = 0;
			printf("Selected slot: %i\n", selected_slot);
		}
		
		gfxFlushBuffers();
		gfxSwapBuffers();
		
		gspWaitForVBlank();
	}
	
	cfguExit();
	gfxExit();
	
	APT_HardwareResetAsync();
	
	return 0;
}
