#include "sort.h"

int checkSorted(const void * a, const void * b)
{
	dirInfo * entryA = (dirInfo *)a;
	dirInfo * entryB = (dirInfo *)b;

	if ((entryA->isFile && entryB->isFile) || (!entryA->isFile && !entryB->isFile)) {
		return strcmp(entryA->name, entryB->name);
	}
	else if (entryA->isFile) {
        return 1;
	}
	else if (entryB->isFile) {
        return -1;
	}

	return 0;
}

void sortDirList(dirInfo * dirInfoArrayArray, int dirCount)
{
	qsort(dirInfoArrayArray, dirCount, sizeof(dirInfo), &checkSorted);
}
