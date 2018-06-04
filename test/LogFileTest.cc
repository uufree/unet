/*************************************************************************
	> File Name: LogFileTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月03日 星期日 20时32分47秒
 ************************************************************************/

#include<iostream>
#include"../LogFile.h"

using namespace unet;

int main(int argc,char** argv)
{
    ::putenv((char*)"NAME=a");
    LogFile logfile;
    std::cout << "LogFile fd: " << logfile.fd() << std::endl;
    std::cout << "LogFile: " << logfile.logFileName() << std::endl;

    return 0;
}
