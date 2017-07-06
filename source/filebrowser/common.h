#pragma once

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

// according to wikipedia, the max FAT32 path length is 255 UTF-16 characters, so 0xFF * 2 (because the 16 in UTF-16 means 16 bits = 2 bytes) (shifting left of 1 is the same as multiplying by 2)
#define MAX_PATH_LEN (0xFF << 1)

typedef struct {
	char name[MAX_PATH_LEN+1];
	bool isFile;
} dirInfo;
