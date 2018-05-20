/*************************************************************************
	> File Name: EventDemultiplexer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 12时29分04秒
 ************************************************************************/

#ifndef _EVENTDEMULTIPLEXER_H
#define _EVENTDEMULTIPLEXER_H

#include<memory>
#include<vector>

namespace unet
{
    class Event;
    class EventMap;
    
    class EventDemultiplexer 
    {
        public:
            explicit EventDemultiplexer();
            EventDemultiplexer(const EventDemultiplexer&) = delete;
            EventDemultiplexer& operator=(const EventDemultiplexer&) = delete;
            EventDemultiplexer(EventDemultiplexer&&);
            EventDemultiplexer& operator=(EventDemultiplexer&&);
            virtual ~EventDemultiplexer(){u_start = false;};
            
            bool operator==(const EventDemultiplexer& event)const {return u_wfds==event.u_wfds && u_rfds==event.u_rfds;};

            bool start() const{return u_start;};
            void start(){u_start = true;};
            void stop(){u_start = false;};
            int watchFds() const{return u_wfds;};
            int activeFds() const{return u_rfds;};

            virtual void addEvent(int,int){};
            virtual void delEvent(int){};
            virtual void poll(const EventMap&,std::vector<std::shared_ptr<Event>>&){};
    
        protected:
            bool u_start;
            int u_wfds;     //watch fds
            int u_rfds;     //active fds
    };
}

#endif
