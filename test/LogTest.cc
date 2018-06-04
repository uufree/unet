/*************************************************************************
	> File Name: LogTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月03日 星期日 16时09分50秒
 ************************************************************************/

#include<iostream>
#include"../Log.h"
#include"../base/Thread.h"
#include<unistd.h>

using namespace unet;

void ThreadA()
{
    unsigned long tid = ::pthread_self();
    std::cout << "Thread A Start: " << tid  << std::endl;
    for(int i=0;i<10240;i++)
        LOG_INFO(tid,"Hello,I'm Thread A!");
}

void ThreadB()
{
    unsigned long tid = ::pthread_self();
    std::cout << "Thread B Start: " << tid  << std::endl;
    for(int i=0;i<10240;i++)
        LOG_INFO(tid,"Hello,I'm Thread B!");
}

int main(int argc,char** argv)
{
    ::putenv((char*)"NAME=a");
    unsigned long tid = ::pthread_self();
    LogStart();
    base::Thread threadA(std::bind(&ThreadA));
    base::Thread threadB(std::bind(&ThreadB));
    threadA.start();
    threadB.start();
    
    std::cout << "Main Thread Start: " << tid << std::endl;
    for(int i=0;i<10;i++)
        LOG_INFO(tid,"Hello,I'm Thread Main!");
    
    LogStop();
    return 0;
}
