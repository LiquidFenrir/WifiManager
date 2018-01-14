#include "stringutils.h"

void cleanPath(char * path)
{
    for(unsigned int i = 0; i < strlen(path); i++)
    { //replace all spaces and fat32 reserved characters in the path with underscores 
        switch (path[i]) {
            case ' ':
            case '"':
            case '*':
            case ':':
            case '<':
            case '>':
            case '?':
            case '\\':
            case '|':
                path[i] = '_';
            default:
                break;
        }
    }
}
