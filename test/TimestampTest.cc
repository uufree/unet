/*************************************************************************
	> File Name: TimestampTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月27日 星期四 21时34分27秒
 ************************************************************************/
#include"../Timestamp.h"
#include<iostream>
using namespace unet::time;

int main(int argc,char** argv)
{
    Timestamp stamp;
    std::cout << "stamp:  " << stamp.getTime() << std::endl;
    
    sleep(1);
    Timestamp stamp1;
    std::cout << "stamp1: " << stamp1.getTime() << std::endl;

    stamp1.addTime(2);
    std::cout << "stamp1: " << stamp1.getTime() << std::endl;
    
    if(stamp1 < stamp)
        std::cout <<  "stamp1 > stamp" << std::endl;
    else
        std::cout << "stamp1 < stamp" << std::endl;

    return 0;
}

