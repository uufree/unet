/*************************************************************************
	> File Name: TimerQueue.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 21时30分32秒
 ************************************************************************/

#ifndef _TIMERQUEUE_H
#define _TIMERQUEUE_H

#include"Timer.h"
#include"Timestamp.h"
#include"Channel.h"

#include<map>
#include<sys/timerfd.h>
#include<functional>
#include<memory>

namespace unet
{
    namespace time
    {
        class TimerQueue final
        {
            typedef std::unique_ptr<Timer> TimerPtr;
            typedef std::map<Timestamp,TimerPtr> TimerMap;
            typedef std::unique_ptr<unet::net::Channel> ChannelPtr;
            typedef std::function<void(ChannelPtr&&)> InsertChannelCallBack;
            typedef std::function<void(int)> EraseChannelCallBack;

            public:
                TimerQueue();
                TimerQueue(const TimerQueue& lhs) = delete;
                TimerQueue& operator=(const TimerQueue& lhs) = delete;
                TimerQueue(TimerQueue&& lhs);
                TimerQueue& operator=(TimerQueue&& lhs);
                ~TimerQueue();

                void addTimer(Timer&& timer_);
                void removeTimer(const Timer& lhs);

                void setInsertChannelCallBack(const InsertChannelCallBack& cb)
                {insertChannelCallBack = cb;};

                void setEraseChannelCallBack(const EraseChannelCallBack& cb)
                {eraseChannelCallBack = cb;};
            
            private:
                void handleRead();
            
            private:
                const int timefd;
                ChannelPtr timefdChannel;
                TimerMap timerMap;
                bool started;
                InsertChannelCallBack insertChannelCallBack;
                EraseChannelCallBack eraseChannelCallBack;
        };

    }
}
















#endif

