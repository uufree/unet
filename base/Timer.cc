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
        
        bool operator==(const Time& a,const Time& b)
        {return a.u_microseconds == b.u_microseconds;};

        bool operator<(const Time& a,const Time& b)
        {return a.u_microseconds < b.u_microseconds;};
    }

    Timer::Timer(base::Time time,bool repeat,double repeatTime,const TimerEventPtr& ptr) :
        u_start(false),
        u_time(time),
        u_repeat(repeat),
        u_repeatTime(repeatTime),
        u_timeCallBack(),
        u_timers(ptr)
    {};

    Timer::Timer(bool repeat,double repeatTime,const TimerEventPtr& ptr) :
        u_start(false),
        u_time(base::Time()),
        u_repeat(repeat),
        u_repeatTime(repeatTime),
        u_timeCallBack(),
        u_timers(ptr)
    {};
        
    Timer::Timer(bool repeat,double repeatTime,const TimeCallBack& callback,const TimerEventPtr& ptr) :
        u_start(false),
        u_time(base::Time()),
        u_repeat(repeat),
        u_repeatTime(repeatTime),
        u_timeCallBack(callback),
        u_timers(ptr)
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
        /*not-thread safety*/
        if(u_start)
            stop();
    }

    void Timer::start()
    {
        std::shared_ptr<unet::TimerEvent> ptr = u_timers.lock();
        if(ptr)
            ptr->addTimerWithLock(shared_from_this());        
    }
    
    /*为什么不通过调整repeat属性来关闭Timer？
     * 1.直接调整u_repeat是not-thread safety
     * 2.如果出现快速的停止，然后又重新加入，Timer会出现两次
     *解决方式1：
     * 在TimerEvent中维护一个deleteList，每次向判断repeat属性时，在deleteList中
     * 进行一次搜索，如果存在，将queue和list中的Timer删除；如果不存在，再次添加到
     * 观察链表中。
     * 与此同时，每次添加与删除Timer的时候，需要先对list进行搜索：
     *  a.添加：如果list中存在，就意味着准备删除Timer但是还没有删除，就是说Timer目
     * 前存在于queue中，从list中删除Timer即可，无需添加。如果list中不存在，说明
     * Timer不存在于任何队列中，可以直接添加
     *  b.删除：如果在list中存在，意味着上一次设置的定时器还没有到期，不做任何调
     *整。如果在list中不存在，添加到list中即可
     *
     *解决方式2：
     * 将Timer的start状态交由TimerEvent维护：
     * 1.start=falae，repeat=true：准备加入queue
     * 2.start=false，repeat=false：已经完全从queue中移除
     * 3.start=true，repeat=false：只在queue中运行一次的Timer
     * 4.start=true，repeat=true：正在运行
     *start：决定Timer现在是否存在于queue中，TimerEvent维护
     *repeat：决定Timer将来是否存在于queue中，用户维护
     */

    /*为了保护时间优先级队列的复杂度，采用方案2*/
    
    void Timer::stop()
    {
        /*not-thread safety*/
        if(!u_start)
            return;
        u_repeat = false;
    }
}

