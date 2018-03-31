/*************************************************************************
	> File Name: TimerMap.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年04月01日 星期日 01时03分07秒
 ************************************************************************/

#ifndef _TIMERMAP_H
#define _TIMERMAP_H

#include<map>
#include<sys/timerfd.h>
#include<functional>
#include<memory>
#include<vector>
#include<string.h>

#include"base/Timer.h"
#include"Channel.h"
#include"base/Mutex.h"

namespace unet
{
    class TimerMap final
    {
        typedef std::shared_ptr<base::Timer> TimerPtr;
        typedef std::map<base::Time,TimerPtr> TimerMaps;
        typedef std::unique_ptr<unet::net::Channel> ChannelPtr;

        public:
            TimerMap();
            TimerMap(const TimerMap& lhs) = delete;
            TimerMap& operator=(const TimerMap& lhs) = delete;
            TimerMap(TimerMap&& lhs);
            TimerMap& operator=(TimerMap&& lhs);
            ~TimerMap();
            
            bool operator==(const TimerMap& map){return _timefd==map._timefd;};

            void addTimer(TimerPtr&& timer_);
            void addTimer(base::Time&& time_,TimerPtr&& ptr);
            void start();
            void stop();

        private:
            void handleRead();
            
        private:
            const int _timefd;
            std::map<base::Time,TimerPtr> _timerMaps;
            bool _start;
            base::MutexLock _mutex;
    };
}

#endif



