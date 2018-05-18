/*************************************************************************
	> File Name: TimerEvent.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月18日 星期五 18时39分33秒
 ************************************************************************/

#ifndef _TIMEREVENT_H
#define _TIMEREVENT_H

#include<queue>
#include<utility>
#include<list>

#include"type.h"
#include"base/Timer.h"

namespace unet
{

    class TimerEvent final
    {
        typedef std::pair<uint64_t,base::Timer> TimerPair;
        
        struct op
        {
            bool operator()(const TimerPair& lhs,const TimerPair& rhs)
            {return lhs.first > rhs.first;};
        };
        
        private:
            int u_timerfd;
            bool u_start;
            std::priority_queue<TimerPair,std::list<TimerPair>,op> u_timerHeap;
    };
}

#endif
