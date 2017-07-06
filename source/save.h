#pragma once

#include "basic.h"

void loadFileToBuf(u8 * buf, u32 size, const char * path);
void saveBufToFile(u8 * buf, u32 size, const char * path);
