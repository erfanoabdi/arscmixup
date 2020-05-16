/*
 * File:   ApkZipOp.cpp
 * Author: angeltoms
 *
 */

#include "ApkZipOp.h"
#include "../zip/ZipEntry.h"
#include "../utils/FileScan.h"
#include "../QupLog.h"

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
//#include <error.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>

ApkZipOp::ApkZipOp(std::string fname, bool isBack) :
mFileName(fname), mIsBack(isBack) {
}

ApkZipOp::~ApkZipOp() {
}

static int mkdirp(char* path) {
    int ret;
    char currpath[PATH_MAX];
    struct stat st;
    // reset path
    strcpy(currpath, "");
    char* pathpiece = strtok(path, "/");
    if (path[0] == '/') {
        // prepend / if needed
        strcat(currpath, "/");
    }

    while (pathpiece != NULL) {
        if (strlen(currpath) + strlen(pathpiece) + 2/*NUL and slash*/ > PATH_MAX) {
            QUP_LOGI("[-] Invalid path specified: too long\n");
            return -1;
        }
        strcat(currpath, pathpiece);
        strcat(currpath, "/");
        if (stat(currpath, &st) != 0) {
            ret = mkdir(currpath, 0755);
            if (ret < 0) {
                QUP_LOGI("[-] mkdir failed for %s, %s\n", currpath, strerror(errno));
                return ret;
            }
        }
        pathpiece = strtok(NULL, "/");
    }
}

bool ApkZipOp::unzipResToPwd() {

    mUnzipPath = std::string(mFileName.substr(0, mFileName.find(".apk")));

    if (mIsBack) {
        std::string nfname = mFileName;
        nfname.append(".bak.apk");

        QUP_LOGI("[*] backed apk name = %s", nfname.c_str());
        int fd = open(mFileName.c_str(), O_RDONLY);
        if (fd < 0) {
            QUP_LOGI("[-] open file %s fail , %s", mFileName.c_str(), strerror(errno));
            return false;
        }

        int fod = open(nfname.c_str(), O_WRONLY | O_CREAT, 0775);
        if (fod < 0) {
            QUP_LOGI("[-] create file %s fail , %s", nfname.c_str(), strerror(errno));
            close(fd);
            return false;
        }

        char buf[4096];

        int len;
        while ((len = read(fd, buf, 4096)) > 0) {
            write(fod, buf, len);
            fsync(fod);
        }

        close(fod);
        close(fd);

        QUP_LOGI("[*] back apk success");
    }

    std::string apkfile = mUnzipPath;
    QUP_LOGI("[*] apk name = %s", apkfile.c_str());

    if (access(apkfile.c_str(), F_OK) != -1) {
        if (!iter_del_dir(apkfile.c_str(), 0)) {
            QUP_LOGI("[-] delete dir %s fail", apkfile.c_str());
            return false;
        }

        rmdir(apkfile.c_str());
    }

    if (mkdir(apkfile.c_str(), 0755) != 0) {
        QUP_LOGI("[-] create dir %s fail %s", apkfile.c_str(), strerror(errno));
        return false;
    }

    if (chdir(apkfile.c_str()) != 0) {
        QUP_LOGI("[-] cd dir %s fail %s", apkfile.c_str(), strerror(errno));
        return false;
    }

    char pwd[1024];
    memset(pwd, 0, 1024);
    if (getcwd(pwd, 1024) == NULL) {
        QUP_LOGI("[-] get pwd fail %s", strerror(errno));
    }

    QUP_LOGI("[*] current dir = %s", pwd);

    ZipFile zipFile;

    if (zipFile.open(mFileName.c_str(), ZipFile::kOpenReadOnly) != NO_ERROR) {
        QUP_LOGI("[-] Unable to open '%s' for verification", mFileName.c_str());
        return false;
    }

    int numEntries = zipFile.getNumEntries();

    QUP_LOGI("[*] apk entries = %d", numEntries);
    ZipEntry* pEntry;
    for (int i = 0; i < numEntries; i++) {
        pEntry = zipFile.getEntryByIndex(i);
        if (pEntry->isCompressed()) {
#ifdef DEBUG_ALL
            QUP_LOGI("[*] %8ld %s (OK - compressed) compress method(%d)",
                    (long) pEntry->getFileOffset(), pEntry->getFileName(),
                    pEntry->getCompressionMethod());
#endif
        } else {
#ifdef DEBUG_ALL
            QUP_LOGI("[*] %8ld %s (OK - not compressed) compress method(%d)",
                    (long) pEntry->getFileOffset(), pEntry->getFileName(),
                    pEntry->getCompressionMethod());
#endif
        }

        if (strstr(pEntry->getFileName(), "res/") != NULL) {
            void* buf = zipFile.uncompress(pEntry);
            std::string fname(pwd);
            fname.append("/").append(pEntry->getFileName());

#ifdef DEBUG_ALL
            QUP_LOGI("[*] unzip file name = %s", fname.c_str());
#endif

            std::string fdir = fname.substr(0, fname.rfind("/"));

            if (mkdirp((char*) fdir.c_str()) != 0) {
                QUP_LOGI("[-] creat dir %s fail %s", fdir.c_str(), strerror(errno));
                return false;
            }

            FILE* f = fopen(fname.c_str(), "w+");
            if (!f) {
                QUP_LOGI("[-] open and create file %s fail %s", fname.c_str(), strerror(errno));
                return false;
            }

            fwrite(buf, 1, pEntry->getUncompressedLen(), f);
            fclose(f);
        }

        if (strstr(pEntry->getFileName(), "resources.arsc") != NULL) {
            void* buf = zipFile.uncompress(pEntry);
            std::string fname(pwd);
            fname.append("/").append(pEntry->getFileName());
            QUP_LOGI("[*] unzip file name = %s", fname.c_str());

            std::string fdir = fname.substr(0, fname.rfind("/"));

            FILE* f = fopen(fname.c_str(), "w+");
            if (!f) {
                QUP_LOGI("[-] open and create file %s fail %s", fname.c_str(), strerror(errno));
                return false;
            }

            fwrite(buf, 1, pEntry->getUncompressedLen(), f);
            fclose(f);
        }
    }
    return true;
}

