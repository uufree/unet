/*************************************************************************
	> File Name: AllocTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月30日 星期三 01时04分28秒
 ************************************************************************/

#include<iostream>
#include<cstdio>
#include"../Alloc.h"
#include"../Thread.h"

using namespace unet;

/*单线程测试*/
void singleThread()
{
    std::cout << "Init Size: " << std::endl;
    std::cout << alloc::getLogBufferSize() << std::endl;
    std::cout << alloc::getUsrBufferSize() << std::endl;
    
    for(int i=0;i<1024;i++)
    {
        UsrBuffer* usr = alloc::allocUsrBuffer();
        std::cout << "Usr Buffer Size: " << alloc::getUsrBufferSize()  << std::endl;
    }
}

/*多线程测试*/
void threadFuncA()
{
    UsrBuffer* usr = NULL;
    LogBuffer* log = NULL;

    for(int i=0;i<1000;i++)
    {
        usr = alloc::allocUsrBuffer();
        log = alloc::allocLogBuffer();
        alloc::deallocLogBuffer(log);
        alloc::deallocUsrBuffer(usr);

        printf("Log Buffer Size: %d\n",alloc::getLogBufferSize());
        printf("Usr Buffer Size: %d\n",alloc::getUsrBufferSize());
    }
}

void threadFuncB()
{
    UsrBuffer* usr = NULL;
    LogBuffer* log = NULL;

    for(int i=0;i<1000;i++)
    {
        usr = alloc::allocUsrBuffer();
        log = alloc::allocLogBuffer();
        alloc::deallocLogBuffer(log);
        alloc::deallocUsrBuffer(usr);

        printf("Log Buffer Size: %d\n",alloc::getLogBufferSize());
        printf("Usr Buffer Size: %d\n",alloc::getUsrBufferSize());
    }
}

void threadFuncC()
{
    UsrBuffer* usr = NULL;
    LogBuffer* log = NULL;

    for(int i=0;i<1000;i++)
    {
        usr = alloc::allocUsrBuffer();
        log = alloc::allocLogBuffer();
        alloc::deallocLogBuffer(log);
        alloc::deallocUsrBuffer(usr);

        printf("Log Buffer Size: %d\n",alloc::getLogBufferSize());
        printf("Usr Buffer Size: %d\n",alloc::getUsrBufferSize());
    }
}
void threadFuncD()
{
    UsrBuffer* usr = NULL;
    LogBuffer* log = NULL;

    for(int i=0;i<1000;i++)
    {
        usr = alloc::allocUsrBuffer();
        log = alloc::allocLogBuffer();
        alloc::deallocLogBuffer(log);
        alloc::deallocUsrBuffer(usr);

        printf("Log Buffer Size: %d\n",alloc::getLogBufferSize());
        printf("Usr Buffer Size: %d\n",alloc::getUsrBufferSize());
    }
}

void mutilThread()
{
    base::Thread threadA(std::bind(&threadFuncA));
    base::Thread threadB(std::bind(&threadFuncB));
    base::Thread threadC(std::bind(&threadFuncC));
    base::Thread threadD(std::bind(&threadFuncD));
    threadA.start();
    threadB.start();
    threadC.start();
    threadD.start();

    sleep(5);
    
    printf("===============================\n");
    printf("Log Buffer Size: %d\n",alloc::getLogBufferSize());
    printf("Usr Buffer Size: %d\n",alloc::getUsrBufferSize());
}

/*Log Buffer写入测试*/
void logBufferTest()
{
    LogBuffer* log = alloc::allocLogBuffer();
    int x = 20500;
    while(x--)
        writeInLogBuffer(log,"uuchen",6);
    int res = x * 6;
    std::cout << "res: " << res << std::endl;
    std::cout << "offset: " << log->u_data - log->u_buf << std::endl;
    alloc::deallocLogBuffer(log);
}

int main(int argc,char** argv)
{
    logBufferTest();
    return 0;
}


