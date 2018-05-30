/*************************************************************************
	> File Name: ThreadTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 21时49分03秒
 ************************************************************************/

#include<iostream>
#include<cstdlib>
#include<unistd.h>
#include"../Thread.h"
#include"../Mutex.h"
#include"../SpinLock.h"
#include"../RDMutex.h"
#include"../Condition.h"
#include<cstdio>

int threadNum = 0;
unet::base::MutexLock mutex;
unet::base::SpinLock spin;
unet::base::RDMutexLock rdlock;
unet::base::Condition cond(mutex);

class NewBlock
{
    public:
        NewBlock(){std::cout << "new" << std::endl;};
        ~NewBlock(){std::cout << "delete" << std::endl;};
};

/*基本的初始化与销毁*/
void mutexInit()
{
    unet::base::MutexLock mutex;
    unet::base::MutexLockGuard guard(mutex);
}

/*线程函数*/
/*测试线程中对象的生命周期*/
/*在短临界区的情况下，SpinLock性能优于MutexLock。好吧，原因：SpinLock没有pthread_self系统调用的开销*/
/*Mutex测试函数*/
void threadFunc()
{
    while(1)
    {
        /*MutexLock测试*/
//        unet::base::MutexLock llock = std::move(mutex);
//        unet::base::MutexLockGuard guard(llock);
//        unet::base::MutexLock lock(std::move(llock));
//        unet::base::MutexLockGuard guard(llock);
//        unet::base::MutexLockGuard guard(mutex);    
        /*SpinLock测试*/
//        unet::base::SpinLock sslock(std::move(spin));
//        unet::base::SpinLock slock = std::move(spin);
//        unet::base::SpinLockGuard guard(slock);
        /*RDMutexLock测试*/
//        unet::base::RDMutexLock rdlocks = std::move(rdlock);
//        unet::base::RDMutexLock rdlockss(std::move(rdlock));
//        unet::base::RDMutexLockGuard guard(rdlockss,unet::base::WRLOCK);
        ++threadNum;
        printf("%ld    %d\n",::pthread_self(),threadNum);
    }
}

void mutexTest()
{
    unet::base::Thread threadA(std::bind(&threadFunc));
    unet::base::Thread threadB(std::bind(&threadFunc));
    threadA.start();
    threadB.start();
    ::sleep(5);
}

/*cond测试函数*/
void threadFuncA()
{
    while(1)
    {
        unet::base::MutexLockGuard guard(mutex);
        unet::base::Condition conds(std::move(cond));
        while(threadNum % 2 == 0)
            conds.wait();
        ++threadNum;
        printf("%ld    %d\n",::pthread_self(),threadNum);
        conds.notifyAll();
    }
}

void threadFuncB()
{
    while(1)
    {
        unet::base::MutexLockGuard guard(mutex);
        unet::base::Condition conds(std::move(cond));
        while(threadNum % 2 == 1)
            conds.wait();
        ++threadNum;
        printf("%ld    %d\n",::pthread_self(),threadNum);
        conds.notify();
    }
}


void condTest()
{
    unet::base::Thread threadA(std::bind(&threadFuncA));
    unet::base::Thread threadB(std::bind(&threadFuncB));
    threadA.start();
    threadB.start();
    ::pause();
}


/*线程测试*/
/*正常运行无错误*/
void startAndStopTest()
{
    unet::base::Thread thread;
    thread.setThreadCallBack(std::bind(&threadFunc));
    thread.start();
    ::sleep(5);
    std::cout << "Thread ID: " << thread.getThreadId() << std::endl;
    std::cout << "Thread isStart: " << thread.isStart() << std::endl;
    thread.stop();
    std::cout << "Thread ID: " << thread.getThreadId() << std::endl;
    std::cout << "Thread isStart: " << thread.isStart() << std::endl;
    
    std::cout << "--------------------------------------" << std::endl;

    unet::base::Thread threada(std::move(thread));
    unet::base::Thread threads = std::move(threada);
    threads.start();
    ::sleep(5);
    std::cout << "Thread ID: " << threads.getThreadId() << std::endl;
    std::cout << "Thread isStart: " << threads.isStart() << std::endl;
    threads.stop();
    std::cout << "Thread ID: " << threads.getThreadId() << std::endl;
    std::cout << "Thread isStart: " << threads.isStart() << std::endl;
}

int main(int argc,char** argv)
{
    condTest();
    return 0;
}
