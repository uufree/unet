/*************************************************************************
	> File Name: TimerEvent.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月18日 星期五 19时42分02秒
 ************************************************************************/

#include"TimerEvent.h"
#include<sys/timerfd.h>

namespace unet
{
    static int createTimefd()
    {
        int timefd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
        if(timefd < 0)
            unet::handleError(errno);
        return timefd;
    }

    static struct timespec howMuchTimeFromNow(const base::Time& lhs)
    {
            int64_t mic = lhs.getTime() - base::Time().getTime();
            if(mic < 100)
                mic = 100;

            struct timespec ts;
            ts.tv_sec = static_cast<time_t>(mic / base::Time::KMicroseconds);
            ts.tv_nsec = static_cast<long>((mic % base::Time::KMicroseconds)*1000);
            return ts;
    }
        
    TimerEvent::TimerEvent() :
        u_timerfd(createTimefd()),
        u_start(false),
        u_timerHeap(),
        u_mutex()
    {};

    TimerEvent::TimerEvent(TimerEvent&& event) :
        u_timerfd(::dup(event.u_timerfd)),
        u_start(false),
        u_timerHeap(std::move(event.u_timerHeap)),
        u_mutex()
    {};

    TimerEvent& TimerEvent::operator=(TimerEvent&& event)
    {
        if(*this == event)
            return *this;
        if(::close(u_timerfd) < 0)
            unet::handleError(errno);
        u_timerHeap.swap(event.u_timerHeap);
        u_start = false;
    }

    TimerEvent::~TimerEvent()
    {
        if(::close(u_timerfd) < 0)
            unet::handleError(errno);
        u_start = false;
    }
    
    void TimerEvent::addTimerWithLock(TimerPtr timer)
    {
        if(u_start == false)
            return;

        base::Time now;
        now.addTime(timer->getRepeatTime());
        
        if((u_timerHeap.size() > 0 && now.getTime() < u_timerHeap.top().first) || u_timerHeap.size() == 0)
        {
            struct itimerspec newSpec;
            bzero(&newSpec,sizeof(newSpec));
            struct itimerspec oldSpec;
            bzero(&oldSpec,sizeof(oldSpec));
            newSpec.it_value = howMuchTimeFromNow(now);
                
            if(::timerfd_settime(u_timerfd,0,&newSpec,&oldSpec) < 0)
                    unet::handleError(errno);
        }
        
        {
            base::MutexLockGuard guard(u_mutex);
            u_timerHeap.push(std::make_pair(now.getTime(),timer));
        }
    }
    
    void TimerEvent::addTimer(TimerPtr timer)
    {
        if(u_start == false)
            return;

        base::Time now;
        now.addTime(timer->getRepeatTime());
        
        if((u_timerHeap.size() > 0 && now.getTime() < u_timerHeap.top().first) || u_timerHeap.size() == 0)
        {
            struct itimerspec newSpec;
            bzero(&newSpec,sizeof(newSpec));
            struct itimerspec oldSpec;
            bzero(&oldSpec,sizeof(oldSpec));
            newSpec.it_value = howMuchTimeFromNow(now);
                
            if(::timerfd_settime(u_timerfd,0,&newSpec,&oldSpec) < 0)
                    unet::handleError(errno);
        }
        
        u_timerHeap.push(std::make_pair(now.getTime(),timer));
    }

    void TimerEvent::handleEvent()
    {
        if(u_start == false)
            return;

        ::read(u_timerfd,NULL,0);
        base::Time time;
        uint64_t now = time.getTime();
        std::vector<TimerPtr> handleList;

        base::MutexLockGuard guard(u_mutex);
        {
            while(u_timerHeap.top().first < now)
            {
                handleList.push_back(u_timerHeap.top().second); 
                if(u_timerHeap.top().second->isRepeat())
                    addTimer(u_timerHeap.top().second);
                u_timerHeap.pop();
            }
        }
            
        for(size_t i=0;i<handleList.size();++i)
            handleList[i]->run();
    }
    
    void TimerEvent::stop()
    {
        u_start = false;
        
        {
            base::MutexLockGuard guard(u_mutex);
            while(!u_timerHeap.empty())
                u_timerHeap.pop();
        }
    }
}

