/*************************************************************************
	> File Name: TimerEventTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月01日 星期五 16时48分19秒
 ************************************************************************/

#include<iostream>
#include"../TimerEvent.h"
#include"../base/Thread.h"

using namespace unet;
using namespace unet::base; 

void timer1sCallBack()
{std::cout << "1s" << std::endl;}

void timer2sCallBack()
{std::cout << "2s" << std::endl;}

void timer3sCallBack()
{std::cout << "3s" << std::endl;}

Timer::TimerEventPtr timerEventPtr(std::make_shared<TimerEvent>());
Timer::TimerPtr time1(std::make_shared<Timer>(true,1,std::bind(&timer1sCallBack),timerEventPtr));
Timer::TimerPtr time2(std::make_shared<Timer>(true,2,std::bind(&timer2sCallBack),timerEventPtr));
Timer::TimerPtr time3(std::make_shared<Timer>(true,3,std::bind(&timer3sCallBack),timerEventPtr));

/*单线程测试*/
void TimerSignal()
{
    time1->start();
    time2->start();
    time3->start();
}

int main(int argc,char** argv)
{
    timerEventPtr->start();
    
    /*多线程测试*/
    Thread threadA(std::bind(&TimerSignal));
    threadA.start();

    for(int i=0;i<5;i++)
    {
        timerEventPtr->handleEvent();
        sleep(1);
    }

    time2->stop();
    while(1)
    {
        timerEventPtr->handleEvent();
        sleep(1);
    }

    return 0;
}
