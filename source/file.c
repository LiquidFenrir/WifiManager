#include "file.h"

Result createFolderRecursively(const char * path, FS_Archive archive)
{
    Result ret = 0;
    char * dup = strdup(path);
    char * slashpos = NULL;

    for(slashpos = strchr(dup+1, '/'); slashpos != NULL; slashpos = strchr(slashpos+1, '/'))
    {
        if(slashpos != NULL)
            *slashpos = '\0';
        FS_Path dirpath = fsMakePath(PATH_ASCII, dup);
        Handle dirHandle;
        ret = FSUSER_OpenDirectory(&dirHandle, archive, dirpath);
        if(R_SUCCEEDED(ret))
        {
            FSDIR_Close(dirHandle);
        }
        else
        {
            if(R_FAILED(ret = FSUSER_CreateDirectory(archive, dirpath, FS_ATTRIBUTE_DIRECTORY)))
                break;
        }

        if(slashpos != NULL)
            *slashpos = '/';
        else
            break;
    }

    free(dup);
    return ret;
}

Result openFile(const char * path, FS_Archive archive, Handle * handle, bool write)
{
    u32 flags = (write ? (FS_OPEN_WRITE | FS_OPEN_CREATE) : FS_OPEN_READ);

    createFolderRecursively(path, archive);
    if (write)
        deleteFile(path, archive);  
    return FSUSER_OpenFile(handle, archive, fsMakePath(PATH_ASCII, path), flags, 0);
}

Result deleteFile(const char * path, FS_Archive archive)
{
    return FSUSER_DeleteFile(archive, fsMakePath(PATH_ASCII, path));
}
