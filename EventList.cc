/*************************************************************************
	> File Name: EventList.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月07日 星期五 13时10分57秒
 ************************************************************************/

#include"EventList.h"

namespace unet
{
    namespace net
    {
        EventList::EventList()
        {};

        EventList::EventList(EventList&& lhs) :
            eventList(std::move(lhs.eventList)),
            eventFdList(std::move(lhs.eventFdList))
        {};

        EventList& EventList::operator=(EventList&& lhs)
        {
            eventList = std::move(lhs.eventList);
            eventFdList = std::move(lhs.eventFdList);
            return *this;
        }

        EventList::~EventList()
        {};

        void EventList::swap(EventList& lhs)
        {
            std::swap(eventList,lhs.eventList);
            std::swap(eventFdList,lhs.eventFdList);
        };

        void EventList::insert(int fd,int event_)
        {
            struct epoll_event event;
            bzero(&event,sizeof(event));
            event.events = event_;
            event.data.fd = fd;
            
            {
                thread::MutexLockGuard guard(mutex);
                eventList.push_back(event);
                eventFdList.push_back(event.data.fd);
            }
        }


        void EventList::erase(int fd)
        {
            thread::MutexLockGuard guard(mutex);
            auto pos = std::find(eventFdList.begin(),eventFdList.end(),fd);
            int index = pos - eventFdList.cbegin();
            auto pos1 = eventList.begin() + index;
            eventList.erase(pos1);
        }

        std::vector<struct epoll_event>& EventList::getEventList()
        {return eventList;}
    }
}

