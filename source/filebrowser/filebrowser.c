#include "common.h"
#include "dir.h"
#include "draw.h"
#include "sort.h"

static char currentPath[MAX_PATH_LEN+1]; //for the ending nullbyte

static dirInfo dirInfoArray[256];
static int dirCount = 0;
static int currentDir = 0;

char * filebrowser(char * startDir) {
	chdir(startDir);
	
	goto change;
	
	while (aptMainLoop()) {
		
		//things in there will only run if you do a goto
		//otherwise the screen would flicker because of constantly clearing
		if (false) {
			change:
			currentDir = 0;
			getcwd(currentPath, MAX_PATH_LEN+1);
			dirCount = listdir(currentPath, dirInfoArray);
			for (int i = dirCount; i < 256; i++) {
				dirInfoArray[i].name[0] = 0;
				dirInfoArray[i].isFile = false;
			}
			sortDirList(&dirInfoArray[2], dirCount-2);
			
			draw:
			if (currentDir > dirCount-1) currentDir = dirCount-1;
			if (currentDir < 0) currentDir = 0;
			drawDirList(dirInfoArray, currentPath, currentDir, dirCount);
			gfxFlushBuffers();
			gfxSwapBuffers();
			gspWaitForVBlank();
		}
		
		hidScanInput();
		
		if (hidKeysDown() & KEY_START) {
			break;
		}
		else if (hidKeysDown() & KEY_A) {
			if (dirInfoArray[currentDir].isFile) {
				//return the path to the file
				char fullPath[MAX_PATH_LEN+1];
				sprintf(fullPath, "%s%s", currentPath, dirInfoArray[currentDir].name);
				return strdup(fullPath);
			}
			else {
				chdir(dirInfoArray[currentDir].name);
				goto change;
			}
		}
		else if (hidKeysDown() & KEY_B) {
			chdir("..");
			goto change;
		}
		else if (hidKeysDown() & KEY_LEFT) {
			currentDir = 0;
			goto draw;
		}
		else if (hidKeysDown() & KEY_RIGHT) {
			currentDir = dirCount-1;
			goto draw;
		}
		else if (hidKeysDown() & KEY_UP) {
			currentDir--;
			goto draw;
		}
		else if (hidKeysDown() & KEY_DOWN) {
			currentDir++;
			goto draw;
		}
		
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}
	
	return NULL;
}
