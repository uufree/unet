/*************************************************************************
	> File Name: Poller.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 12时32分18秒
 ************************************************************************/

#ifndef _POLLER_H
#define _POLLER_H

#include"EventDemultiplexer.h"
#include"base/Mutex.h"
#include<set>
#include<map>

#include<iostream>

namespace unet
{
    class Poller final : public EventDemultiplexer
    {
        public:
            explicit Poller();
            Poller(const Poller&) = delete;
            Poller& operator=(const Poller&) = delete;
            Poller(Poller&&);
            Poller& operator=(Poller&&);
            ~Poller() override;
            
            bool operator==(const Poller& poll){return u_eventList.begin() == poll.u_eventList.begin();};

            void addEvent(int,int) override;
            void delEvent(int) override;
            void poll(const EventMap&,std::vector<std::shared_ptr<Event>>&) override;
            void resetEvent(int) override; 
        private:
            int switchEvent(int usrEvent);
    
        private:
            std::vector<struct pollfd> u_eventList;
            std::set<int> u_set;
            
            base::MutexLock u_mutex;
            std::map<int,struct pollfd> u_stopMap;
    };
}

#endif
