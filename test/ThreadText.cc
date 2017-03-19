/*************************************************************************
	> File Name: Thread.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月26日 星期日 00时35分15秒
 ************************************************************************/

#include"../Thread.h"
#include"../Mutex.h"
#include"../CurrentThread.h"
#include<iostream>
#include<unistd.h>

class node
{
    public:
        node() : m(10),lock(),cond(lock){};
        ~node() {};

        void wait()
        {
            unet::thread::MutexLockGuard guard(lock);
            while(m > 0)
                cond.wait();
        }

        void countDown()
        {
            unet::thread::MutexLockGuard guard(lock);
            --m;
            if(m == 0)
                cond.notify();
        }
        
        int getCount()
        {
            unet::thread::MutexLockGuard guard(lock);
            return m;
        };

    private:
        int m;
        unet::thread::MutexLock lock;
        unet::thread::Condition cond;
};

class node mkl;

void func(int m,int n,int x)
{
    while(1)
    {
        if(mkl.getCount() != 0)
        {
            mkl.countDown();
            std::cout << mkl.getCount() << " ";
            std::cout << "thread runing!" << std::endl;
            ::sleep(1);
        }
    }
}


int main(int argc,char** argv)
{
    unet::thread::Thread thread(std::bind(&func,3,1,4));
    thread.start();
    
    while(1)
    {
        mkl.wait();
        std::cout << "main wake up!" << std::endl;
        sleep(1);
    }
    return 0;
}







































