/*************************************************************************
	> File Name: LogFormatTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月03日 星期日 20时16分19秒
 ************************************************************************/

#include<iostream>
#include"../LogFormat.h"

using namespace unet;

int main(int argc,char** argv)
{
    ::putenv((char*)"NAME=a");
    LogFormat format;
    std::cout << pthread_self() << std::endl;
    std::cout << "Log Format: " << format.formatLog(::pthread_self(),INFO,"Hello,I'm Main Thread",__FILE__,__LINE__) << std::endl;
    std::cout << "Log File Format: " << format.formatLogFile("a","uuchen",2019) << std::endl;
    
    return 0;
}
