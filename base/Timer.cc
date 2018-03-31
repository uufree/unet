/*************************************************************************
	> File Name: Timer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月27日 星期四 13时56分23秒
 ************************************************************************/

#include"Timer.h"

namespace unet
{
    namespace base
    {
        Timer::Timer(Time time,bool repeat,double repeatTime) :
            _time(time),
            _repeat(repeat),
            _repeatTime(repeatTime)
        {};

        Timer::Timer(bool repeat,double repeatTime) :
            _time(Time()),
            _repeat(repeat),
            _repeatTime(repeatTime)
        {};

        Timer::Timer(Timer&& lhs) :
            _time(std::move(lhs._time)),
            _repeat(lhs._repeat),
            _repeatTime(lhs._repeatTime),
            _timeCallBack(std::move(lhs._timeCallBack))
        {};
          
        Timer& Timer::operator=(Timer&& lhs)
        {
            if(*this == lhs)
                return *this;
            _time = std::move(lhs._time);
            _repeat = lhs._repeat;
            _repeatTime = lhs._repeatTime;
            _timeCallBack = std::move(lhs._timeCallBack);
            
            return *this;
        }
    }
}

