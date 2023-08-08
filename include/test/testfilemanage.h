#include <iostream>
#include "../log.h"
#include "../filemanage.h"
#include "../thread.h"
#include "../util.h"
#include "yaml-cpp/yaml.h"

namespace sylar{
       void test_filemanage(){
        //
        sylar::FSUtil fds;

        //
        std::vector<std::string>files;
        std :: string path = "/home/longsl/test01";
        std :: string subfix;
        //std :: string subfix = "ex";
        fds.ListAllFile(files , path , subfix);
        for(auto s : files){
            std :: cout << s << std :: endl;
        }

        //
        std :: string mkdr = "/home/longsl/test01/mkd01";
        fds.Mkdir(mkdr);

        //
        std :: string runfile = "/home/longsl/test01/te1.c";
        if(fds.IsRunningPidfile(runfile))std :: cout << "Y\n";
        else std :: cout << "N\n";

        //
    //    std :: string linkfr = "/home/longsl/test01/k01";
    //    std :: string linkto = "/home/longsl/test01/k02";
    //    fds.Symlink(linkfr,linkto);
    //    fds.Unlink(linkto , 1);

        // 
        fds.Rm(mkdr);

        //
        // std :: string mvfr = "/home/longsl/test01/k03";
        // std :: string mvto = "/home/longsl/test01/testdir/k03";
        // fds.Mv(mvfr , mvto);


    }
}