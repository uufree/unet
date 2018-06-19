/*************************************************************************
	> File Name: ClockTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月29日 星期二 03时16分34秒
 ************************************************************************/

#include<iostream>
#include"../Clock.h"

int main(int argc,char** argv)
{
    unet::base::Clock clock;
    std::cout << clock.getMouth() << std::endl;
    std::cout << clock.getWeak() << std::endl;
    std::cout << clock.getDay() << std::endl;
    std::cout << clock.getTime() << std::endl;
    std::cout << clock.getYear() << std::endl;
    std::cout << clock.getHourAndMinutes() << std::endl;
    std::cout << clock.getMouthAndDays() << std::endl;
    return 0;
}
