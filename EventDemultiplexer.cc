/*************************************************************************
	> File Name: EventDemultiplexer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 12时33分41秒
 ************************************************************************/

#include"EventDemultiplexer.h"

namespace unet
{
    EventDemultiplexer::EventDemultiplexer() :
        u_wfds(0),
        u_rfds(0)
    {};

    EventDemultiplexer::EventDemultiplexer(EventDemultiplexer&& event) :
        u_wfds(event.u_rfds),
        u_rfds(0)
    {};
    
    EventDemultiplexer& EventDemultiplexer::operator=(EventDemultiplexer&& event)
    {
        if(event == *this)
            return *this;
        u_wfds = event.u_wfds;
        u_rfds = event.u_rfds;

        return *this;
    }
}

