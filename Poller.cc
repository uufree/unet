/*************************************************************************
	> File Name: Poller.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 12时41分59秒
 ************************************************************************/

#include<poll.h>
#include<algorithm>

#include"Type.h"
#include"base/Global.h"
#include"Event.h"
#include"EventMap.h"
#include"Poller.h"

namespace unet
{
    Poller::Poller() : 
        EventDemultiplexer(),
        u_eventList(),
        u_set(),
        u_mutex(),
        u_stopMap()
    {};

    Poller::Poller(Poller&& poll) :
        EventDemultiplexer(),
        u_eventList(poll.u_eventList),
        u_set(poll.u_set),
        u_mutex(),
        u_stopMap()
    {
        base::MutexLockGuard guard(poll.u_mutex);
        std::swap(u_stopMap,poll.u_stopMap);
    };

    Poller& Poller::operator=(Poller&& poll)
    {
        if(*this == poll)
            return *this;
        std::swap(poll.u_eventList,u_eventList);
        std::swap(u_set,poll.u_set);
        
        {
            base::MutexLockGuard guard(poll.u_mutex);
            {
                base::MutexLockGuard guard(u_mutex);
                std::swap(u_stopMap,poll.u_stopMap);
            }
        }

        return *this;
    }

    Poller::~Poller()
    {
        u_eventList.clear();
        u_stopMap.clear();
        u_set.clear();
    };
    
    int Poller::switchEvent(int usrEvent)
    {
        int sysEvent = 0;
        if(usrEvent & U_READ)
            sysEvent |= POLLIN;
        if(usrEvent & U_WRITE)
            sysEvent |= POLLOUT;
        if(usrEvent & U_EXCEPTION)
            sysEvent |= POLLERR|POLLHUP|POLLNVAL|POLLRDHUP;
        return sysEvent;
    }

    void Poller::addEvent(int fd,int wevent)
    {
        if(u_set.find(fd) == u_set.end())
        {
            ++u_wfds;
            u_set.insert(fd);
            struct pollfd pfd;
            pfd.fd = fd;
            pfd.events = switchEvent(wevent);
            pfd.revents = 0;
            u_eventList.push_back(pfd);
        }
        else
        {
            auto iter = u_eventList.begin();
            for(;iter!=u_eventList.end();++iter)
                if(iter->fd == fd)
                    break;

            if(iter != u_eventList.end())
            {
                (*iter).events = switchEvent(wevent);
                (*iter).revents = 0;
            }
        }
    }

    void Poller::delEvent(int fd)
    {
        if(u_set.find(fd) == u_set.end())
            return;
        u_set.erase(fd);
        --u_wfds;
        
        {
            base::MutexLockGuard guard(u_mutex);
            auto miter = u_stopMap.find(fd);
            if(miter != u_stopMap.end())
                u_stopMap.erase(miter);
            
            miter = u_stopMap.find(-fd);
            if(miter != u_stopMap.end())
                u_stopMap.erase(miter);
        }

        auto iter = u_eventList.begin();
        for(;iter!=u_eventList.end();++iter)
        {
            if(iter->fd == fd)
            {
                u_eventList.erase(iter);
                break;
            }
        }
    }
    
    void Poller::poll(const EventMap& eventMap,std::vector<std::shared_ptr<Event>>& eventList)
    {
        {
            base::MutexLockGuard guard(u_mutex);
            for(auto iter=u_stopMap.begin();iter!=u_stopMap.end();++iter)
            {
                if(iter->first > 0)
                {
                    u_eventList.push_back(iter->second);
                    u_stopMap.erase(iter);
                }
            }
        }

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
                
                /*使用了一个小技巧，将索引改变为-1的。在事件处理线程重置事件
                时，将符号变正即可*/
                {
                    base::MutexLockGuard guard(u_mutex);
                    u_stopMap.insert({-iter->fd,*iter});
                }
                u_eventList.erase(iter);
            }
        }
    }

    void Poller::resetEvent(int fd)
    {
        base::MutexLockGuard guard(u_mutex);
        auto iter = u_stopMap.find(-fd);
        if(u_set.find(fd) == u_set.end() ||  iter == u_stopMap.end())
            return;
        u_stopMap.insert({fd,iter->second}); 
        u_stopMap.erase(iter);
    }
}

