/* 
 * File:   main.cpp
 * Author : angelToms
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <errno.h>

#include "mixer/Mixer.h"
#include "mixer/ArscMixer.h"
#include "zipapk/ApkZipOp.h"
#include "utils/FileScan.h"
#include "Common.h"
#include "QupLog.h"

#define ONLY_PARSE_NOT_MIXUP

static void usage() {
    QUP_LOGI("Usage :\n"
            "           arscmaker -t [python | bin] -p path -o [mixup | parse]\n"
            "           -t: type, -p: path, -o: operate\n"
            "           -t python:\n"
            "           use python zip tools\n"
            "           -t bin:\n"
            "               use system bin zip tools\n"
            "           path:\n"
            "               like /tmp/test/a.apk(mixup apk) or /tmp/test/(parse resources.arsc)\n"
            "           -o mixup:\n"
            "               mixup apk res and create a new apk, path must contain apk, like a.apk\n"
            "           -o parse:\n"
            "               only parse resources.arsc,if path contains apk, it will unzip and parse res, and remove it at last\n"
            "           ex:\n"
            "               arscmaker -t python -p /tmp/test/a.apk -o mixup");
    exit(-1);
}

static void onlyParse(std::string ppath) {

    std::string rstrdir(ppath);
    rstrdir.append("resources.arsc");

    std::string rfstrdir(ppath);
    rfstrdir.append("res");

    const char* rdir = rstrdir.c_str();
    const char* rfdir = rfstrdir.c_str();
    bool isCopy = false;
    //parse全局日志开关
    bool isLogAll = true;
    //creator 全局日志开关， parse 二级日志开关(parse打印更详细的日志)
    bool isLog = true;
    //是否仅仅解析资源文件，而不是解析+混淆
    bool isOnlyParse = true;

#ifdef ONLY_PARSE_NOT_MIXUP
    isOnlyParse = true;
#endif

    Mixer* mixer = new ArscMixer(TYPE_RESOURCE_ARSC,
            isCopy,
            isLogAll,
            isLog,
            isOnlyParse,
            rdir,
            rfdir);

    if (!mixer) {
        QUP_LOGI("[-] calloc mixer fail");
        exit(-1);
    }

    if (!mixer->mixer()) {
        exit(-1);
    }

    delete mixer;
    mixer = NULL;
}

static void useBinParse(std::string apkpath) {

    std::string apkstring(apkpath);
    apkstring = apkstring.substr(0, apkstring.find(".apk"));
    QUP_LOGI("[*] apk path = %s", apkstring.c_str());

    std::string command("unzip -o -d ");
    command.append(apkstring).append(" ").append(apkpath);
    QUP_LOGI("[*] unzip command = %s", command.c_str());

    if (system(command.c_str()) < 0) {
        QUP_LOGI("[-] unzip apk fail %s", strerror(errno));
        exit(-1);
    }

    const char* basedir = apkstring.c_str();
    std::string rstrdir(basedir);
    rstrdir.append("/");
    rstrdir.append("resources.arsc");

    std::string rfstrdir(basedir);
    rfstrdir.append("/");
    rfstrdir.append("res");

    const char* rdir = rstrdir.c_str();
    const char* rfdir = rfstrdir.c_str();

    QUP_LOGI("[*] resource dir = %s, res file dir = %s,"
            , rdir, rfdir);

    bool isCopy = false;
    //parse全局日志开关
    bool isLogAll = true;
    //creator 全局日志开关， parse 二级日志开关(parse打印更详细的日志)
    bool isLog = true;
    //是否仅仅解析资源文件，而不是解析+混淆
    bool isOnlyParse = true;

#ifdef ONLY_PARSE_NOT_MIXUP
    isOnlyParse = true;
#endif

    Mixer* mixer = new ArscMixer(TYPE_RESOURCE_ARSC,
            isCopy,
            isLogAll,
            isLog,
            isOnlyParse,
            rdir,
            rfdir);

    if (!mixer) {
        QUP_LOGI("[-] calloc mixer fail");
        exit(-1);
    }

    if (!mixer->mixer()) {
        exit(-1);
    }

    delete mixer;
    mixer = NULL;
}

static void usePythonZipParse(std::string ppath) {

    onlyParse(ppath);

    QUP_LOGI("[*] iter remove %s ", ppath.c_str());
    //iter remove unzip file
    if (!iter_del_dir(ppath.c_str(), 0)) {
        QUP_LOGI("[-] iter remove %s fail", ppath.c_str());
        exit(-1);
    }

    rmdir(ppath.c_str());
}

static void useBinMixup(std::string apkpath) {
    std::string filename(apkpath);
    filename = filename.substr((filename.rfind("/") + 1));
    filename = filename.substr(0, filename.rfind(".")).append(".new.apk");
    QUP_LOGI("[*] new apk name = %s", filename.c_str());

    std::string apkstring(apkpath);
    apkstring = apkstring.substr(0, apkstring.find(".apk"));
    QUP_LOGI("[*] apk path = %s", apkstring.c_str());

    std::string command("unzip -o -d ");
    command.append(apkstring).append(" ").append(apkpath);
    QUP_LOGI("[*] unzip command = %s", command.c_str());

    if (system(command.c_str()) < 0) {
        QUP_LOGI("[-] unzip apk fail %s", strerror(errno));
        exit(-1);
    }

    const char* basedir = apkstring.c_str();
    std::string rstrdir(basedir);
    rstrdir.append("/");
    rstrdir.append("resources.arsc");

    std::string rfstrdir(basedir);
    rfstrdir.append("/");
    rfstrdir.append("res");

    std::string metadir(basedir);
    metadir.append("/");
    metadir.append("META-INF");

    const char* rdir = rstrdir.c_str();
    const char* rfdir = rfstrdir.c_str();

    QUP_LOGI("[*] resource dir = %s, res file dir = %s,"
            "META-INF dir = %s", rdir, rfdir, metadir.c_str());

    bool isCopy = false;
    //parse全局日志开关
    bool isLogAll = false;
    //creator 全局日志开关， parse 二级日志开关(parse打印更详细的日志)
    bool isLog = true;
    //是否仅仅解析资源文件，而不是解析+混淆
    bool isOnlyParse = false;

#ifdef ONLY_PARSE_NOT_MIXUP
    isOnlyParse = true;
#endif

    Mixer* mixer = new ArscMixer(TYPE_RESOURCE_ARSC,
            isCopy,
            isLogAll,
            isLog,
            isOnlyParse,
            rdir,
            rfdir);

    if (!mixer) {
        QUP_LOGI("[-] calloc mixer fail");
        exit(-1);
    }

    if (!mixer->mixer()) {
        exit(-1);
    }

    //remove resources.arsc and rename resources.arsc.1 to resources.arsc
    if (unlink("resources.arsc") != 0) {
        QUP_LOGI("[-] delete backed resources.arsc fail %s", strerror(errno));
        exit(-1);
    }

    if (rename("resources.arsc.1", "resources.arsc") != 0) {
        QUP_LOGI("[-] rename resources.arsc.1 to resources.arsc fail %s", strerror(errno));
        exit(-1);
    }

    QUP_LOGI("[*] delete META-INF");
    //iter remove META-INF
    if (!iter_del_dir(metadir.c_str(), 0)) {
        QUP_LOGI("[-] iter remove META-INF fail");
        exit(-1);
    }
    rmdir(metadir.c_str());

    //    zip -r sgsearch.zip *
    if (chdir(basedir) != 0) {
        QUP_LOGI("[-] cd dir %s fail %s", basedir, strerror(errno));
        exit(-1);
    }

    command = std::string("zip -r ").append(filename).append(" *");
    QUP_LOGI("[*] zip command = %s", command.c_str());
    if (system(command.c_str()) < 0) {
        QUP_LOGI("[-] zip apk fail %s", strerror(errno));
        exit(-1);
    }

    command = std::string("cp ").append(filename).append(" ../").append(filename);
    QUP_LOGI("[*] cp command = %s", command.c_str());
    if (system(command.c_str()) < 0) {
        QUP_LOGI("[-] cp new apk fail %s", strerror(errno));
        exit(-1);
    }

    QUP_LOGI("[*] iter remove %s ", basedir);
    //iter remove unzip file
    if (!iter_del_dir(basedir, 0)) {
        QUP_LOGI("[-] iter remove %s fail", basedir);
        exit(-1);
    }

    rmdir(basedir);

    delete mixer;
    mixer = NULL;
}

static void usePythonZipMixup(std::string ppath) {
    std::string rstrdir(ppath);
    rstrdir.append("resources.arsc");

    std::string rfstrdir(ppath);
    rfstrdir.append("res");

    const char* rdir = rstrdir.c_str();
    const char* rfdir = rfstrdir.c_str();
    bool isCopy = false;
    //parse全局日志开关
    bool isLogAll = false;
    //creator 全局日志开关， parse 二级日志开关(parse打印更详细的日志)
    bool isLog = true;
    //是否仅仅解析资源文件，而不是解析+混淆
    bool isOnlyParse = false;

#ifdef ONLY_PARSE_NOT_MIXUP
    isOnlyParse = true;
#endif

    Mixer* mixer = new ArscMixer(TYPE_RESOURCE_ARSC,
            isCopy,
            isLogAll,
            isLog,
            isOnlyParse,
            rdir,
            rfdir);

    if (!mixer) {
        QUP_LOGI("[-] calloc mixer fail");
        exit(-1);
    }

    if (!mixer->mixer()) {
        exit(-1);
    }

    delete mixer;
    mixer = NULL;
}

int main(int argc, char** argv) {

    if (argc < 7) {
        usage();
    }

    bool isPythonTools = true;
    bool isMixup = true;
    bool isPathConApk = false;

    std::string path("");

    int opt;
    char *optstring = (char*) "t:p:o:";
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        QUP_LOGI("[*] opt = %c, optarg = %s", opt, optarg);
        switch (opt) {
            case 't':
                if (strcmp(optarg, "python") != 0 && strcmp(optarg, "bin") != 0)
                    usage();
                if (strcmp(optarg, "bin") == 0) {
                    isPythonTools = false;
                }
                break;
            case 'p':
                path = std::string(optarg);
                break;
            case 'o':
                if (strcmp(optarg, "mixup") != 0 && strcmp(optarg, "parse") != 0)
                    usage();
                if ((strcmp(optarg, "mixup") == 0 && !isPythonTools) && path.find(".apk") == std::string::npos) {
                    usage();
                }
                if (strcmp(optarg, "parse") == 0) {
                    isMixup = false;
                }
                break;
        }
    }

    if (path.find(".apk") != std::string::npos)
        isPathConApk = true;

    QUP_LOGI("[*] use python zip tools = %d, "
            "path = %s, operate mixup = %d,"
            " path contain .apk = %d",
            isPythonTools, path.c_str(), isMixup, isPathConApk);

    std::string apkPath("");
    if (isPathConApk) {
        apkPath.append(path.substr(0, (path.rfind("/") + 1)));
    } else {
        apkPath.append(path).append("/");
    }

    QUP_LOGI("[*] parent path = %s", apkPath.c_str());

    if (!isMixup) {
        if (isPathConApk) {
            if (!isPythonTools)
                useBinParse(path);
            else
                usePythonZipParse(apkPath);
        } else
            onlyParse(apkPath);
    } else {
        if (!isPythonTools)
            useBinMixup(path);
        else
            usePythonZipMixup(apkPath);
    }

    return 0;
}
