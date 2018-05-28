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
void threadFunc()
{
    NewBlock newBlock;
    while(1)
    {
        std::cout << "1" << std::endl;
        ::sleep(1);
    }
}

/*线程的启动与终止测试*/
void startAndStopTest()
{
    unet::base::Thread thread;
    thread.setThreadCallBack(std::bind(&threadFunc));
    thread.start();
    ::sleep(5);
    thread.stop();
    ::sleep(2);
    thread.start();
    ::sleep(5);
    thread.stop();
}

int main(int argc,char** argv)
{
    startAndStopTest();
    return 0;
}
