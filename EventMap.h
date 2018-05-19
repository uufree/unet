/*************************************************************************
	> File Name: EventMap.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月07日 星期五 13时04分02秒
 ************************************************************************/

#ifndef _EVENTMAP_H
#define _EVENTMAP_H

#include<memory>
#include<map>

#include"base/Mutex.h"

namespace unet
{
    class Event;
    class Mutex;
    
    class EventMap final
    {
        public:
            explicit EventMap();
            EventMap(const EventMap& lhs) = delete;
            EventMap(EventMap&& lhs);
            EventMap& operator=(const EventMap& lhs) = delete;
            EventMap& operator=(EventMap&& lhs);
            ~EventMap(){};
            
            bool operator==(const EventMap& event){return u_eventMap.size() == event.u_eventMap.size();};

            int size() const{return u_eventMap.size();};
            bool empty() const {return u_eventMap.empty();};
            void insert(int fd,int type,int wevent);
            void erase(int fd);
            std::shared_ptr<Event> find(int fd) const;

        private:
            base::MutexLock u_mutex;
            std::map<int,std::shared_ptr<Event>> u_eventMap;
    };
}

#endif
