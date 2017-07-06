#include "draw.h"

//width in characters of the screen you're drawing on. 50 for the top screen, 40 for bottom
#define SCREEN_WIDTH 50
#define SCREEN_HEIGHT 30

#define OFFSET 1 //offset in characters of the file list from the location bar

#define MAX_ENTRIES_PER_SCREEN (SCREEN_HEIGHT-1-OFFSET*2)

static int scroll = 0;

static char uparrow[] = {30, 0};
static char downarrow[] = {31, 0};

void drawDirList(dirInfo * dirInfoArray, char * currentPath, int currentDir, int dirCount)
{
	consoleClear();
	if (strlen(currentPath) <= SCREEN_WIDTH) printf("\x1b[0;0H\x1b[47;30m%s\x1b[0m", currentPath);
	else printf("\x1b[0;0H\x1b[47;30m...%s\x1b[0m", &currentPath[strlen(currentPath)-SCREEN_WIDTH+3]);
	
	if (currentDir == 0) {
		scroll = 0;
	}
	else if ((dirCount > MAX_ENTRIES_PER_SCREEN) && (currentDir == dirCount-1)) {
		scroll = dirCount - MAX_ENTRIES_PER_SCREEN;
	}
	else if (currentDir >= (MAX_ENTRIES_PER_SCREEN + scroll)) {
		scroll++;
	}
	else if ((currentDir - scroll) < 0) {
		scroll--;
	}
	
	if (scroll != 0) printf("\x1b[%i;0H%s", OFFSET+1, uparrow);
	
	if ((dirCount > MAX_ENTRIES_PER_SCREEN) && scroll != (dirCount - MAX_ENTRIES_PER_SCREEN)) printf("\x1b[%i;0H%s", MAX_ENTRIES_PER_SCREEN+OFFSET, downarrow);
	
	for (int i = scroll; (i-scroll) < MAX_ENTRIES_PER_SCREEN; i++) {
		//selected dir has white background, others have black
		int bgcolor = (i == currentDir) ? 47 : 40;
		//if it's a file, it has a blue name. otherwise, reverse of background
		int txtcolor = (dirInfoArray[i].isFile) ? 36 : ((i == currentDir) ? 30 : 37);
		if (dirInfoArray[i].name != NULL) printf("\x1b[%i;2H\x1b[%i;%im%s\x1b[0m", i+OFFSET+1-scroll, bgcolor, txtcolor, dirInfoArray[i].name);
	}
}
