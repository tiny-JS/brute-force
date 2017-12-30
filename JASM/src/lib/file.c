#include "unit.h"
off_t
/**
 * 获取文件大小
 * @param ptrName
 * @return
 */
getFileSize(const char *ptrName)
{
    struct stat *info;
    if (stat(ptrName, info) == -1)
        err_sys("stat");

    return info->st_size;
}