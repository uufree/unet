/*************************************************************************
	> File Name: EventList.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月07日 星期五 13时10分57秒
 ************************************************************************/

#include"EventList.h"
#include"Channel.h"
#include<algorithm>

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

        int EventList::size() const
        {return eventList.size();};

        bool EventList::empty() const
        {return eventList.empty();};

        void EventList::insert(const Channel& channel)
        {
            struct epoll_event event;
            bzero(&event,sizeof(event));
            event.events = channel.getEvent();
            event.data.fd = channel.getFd();

            eventList.push_back(std::move(event));
            eventFdList.push_back(event.data.fd);
        }

        void EventList::erase(int fd)
        {
            auto pos = std::find(eventFdList.begin(),eventFdList.end(),fd);
            int index = pos - eventFdList.cbegin();
            pos = eventList.begin() + index;
            eventList.erase(pos);
        }

        std::vector<struct epoll_event>& EventList::getEventList()
        {return eventList;}
    }
}

