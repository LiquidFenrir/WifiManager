#include <dirent.h>
#include "dir.h"

int listdir(char * path, dirInfo * dirInfoArray)
{
	DIR *dir;
	struct dirent *ent;
	int count = 2;
	strncpy(dirInfoArray[0].name, "..", MAX_PATH_LEN);
	dirInfoArray[0].isFile = false;
	strncpy(dirInfoArray[1].name, ".", MAX_PATH_LEN);
	dirInfoArray[1].isFile = true;
	
	if ((dir = opendir(path)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			strncpy(dirInfoArray[count].name, ent->d_name, MAX_PATH_LEN);
			dirInfoArray[count].isFile = (ent->d_type == 8);
			count++;
		}
		closedir (dir);
	}
	
	return count;
}
