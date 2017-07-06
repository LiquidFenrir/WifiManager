#include "blocks.h"
#include "checksum.h"

Result getWifiSlot(int selected_slot, wifiBlock * slotData)
{
	Result ret = CFG_GetConfigInfoBlk8(CFG_WIFI_SLOT_SIZE, CFG_WIFI_BLKID+selected_slot, (u8*)slotData);
	if (ret) printf("CFG_GetConfigInfoBlk8\nresult: 0x%.8lx\n", ret);
	return ret;
}

Result setWifiSlot(int selected_slot, wifiBlock * slotData)
{
	Result ret = CFG_SetConfigInfoBlk8(CFG_WIFI_SLOT_SIZE, CFG_WIFI_BLKID+selected_slot, (u8*)slotData);
	if (ret) printf("CFG_GetConfigInfoBlk8\nresult: 0x%.8lx\n", ret);
	return ret;
}

void fixSlotCRC(wifiBlock * slotData)
{
	printf("Previous CRC-16 checksum: %.4X\n", slotData->checksum);
	slotData->checksum = crc_16((u8*)(&(slotData->network)), 0x410);
	printf("New CRC-16 checksum: %.4X\n", slotData->checksum);
}
