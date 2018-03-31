/*************************************************************************
	> File Name: TimerMap.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年04月01日 星期日 01时04分44秒
 ************************************************************************/

#include"TimerMap.h"
#include"error.h"

namespace unet
{
    static int createTimefd()
    {
        int timefd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
        if(timefd < 0)
            unet::handleError(errno);
        return timefd;
    }

    struct timespec howMuchTimeFromNow(const base::Time& lhs)
    {
            int64_t mic = lhs.getTime() - base::Time().getTime();
            if(mic < 100)
                mic = 100;

            struct timespec ts;
            ts.tv_sec = static_cast<time_t>(mic / base::Time::KMicroseconds);
            ts.tv_nsec = static_cast<long>((mic % base::Time::KMicroseconds)*1000);
            return ts;
    }
        
    TimerMap::TimerMap() : 
        _timefd(createTimefd()),
        _timerMaps(),
        _start(false),
        _mutex()
    {};

    TimerMap::TimerMap(TimerMap&& lhs) : 
        _timefd(createTimefd()),
        _timerMaps(),
        _start(false),
        _mutex()
    {};

    TimerMap& TimerMap::operator=(TimerMap&& lhs)
    {
        if(*this == lhs)
            return *this;
        
        _timerMaps.clear();
        _timerMaps = std::move(lhs._timerMaps);
        _start = false;

        return *this;
    }

        
    TimerMap::~TimerMap()
    {
        ::close(_timefd);
        _timerMaps.clear();
    }

    void TimerMap::addTimer(TimerPtr&& timer)
    {
        base::Time now;
        now.addTime(timer->getRepeatTime());
            
        if(_timerMaps.size() > 0)
        {//setTimer
            if(now < _timerMaps.begin()->first)
            {
                struct itimerspec newSpec;
                bzero(&newSpec,sizeof(newSpec));
                struct itimerspec oldSpec;
                bzero(&oldSpec,sizeof(oldSpec));
                newSpec.it_value = howMuchTimeFromNow(now);
                
                if(::timerfd_settime(_timefd,0,&newSpec,&oldSpec) < 0)
                    unet::handleError(errno);
            }
        }
        else if(_timerMaps.size() == 0)
        {
            struct itimerspec newSpec;
            bzero(&newSpec,sizeof(newSpec));
            struct itimerspec oldSpec;
            bzero(&oldSpec,sizeof(oldSpec));
            newSpec.it_value = howMuchTimeFromNow(now);
                
            if(::timerfd_settime(_timefd,0,&newSpec,&oldSpec) < 0)
                unet::handleError(errno);
        }
        else
        {};

        {
            base::MutexLockGuard guard(_mutex);
            _timerMaps.insert({now,timer});
        }
    }

    void TimerMap::addTimer(base::Time&& time,TimerPtr&& ptr)
    {
        if(_timerMaps.size() > 0)
        {//setTimer
            if(time < _timerMaps.begin()->first)
            {
                struct itimerspec newSpec;
                bzero(&newSpec,sizeof(newSpec));
                struct itimerspec oldSpec;
                bzero(&oldSpec,sizeof(oldSpec));

                newSpec.it_value = howMuchTimeFromNow(time);
                if(::timerfd_settime(_timefd,0,&newSpec,&oldSpec) < 0)
                    unet::handleError(errno);
            }
        }
        else if(_timerMaps.size() == 0)
        {
            struct itimerspec newSpec;
            bzero(&newSpec,sizeof(newSpec));
            struct itimerspec oldSpec;
            bzero(&oldSpec,sizeof(oldSpec));

            newSpec.it_value = howMuchTimeFromNow(time);
            if(::timerfd_settime(_timefd,0,&newSpec,&oldSpec) < 0)
                unet::handleError(errno);
        }
        else
        {};

        {
            base::MutexLockGuard guard(_mutex);
            _timerMaps.insert({time,ptr});
        }
    }

    void TimerMap::handleRead()
    {
        ::read(_timefd,NULL,0);
            
        std::vector<base::Time> list;
        base::Time now;
            
        for(auto iter=_timerMaps.begin();iter!=_timerMaps.end();++iter)
        {
            if(iter->first < now)
            {
                iter->second->run();
                    
                if(iter->second->isRepeat())
                {
                    list.push_back(iter->first);
                    base::Time time = iter->first;
                    time.addTime(iter->second->getRepeatTime());
                    addTimer(std::move(time),std::move(iter->second));
                }                        
            }
            else
                break;
        }

        {
            base::MutexLockGuard guard(_mutex);
            for(auto iter=list.cbegin();iter!=list.cend();++iter)
                _timerMaps.erase(*iter);
        }
    }

    void TimerMap::start()
    {
    }

    void TimerMap::stop()
    {
    }
}


