/*************************************************************************
	> File Name: EventMap.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月07日 星期五 13时10分57秒
 ************************************************************************/

#include"EventMap.h"
#include"Event.h"

namespace unet
{
    EventMap::EventMap() :
        u_mutex(),
        u_eventMap()
    {};
    
    EventMap::EventMap(EventMap&& event) :
        u_mutex(),
        u_eventMap(event.u_eventMap)
    {};

    EventMap& EventMap::operator=(EventMap&& event)
    {
        if(event == *this)
            return *this;
        {
            base::MutexLockGuard guard(u_mutex); 
            base::MutexLockGuard guardS(event.u_mutex);
            std::swap(u_eventMap,event.u_eventMap);
        }
        return *this;
    }
    
    void EventMap::insert(std::shared_ptr<Event>& ptr)
    {
        auto iter = u_eventMap.find(ptr->getFd());
        if(iter != u_eventMap.end())
            return;
        {
            base::MutexLockGuard guard(u_mutex);
            u_eventMap.insert(std::make_pair(ptr->getFd(),ptr));
        }
    }

    void EventMap::erase(int fd)
    {
        auto iter = u_eventMap.find(fd);
        if(iter == u_eventMap.end())
            return;
        
        {
            base::MutexLockGuard guard(u_mutex);
            u_eventMap.erase(fd);
        }
    }

    std::shared_ptr<Event> EventMap::find(int fd) const
    {
        auto iter = u_eventMap.find(fd);
        if(iter == u_eventMap.end())
            return nullptr;
        return iter->second;
    }

}

