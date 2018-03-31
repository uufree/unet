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
        _start(false),
        _mutex()
    {};

    TimerMap::TimerMap(TimerMap&& lhs) : 
        _timefd(createTimefd()),
        _start(false),
        _mutex(),
        _insertChannelCallBack(std::move(lhs._insertChannelCallBack)),
        _eraseChannelCallBack(std::move(lhs._eraseChannelCallBack))
    {};

    TimerMap& TimerMap::operator=(TimerMap&& lhs)
    {
        if(*this == lhs)
            return *this;
        
        _timerMap.clear();
        _timerMap = std::move(lhs._timerMap);
        _start = false;

        return *this;
    }

        
    TimerMap::~TimerMap()
    {
        ::close(_timefd);
        _timerMap.clear();
    }

    void TimerMap::addTimer(TimerPtr&& timer)
    {
        base::Time now;
        now.addTime(timer->getRepeatTime());
            
        if(_timerMap.size() > 0)
            {//setTimer
                if(now < timerMap.begin()->first)
                {
                    struct itimerspec newSpec;
                    bzero(&newSpec,sizeof(newSpec));
                    struct itimerspec oldSpec;
                    bzero(&oldSpec,sizeof(oldSpec));
                    newSpec.it_value = howMuchTimeFromNow(now);
                
                    if(::timerfd_settime(timefd,0,&newSpec,&oldSpec) < 0)
                        unet::handleError(errno);
                }
            }
            else if(timerMap.size() == 0)
            {
                struct itimerspec newSpec;
                bzero(&newSpec,sizeof(newSpec));
                struct itimerspec oldSpec;
                bzero(&oldSpec,sizeof(oldSpec));
                newSpec.it_value = howMuchTimeFromNow(now);
                
                if(::timerfd_settime(timefd,0,&newSpec,&oldSpec) < 0)
                    unet::handleError(errno);
            }
            else
            {};

            {
                unet::thread::MutexLockGuard guard(mutex);
                timerMap.insert({now,timer_});
            }
        }

        void TimerQueue::addTimer(Timestamp&& time_,TimerPtr&& ptr)
        {
            if(timerMap.size() > 0)
            {//setTimer
                if(time_ < timerMap.begin()->first)
                {
                    struct itimerspec newSpec;
                    bzero(&newSpec,sizeof(newSpec));
                    struct itimerspec oldSpec;
                    bzero(&oldSpec,sizeof(oldSpec));

                    newSpec.it_value = howMuchTimeFromNow(time_);
                    if(::timerfd_settime(timefd,0,&newSpec,&oldSpec) < 0)
                        unet::handleError(errno);
                }
            }
            else if(timerMap.size() == 0)
            {
                struct itimerspec newSpec;
                bzero(&newSpec,sizeof(newSpec));
                struct itimerspec oldSpec;
                bzero(&oldSpec,sizeof(oldSpec));

                newSpec.it_value = howMuchTimeFromNow(time_);
                if(::timerfd_settime(timefd,0,&newSpec,&oldSpec) < 0)
                    unet::handleError(errno);
            }
            else
            {};


            {
                unet::thread::MutexLockGuard guard(mutex);
                timerMap.insert({time_,ptr});
            }
        }

        void TimerQueue::handleRead()
        {
            ::read(timefd,NULL,0);
            
            TimestampList list;
            Timestamp now;
            
            for(auto iter=timerMap.begin();iter!=timerMap.end();++iter)
            {
                if(iter->first < now)
                {
                    iter->second->run();
                    
                    if(iter->second->isRepeat())
                    {
                        list.push_back(iter->first);
                        Timestamp time = iter->first;
                        time.addTime(iter->second->getRepeatTime());
                        addTimer(std::move(time),std::move(iter->second));
                    }                        
                }
                else
                    break;
            }

            {
                unet::thread::MutexLockGuard guard(mutex);
                for(auto iter=list.cbegin();iter!=list.cend();++iter)
                    timerMap.erase(*iter);
            }
        }

        void TimerQueue::start()
        {
            ChannelPtr channel(new net::Channel(timefd,net::CLOCK));
            channel->setReadCallBack(std::bind(&TimerQueue::handleRead,this));
            
            insertChannelCallBack(std::move(channel));
        }

        void TimerQueue::stop()
        {
            eraseChannelCallBack(timefd);
        }

    }
}


