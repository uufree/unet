/*************************************************************************
	> File Name: Timer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月27日 星期四 13时56分23秒
 ************************************************************************/

#include"Timer.h"
#include"../TimerEvent.h"

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
    }

    Timer::Timer(base::Time time,bool repeat,double repeatTime) :
        u_start(false),
        u_time(time),
        u_repeat(repeat),
        u_repeatTime(repeatTime),
        u_timeCallBack(),
        u_timers()
    {};

    Timer::Timer(bool repeat,double repeatTime) :
        u_start(false),
        u_time(base::Time()),
        u_repeat(repeat),
        u_repeatTime(repeatTime),
        u_timeCallBack(),
        u_timers()
    {};
        
    Timer::Timer(bool repeat,double repeatTime,const TimeCallBack& callback) :
        u_start(false),
        u_time(base::Time()),
        u_repeat(repeat),
        u_repeatTime(repeatTime),
        u_timeCallBack(callback),
        u_timers()
    {};

    Timer::Timer(Timer&& lhs) :
        u_start(false),
        u_time(base::Time()),
        u_repeat(lhs.u_repeat),
        u_repeatTime(lhs.u_repeatTime),
        u_timeCallBack(lhs.u_timeCallBack),
        u_timers(lhs.u_timers)
    {
        lhs.stop();
        lhs.u_start = false;
    };
          
    Timer& Timer::operator=(Timer&& lhs)
    {
        if(*this == lhs)
            return *this;
        lhs.stop();
        lhs.u_start = false;
            
        u_start = false;
        u_time = base::Time();
        u_repeat = lhs.u_repeat;
        u_repeatTime = lhs.u_repeatTime;
        u_timeCallBack = lhs.u_timeCallBack;
        u_timers = lhs.u_timers;

        return *this;
    }
        
    Timer::~Timer()
    {
        if(u_start)
            stop();
    }

    void Timer::start()
    {
        if(u_start)
            return;

        std::shared_ptr<TimerEvent> ptr = u_timers.lock();
        if(ptr)
        {
            ptr->addTimerWithLock(shared_from_this());    
            u_start = true;
        }               
    }

    void Timer::stop()
    {
        if(!u_start)
            return;
        
        /*Not-thread safety*/
        u_repeat = false;
        u_start = false;
    }
}

