#pragma once

#include "basic.hpp"

Result createFolderRecursively(const char * path, FS_Archive archive);
Result openFile(const char * path, FS_Archive archive, Handle * handle, bool write);
Result deleteFile(const char * path, FS_Archive archive);
