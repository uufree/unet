/*************************************************************************
	> File Name: Timer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 21时04分39秒
 ************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

#include<time.h>
#include<sys/time.h>
#include<unistd.h>
#include<stdint.h>
#include<functional>

namespace unet
{
    namespace base
    {
        class Time final
        {
            public:
                static const int KMicroseconds = 1000000;
                
                explicit Time()
                {
                    struct timeval tv;
                    ::gettimeofday(&tv,NULL);
                    _microseconds = tv.tv_sec * Time::KMicroseconds + tv.tv_usec;
                }
                
                bool operator<(const Time& time){return _microseconds < time._microseconds;};
                bool operator==(const Time& time){return _microseconds == time._microseconds;};
                void addTime(int seconds){_microseconds += seconds * KMicroseconds;};
                uint64_t getTime() const {return _microseconds;};

            private:
                uint64_t _microseconds;
        };


        class Timer final
        {
            typedef std::function<void()> TimeCallBack;

            public:
                explicit Timer(Time time,bool repeat,double repeatTime);
                explicit Timer(bool repeat,double repeatTime);
                Timer(const Timer& lhs) = delete;
                Timer(Timer&& lhs);
                Timer& operator=(const Timer& lhs) = delete;
                Timer& operator=(Timer&& lhs);
                ~Timer(){};
                
                bool operator==(const Timer& timer) {return _time<timer._time;};
                bool operator<(const Timer& timer) {return _time<timer._time;};
                void run() const {_timeCallBack();};
                bool isRepeat() const {return _repeat;};
                void setTimeCallBack(const TimeCallBack& callback){_timeCallBack = callback;};
                double getRepeatTime()const{return _repeatTime;};

            private:
                Time _time;
                bool _repeat;
                double _repeatTime;
                TimeCallBack _timeCallBack;
        };
    }
}

#endif

