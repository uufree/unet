/*************************************************************************
	> File Name: Timer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 21时04分39秒
 ************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

namespace unet
{
    namespace time
    {
        class Timer final
        {
            public:
                explicit Timer(const TimeCallBack& callback_,bool repeat_,double repeattime) : callback(callback_),
                repeat(repeat_),repeattime(repeattime_),
                time(Timestamp())
                {
                    addTime(time,repeattime_);
                };

                Timer(const Timer& lhs) = delete;
                Timer& operator=(const Timer& lhs) = delete;
                
                ~Timer(){};

                void run()
                {
                    callback();
                }

                bool isRepeat()
                {
                    return repeat;
                }

                void restart()
                {
                    if(repeat)
                        addTime(time,repeattime);
                }

            private:
                
                typedef std::functor<void()> TimeCallBack;

                const TimeCallBack callback;
                bool repeat;
                double repeattime;
                Timestamp time;
        };
    }
}

#endif

