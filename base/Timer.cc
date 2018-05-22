/*************************************************************************
	> File Name: Timer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月27日 星期四 13时56分23秒
 ************************************************************************/

#include"Timer.h"

#include<time.h>
#include<sys/time.h>
#include<unistd.h>

namespace unet
{
    namespace base
    {
        Time::Time()
        {
            struct timeval tv;
            ::gettimeofday(&tv,NULL);
            u_microseconds = tv.tv_sec * Time::KMicroseconds + tv.tv_usec;
        }

        Timer::Timer(Time time,bool repeat,double repeatTime) :
            u_time(time),
            u_repeat(repeat),
            u_repeatTime(repeatTime)
        {};

        Timer::Timer(bool repeat,double repeatTime) :
            u_time(Time()),
            u_repeat(repeat),
            u_repeatTime(repeatTime)
        {};
        
        Timer::Timer(bool repeat,double repeatTime,const TimeCallBack& callback) :
            u_time(Time()),
            u_repeat(repeat),
            u_repeatTime(repeatTime),
            u_timeCallBack(callback)
        {};

        Timer::Timer(Timer&& lhs) :
            u_time(std::move(lhs.u_time)),
            u_repeat(lhs.u_repeat),
            u_repeatTime(lhs.u_repeatTime),
            u_timeCallBack(std::move(lhs.u_timeCallBack))
        {};
          
        Timer& Timer::operator=(Timer&& lhs)
        {
            if(*this == lhs)
                return *this;
            u_time = std::move(lhs.u_time);
            u_repeat = lhs.u_repeat;
            u_repeatTime = lhs.u_repeatTime;
            u_timeCallBack = std::move(lhs.u_timeCallBack);
            
            return *this;
        }
    }
}

