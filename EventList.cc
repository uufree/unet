/*************************************************************************
	> File Name: EventList.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月07日 星期五 13时10分57秒
 ************************************************************************/

#include"EventList.h"
#include<iostream>

namespace unet
{
    namespace net
    {
        EventList::EventList()
        {
            std::cout << "4" << std::endl;
        };

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
        {
            std::cout << "~EventList" << std::endl;
        };

        void EventList::swap(EventList& lhs)
        {
            std::swap(eventList,lhs.eventList);
            std::swap(eventFdList,lhs.eventFdList);
        };

        void EventList::insert(int fd,int event_,int epollfd)
        {
            struct epoll_event event;
            bzero(&event,sizeof(event));
            event.events = event_;
            event.data.fd = fd;
            
            if(::epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event) < 0)
                unet::handleError(errno);

            {
                thread::MutexLockGuard guard(mutex);
                eventList.push_back(event);
                eventFdList.push_back(event.data.fd);
            }
        }


        void EventList::erase(int fd,int epollfd)
        {
            thread::MutexLockGuard guard(mutex);

            if(::epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,NULL) < 0)
                unet::handleError(errno);

            auto pos = std::find(eventFdList.begin(),eventFdList.end(),fd);
            int index = pos - eventFdList.cbegin();
            auto pos1 = eventList.begin() + index;
            eventList.erase(pos1);
        }

        std::vector<struct epoll_event>& EventList::getEventList()
        {return eventList;}
    }
}

