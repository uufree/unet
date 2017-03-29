/*************************************************************************
	> File Name: ThreadPoolText.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月27日 星期一 23时44分05秒
 ************************************************************************/

#include<stdio.h>
#include"../ThreadPool.h"
#include<unistd.h>
#include<memory>
#include"../Thread.h"

using namespace unet;
using namespace thread;

void callBack(void)
{
    sleep(0.1);
    perror("I'm thread!\n");
}

void callback(void)
{
    sleep(0.1);
    perror("I'm thread2!\n");
}

int main(int argc,char** argv)
{
    ThreadPool pool(2);
    Thread thread;
    thread.setThreadCallBack(std::bind(&callBack));

//    pool.setThreadCallBack(std::move(thread));
    pool.setThreadCallBack(thread);
    pool.start(); 
    pool.joinAll();
  
    sleep(15);
    return 0;
}

