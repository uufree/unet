/*************************************************************************
	> File Name: TimerQueue.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 21时30分32秒
 ************************************************************************/

#ifndef _TIMERQUEUE_H
#define _TIMERQUEUE_H

namespace unet
{
    namespace time
    {
        class TimerQueue final
        {
            public:
                TimerQueue(EventLoop* loop_);
                TimerQueue(const TimerQueue& lhs) = delete;
                TimerQueue& operator(const TimerQueue& lhs) = delete;
                ~TimerQueue();

                void addTimer(Timer* timer_);
            
            private:
                typedef std::pair<Timestamp,Timer*> Entry;
                typedef std::set<Entry> TimerList;
                
                void addTimerInLoop(Timer* timer_);
                void 

                EventLoop* loop;
                Channel timefdchannel;
                TimerList activetimers;
                bool handlecalbacking;
                MutexLock mutexlock;//其他线程会向这个定时器线程写事件
        };

    }
}
















#endif

