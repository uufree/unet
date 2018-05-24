/*************************************************************************
	> File Name: Timer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 21时04分39秒
 ************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

#include<stdint.h>
#include<functional>
#include<memory>


namespace unet
{
    namespace base
    {
        class Time final
        {
            public:
                static const int KMicroseconds = 1000000;
                
                explicit Time();
                bool operator<(const Time& time){return u_microseconds<time.u_microseconds;};
                bool operator==(const Time& time){return u_microseconds==time.u_microseconds;};
                void addTime(int seconds){u_microseconds += seconds * KMicroseconds;};
                uint64_t getTime() const {return u_microseconds;};

            private:
                uint64_t u_microseconds;
        };
    }
        
    class TimerEvent;

    class Timer final : public std::enable_shared_from_this<Timer> 
    {
        typedef std::function<void()> TimeCallBack;
        typedef std::weak_ptr<TimerEvent> TimerEventWPtr;
        typedef std::shared_ptr<TimerEvent> TimerEventPtr;

        public:
            explicit Timer(base::Time time,bool repeat,double repeatTime);
            explicit Timer(bool repeat,double repeatTime);
            explicit Timer(bool repeat,double repeatTime,const TimeCallBack& callback);
            Timer(const Timer& lhs) = delete;
            Timer(Timer&& lhs);
            Timer& operator=(const Timer& lhs) = delete;
            Timer& operator=(Timer&& lhs);
            ~Timer();
                
            bool operator==(const Timer& timer){return u_time<timer.u_time;};
            bool operator<(const Timer& timer) {return u_time<timer.u_time;};
            void run() const {u_timeCallBack();};
            bool repeat() const {return u_repeat;};
            void setTimeCallBack(const TimeCallBack& callback){u_timeCallBack = callback;};
            void setTimesPtr(const TimerEventPtr& ptr){u_timers=ptr;};
            bool hasCallBack()const{return u_timeCallBack?true:false;};
            bool hasTimers() const{return u_timers.lock()?true:false;}
            double getRepeatTime()const{return u_repeatTime;};
            void start();
            void stop();
            bool isStart() const{return u_start;};
            bool isStop() const{return !u_start;};

        private:
            bool u_start;
            base::Time u_time;
            bool u_repeat;
            double u_repeatTime;
            TimeCallBack u_timeCallBack;
            TimerEventWPtr u_timers;
    };
}

#endif

