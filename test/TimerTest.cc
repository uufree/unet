/*************************************************************************
	> File Name: TimerTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月31日 星期四 17时22分30秒
 ************************************************************************/

#include<iostream>
#include<cstdio>
#include"../Timer.h"

using namespace unet::base;

int main(int argc,char** argv)
{
    int number = 10;
    Time time1;
    for(int i=0;i<1000000;i++)
        std::cout << "Hello,Server! " << number << std::endl;
    Time time2;

    uint64_t cppDiff = time2.getTime() - time1.getTime();
    
    Time time3;
    for(int i=0;i<1000000;i++)
        printf("Hello,Server! %d\n",number);
    Time time4;

    uint64_t cDiff = time4.getTime() - time3.getTime();

    std::cout << "CppDiff: " << cppDiff << std::endl;
    std::cout << "CDiff: " << cDiff << std::endl;

    return 0;
}
