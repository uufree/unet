/*************************************************************************
	> File Name: Poller.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 12时41分59秒
 ************************************************************************/

#include<poll.h>
#include<algorithm>

#include"type.h"
#include"base/global.h"
#include"Event.h"
#include"EventMap.h"
#include"Poller.h"

namespace unet
{

    Poller::Poller() : 
        EventDemultiplexer(),
        u_eventList(),
        u_set()
    {};

    Poller::Poller(Poller&& poll) :
        EventDemultiplexer(),
        u_eventList(poll.u_eventList),
        u_set()
    {};

    Poller& Poller::operator=(Poller&& poll)
    {
        if(*this == poll)
            return *this;
        std::swap(poll.u_eventList,u_eventList);
        std::swap(u_set,poll.u_set);

        return *this;
    }

    Poller::~Poller()
    {
        EventDemultiplexer::~EventDemultiplexer();
        u_eventList.clear();
        u_set.clear();
    };

    void Poller::addEvent(int fd,int wevent)
    {
        if(!u_start)
            return;
        
        if(u_set.find(fd) == u_set.end())
        {
            ++u_wfds;
            u_set.insert(fd);
            struct pollfd pfd;
            pfd.fd = fd;
            pfd.events = wevent;
            pfd.events = 0;
            u_eventList.push_back(pfd);
        }
        else
        {
            auto iter = std::find(u_eventList.begin(),u_eventList.end(),[fd](const struct pollfd& pfd){return pfd.fd==fd;});
            if(iter != u_eventList.end())
            {
                (*iter).events = wevent;
                (*iter).events = 0;
            }
        }
    }

    void Poller::delEvent(int fd)
    {
        if(!u_start)
            return;
        if(u_set.find(fd) == u_set.end())
            return;
        u_set.erase(fd);
        --u_wfds;

        auto iter = std::find(u_eventList.begin(),u_eventList.end(),[fd](const struct pollfd& pfd){return fd == pfd.fd;});
        if(iter != u_eventList.end())
            u_eventList.erase(iter);
    }
    
    void Poller::poll(const EventMap& eventMap,std::vector<std::shared_ptr<Event>>& eventList)
    {
        if(!u_start)
            return;
        
        u_rfds = ::poll(&*u_eventList.begin(),u_eventList.size(),-1);
        if(u_rfds < 0)
        {
            perror("poll error!\n");
            unet::handleError(errno);
        }

        int revent = 0;
        for(auto iter=u_eventList.begin();iter!=u_eventList.end();++iter)
        {
            revent = 0;
            if((iter->revents & POLLRDHUP) || (iter->revents & POLLERR) || (iter->revents & POLLHUP) || (iter->revents & POLLNVAL))
                revent |= U_EXCEPTION;
            if(iter->revents & POLLIN)
                revent |= U_READ;
            if(iter->revents & POLLOUT)
                revent |= U_WRITE;

            if(revent)
            {
                std::shared_ptr<Event> ptr = eventMap.find((*iter).fd);
                if(ptr)
                {
                    ptr->setREvent(revent);
                    eventList.push_back(ptr);
                }
            }
        }

    }
}

