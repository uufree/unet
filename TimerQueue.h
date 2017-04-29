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
#include<set>
#include<memory>
#include"Channel.h"
#include"Mutex.h"

namespace unet
{
    namespace time
    {
        class TimerQueue final
        {
            typedef std::pair<Timestamp,Timer*> Entry;
            typedef std::set<Entry> TimerList;
            typedef std::unique_ptr<unet::net::Channel> ClockChannel;
            typedef std::function<void(Channel*)> AddInServerLoop;

            public:
                TimerQueue();
                TimerQueue(const TimerQueue& lhs) = delete;
                TimerQueue& operator=(const TimerQueue& lhs) = delete;
                ~TimerQueue();

                void addTimer(Timer* timer_);

                void setAddInServerLoopCallBack(const AddInServerLoop& cb)
                {addinserverloop = cb;};
/*                
                void addInServerLoop()
                {channelcallback(timefdchannel.get());};
*/
            private:
                void handleRead();

                const int timefd;
                ClockChannel timefdchannel;
                TimerList activetimers;
                bool handlecalbacking;
                Timer* nowtimer;
                unet::thread::MutexLock lock;
                AddInServerLoop addinserverloop;
        };

    }
}
















#endif

