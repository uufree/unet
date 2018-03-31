/*************************************************************************
	> File Name: TimerMap.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年04月01日 星期日 01时03分07秒
 ************************************************************************/

#ifndef _TIMERMAP_H
#define _TIMERMAP_H

#include"base/Timer.h"
#include"Channel.h"
#include"base/Mutex.h"

#include<map>
#include<sys/timerfd.h>
#include<functional>
#include<memory>
#include<vector>
#include<string.h>

namespace unet
{
    typedef std::shared_ptr<base::Timer> TimerPtr;
        
    class TimerMap final
    {
        typedef std::map<base::Time,TimerPtr> TimerMaps;
        typedef std::unique_ptr<unet::net::Channel> ChannelPtr;
        typedef std::function<void(ChannelPtr&&)> InsertChannelCallBack;
        typedef std::function<void(int)> EraseChannelCallBack;
        typedef std::vector<base::Time> TimeList;

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

            void setInsertChannelCallBack(const InsertChannelCallBack& cb){_insertChannelCallBack = cb;};
            void setEraseChannelCallBack(const EraseChannelCallBack& cb){_eraseChannelCallBack = cb;};
                
        private:
            void handleRead();
            
        private:
            const int _timefd;
            TimerMaps _timerMap;
            bool _start;
            base::MutexLock _mutex;
            InsertChannelCallBack _insertChannelCallBack;
            EraseChannelCallBack _eraseChannelCallBack;
    };
}

#endif