bool ApkZipOp::delApkRes() {
    ZipFile zipFile;

    if (zipFile.open(mFileName.c_str(), ZipFile::kOpenReadWrite) != NO_ERROR) {
        QUP_LOGI("[-] Unable to open '%s' for verification", mFileName.c_str());
        return false;
    }

    int numEntries = zipFile.getNumEntries();

    QUP_LOGI("[*] apk entries = %d", numEntries);
    ZipEntry* pEntry;
    for (int i = 0; i < numEntries; i++) {
        pEntry = zipFile.getEntryByIndex(i);
        if (strstr(pEntry->getFileName(), "META-INF/") != NULL ||
                strstr(pEntry->getFileName(), "res/") != NULL ||
                strstr(pEntry->getFileName(), "resources.arsc") != NULL) {
            zipFile.remove(pEntry);
        }
    }
    return true;
}

bool ApkZipOp::addApkMixupRes(std::map<ResMaps*, ResMaps*>& maps) {
    ZipFile zipFile;

    if (zipFile.open(mFileName.c_str(), ZipFile::kOpenReadWrite) != NO_ERROR) {
        QUP_LOGI("[-] Unable to open '%s' for verification", mFileName.c_str());
        return false;
    }

    if (chdir(mUnzipPath.c_str()) != 0) {
        QUP_LOGI("[-] cd dir %s fail %s", mUnzipPath.c_str(), strerror(errno));
        return false;
    }

    if (unlink("resources.arsc") != 0) {
        QUP_LOGI("[-] delete backed resources.arsc fail %s", strerror(errno));
        return false;
    }

    if (rename("resources.arsc.1", "resources.arsc") != 0) {
        QUP_LOGI("[-] rename resources.arsc.1 to resources.arsc fail %s", strerror(errno));
        return false;
    }

    ZipEntry* pEntry;
    std::string resourcesFile = mUnzipPath;
    resourcesFile.append("/").append("resources.arsc");

    std::string compressName = resourcesFile.substr((mUnzipPath.size() + 1));

    QUP_LOGI("[*] compressName = %s", compressName.c_str());

    if (zipFile.add(resourcesFile.c_str(), compressName.c_str(), 0, &pEntry) != 0) {
        QUP_LOGI("[-] compress resources.arsc fail");
        return false;
    }

    QUP_LOGI("[*] pEntry name = %s", pEntry->getFileName());

    std::map<ResMaps*, ResMaps*>::iterator iter;
    static std::string resCompressName("");
    static std::string resPath("");

    for (iter = maps.begin(); iter != maps.end(); iter++) {

        resCompressName = iter->second->name;
        resPath = mUnzipPath;
        resPath.append("/").append(resCompressName);

        if (iter->second->isDir)
            continue;

        QUP_LOGI("[*] res path = %s, compressName = %s", resPath.c_str(), resCompressName.c_str());

        if (compressName.find(".png") != std::string::npos ||
                compressName.find(".jpg") != std::string::npos ||
                compressName.find(".ogg") != std::string::npos ||
                compressName.find(".mp3") != std::string::npos) {
            if (zipFile.add(resourcesFile.c_str(), resCompressName.c_str(), ZipEntry::kCompressDeflated, NULL) != 0) {
                QUP_LOGI("[-] compress res file fail");
                return false;
            }
        } else {
            if (zipFile.add(resourcesFile.c_str(), compressName.c_str(), ZipEntry::kCompressStored, &pEntry) != 0) {
                QUP_LOGI("[-] compress resources.arsc fail");
                return false;
            }
        }

        QUP_LOGI("[*] pEntry name = %s", pEntry->getFileName());

        resPath.clear();
        resCompressName.clear();

    }

    return true;
}
