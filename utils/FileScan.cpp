#include "FileScan.h"

#include <dirent.h>
#include <string.h>  
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <string.h>
#include <errno.h>

#include "../QupLog.h"

bool iter_dir(const char* dir, int depth, int* deep) {
    if (*deep < depth)
        *deep = depth;

    DIR* dp;
    struct dirent* entry;
    struct stat statbuf;

    if ((dp = opendir(dir)) == NULL) {
#ifdef DEBUG_ALL
        QUP_LOGI("[-] can't open dir %s, %s", dir, strerror(errno));
#endif
        return false;
    }

    chdir(dir);

    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if (S_IFDIR & statbuf.st_mode) {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
#ifdef DEBUG_ALL
            QUP_LOGI("[*] depth = %d, dir  name = %s", depth, entry->d_name);
#endif
            return iter_dir(entry->d_name, depth + 1, deep);
        } else {
#ifdef DEBUG_ALL
            QUP_LOGI("[*] depth = %d, file name = %s", depth, entry->d_name);
#endif
        }
    }
    chdir("..");
    closedir(dp);

    return true;
}

bool iter_del_dir(const char* dir, int depth) {

    DIR* dp;
    struct dirent* entry;
    struct stat statbuf;

    if ((dp = opendir(dir)) == NULL) {
#ifdef DEBUG_ALL
        QUP_LOGI("[-] can't open dir %s, %s", dir, strerror(errno));
#endif
        return false;
    }

    chdir(dir);

    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if (S_IFDIR & statbuf.st_mode) {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                continue;
#ifdef DEBUG_ALL
            QUP_LOGI("[*] depth = %d, dir  name = %s", depth, entry->d_name);
#endif
            if(!iter_del_dir(entry->d_name, depth + 1)) {
                return false;
            }
            rmdir(entry->d_name);
        } else {
#ifdef DEBUG_ALL
            QUP_LOGI("[*] depth = %d, file name = %s", depth, entry->d_name);
#endif
            unlink(entry->d_name);
        }
    }
    chdir("..");
    closedir(dp);

    return true;
}
