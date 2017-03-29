/*************************************************************************
	> File Name: ThreadText1.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月28日 星期二 18时03分16秒
 ************************************************************************/

#include"../Thread.h"
#include<stdio.h>
#include<unistd.h>

using namespace unet;
using namespace unet::thread;

void callBack()
{   
    perror("I'm thrad!");
    sleep(5);
}
    

int main(int argc,char** argv)
{
    Thread thread;
    thread.setThreadCallBack(std::bind(&callBack));
//    Thread thread1(std::move(thread));
    Thread thread1;
    thread1 = std::move(thread);
    thread1.start();

    sleep(10);
    return 0;
}


