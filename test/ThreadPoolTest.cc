/*************************************************************************
	> File Name: ThreadPoolTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月02日 星期六 10时14分15秒
 ************************************************************************/

#include<iostream>
#include"../ThreadPool.h"
#include"../TimerEvent.h"

using namespace unet;
using namespace unet::base; 

void threadFuncA()
{
    while(1)
    {
        printf("Pthread ID: %ld   A\n",pthread_self());
        sleep(1);
    }
}

void threadFuncB()
{
    while(1)
    {
        printf("Pthread ID: %ld   B\n",pthread_self());
        sleep(1);
    }
}

void threadFuncC()
{
    while(1)
    {
        printf("Pthread ID: %ld   C\n",pthread_self());
        sleep(1);
    }
}

int main(int argc,char** argv)
{
    ThreadPool pool;
    pool.addThread(std::bind(&threadFuncA),3,"Thread A");
    pool.addThread(std::bind(&threadFuncB),2,"Thread B");
    
    pool.printThreadPoolMessage();
    
    pool.startAllThread();
    
    pool.printThreadPoolMessage();

    sleep(3);
    
    pool.setThreadFunc("Thread B",std::bind(&threadFuncC));
    pool.printThreadPoolMessage();
    
    sleep(3);
    pool.stopAllThread();

    while(1);

    return 0;
}
