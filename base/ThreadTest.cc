/*************************************************************************
	> File Name: ThreadTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 21时49分03秒
 ************************************************************************/

#include<iostream>
#include"Thread.h"
#include"global.h"

void threadFunc()
{
    while(1)
    {
        std::cout << "1" << std::endl;
        ::sleep(1);
    }
}

int main(int argc,char** argv)
{
    unet::base::Thread thread;
    thread.setThreadCallBack(std::bind(&threadFunc));
    thread.start();
    ::sleep(5);
    thread.join();
    while(1);
}
