#include "filemanage.h"
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <signal.h>
namespace sylar{
    /**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
 * @brief: 获取文件夹下的所有文件
 * @param files: 装载文件夹下的所有文件
 * @param path: 文件夹路径
 * @param subfix: 检索文件后缀
 * */
void FSUtil::ListAllFile(std::vector<std::string>& files
                            , const std::string& path
                            , const std::string& subfix) {
    /** 判断该文件夹是否存在 */
    //c_str() 函数可以将 const string* 类型 转化为 const char* 类型
    if(access(path.c_str(), 0) != 0)
        return;
    /** 打开文件夹 */
    DIR* dir = opendir(path.c_str());
    if(dir == nullptr)
        return;

    struct dirent* dp = nullptr;
    /* 循环读取文件夹下的文件名 */
    while ((dp = readdir(dir)) != nullptr) {
        /* 若该文件为文件夹 */
        if(dp->d_type == DT_DIR) {
            if(!strcmp(dp->d_name, "..")|| !strcmp(dp->d_name, "."))
                continue;       // 若该文件为该两者，则直接跳过
            ListAllFile(files, path + "/" + dp->d_name, subfix);    // 递归执行
        /* 若为常规文件 */
        }else if(dp->d_type == DT_REG) {
            std::string filename(dp->d_name);
            if(subfix.empty()) {    // 若没有指定文件后缀
                files.push_back(path + "/" + filename);
            }else {
                /* 判断文件后缀是否满足 */
                if(filename.size() < subfix.size()) {
                    continue;
                }
                if(filename.substr(filename.length() - subfix.size()) == subfix) {
                    files.push_back(path + "/" + filename);
                }
            }
        }
    }
    closedir(dir);
}

/** 获取文件信息 */
static int __lstat(const char* file, struct stat* st = nullptr) {
    struct stat lst;
    int ret = lstat(file, &lst);
    if(st) {
        *st = lst;
    }

    return ret;
}

/** 创建文件夹 */
static int __mkdir(const char* dirname) {
    /* 判断该文件夹是否存在 */
    if(access(dirname, F_OK) == 0) {
        return 0;
    }
    return mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

/** 创建文件夹 */
bool FSUtil::Mkdir(const std::string& dirname) {
    /* 检测文件夹是否存在 */
    if(__lstat(dirname.c_str()) == 0) {
        return true;
    }
    char* path = strdup(dirname.c_str());   // 获取一个新串副本 strdup()函数主要是拷贝字符串s的一个副本，由函数返回值返回
    char* ptr = strchr(path + 1, '/');  // 获取第一个出现/字符的位置 参数 str 所指向的字符串中搜索第一次出现字符 c
    /* 对文件路径进行切割，若出现多级目录，则创建多级目录 */
    do {
        for(; ptr; *ptr = '/', ptr = strchr(ptr + 1, '/')) {
            *ptr = '\0';
            if(__mkdir(path) != 0) {    // 创建文件夹
                break;
            }
        }
        if(ptr != nullptr) {
            break;
        }else if(__mkdir(path) != 0) {
            break;
        }
        free(path);
        return true;
    } while (0);
    free(path);

    return false;
}

bool FSUtil::IsRunningPidfile(const std::string& pidfile) {
    if(__lstat(pidfile.c_str()) != 0) {
        return false;
    }
    std::ifstream ifs(pidfile);
    std::string line;
    if(!ifs || !std::getline(ifs, line)) {
        return false;
    }
    if(line.empty()) {
        return false;
    }
    pid_t pid = atoi(line.c_str());
    if(pid <= 1) {
        return false;
    }
    if(kill(pid, 0)) {
        return false;
    }

    return true;
}

/**
 * @brief: 创建软连接
 * */
bool FSUtil::Symlink(const std::string& frm, const std::string& to) {
    if(!Rm(to)) {
        return false;
    }
    return ::symlink(frm.c_str(), to.c_str());
}

/**
 * @brief: 删除软连接
 * */
bool FSUtil::Unlink(const std::string& filename, bool exist) {
    if(!exist && __lstat(filename.c_str())) {
        return true;
    }
    return ::unlink(filename.c_str()) == 0;
}

/**
 * @brief: 删除文件
 * */
bool FSUtil::Rm(const std::string& path) {
    struct stat st;
    if(lstat(path.c_str(), &st)) {
        return true;
    }
    if(!(st.st_mode & S_IFDIR)) {
        return Unlink(path);
    }

    DIR* dir = opendir(path.c_str());
    if(!dir) {
        return false;
    }

    bool ret = true;
    struct dirent* dp = nullptr;
    while((dp = readdir(dir))) {
        if(!strcmp(dp->d_name, ".")|| !strcmp(dp->d_name, "..")) {
            continue;
        }
        std::string dirname = path + "/" + dp->d_name;
        ret = Rm(dirname);
    }
    closedir(dir);
    if(::rmdir(path.c_str())) {
        ret = false;
    }
    return ret;
}

/** 移动文件 */
bool FSUtil::Mv(const std::string& from, const std::string& to) {
    if(!Rm(to)) {
        return false;
    }
    return rename(from.c_str(), to.c_str()) == 0;
}

/** 获取真实路径 */
bool FSUtil::Realpath(const std::string& path, std::string& rpath) {
    if(__lstat(path.c_str())) {
        return false;
    }
    char* ptr = ::realpath(path.c_str(), nullptr);
    if(nullptr == ptr) {
        return false;
    }
    std::string(ptr).swap(rpath);
    free(ptr);

    return true;
}


bool FSUtil::OpenForRead(std::ifstream& ifs, const std::string& filename, std::ios_base::openmode mode) {
    ifs.open(filename.c_str(), mode);

    return ifs.is_open();
}

bool FSUtil::OpenForWrite(std::ofstream& ofs, const std::string& filename
        , std::ios_base::openmode mode) {
    ofs.open(filename.c_str(), mode);
    if(!ofs.is_open()) {
        std::string dir = Dirname(filename);
        Mkdir(dir);
        ofs.open(filename.c_str(), mode);
    }
    return ofs.is_open();
}

/** 返回文件名 */
std::string FSUtil::Dirname(const std::string& filename) {
    if(filename.empty()) {
        return ".";
    }
    auto pos = filename.rfind('/');
    if(pos == 0) {
        return "/";
    }else if(pos == std::string::npos) {
        return ".";
    }else {
        return filename.substr(0, pos);
    }
}

/** 返回文件名称 */
std::string FSUtil::Basename(const std::string& filename) {
    if(filename.empty()) {
        return filename;
    }
    auto pos = filename.rfind('/');
    if(pos == std::string::npos) {
        return filename;
    }else {
        return filename.substr(pos + 1);
    }
}

}