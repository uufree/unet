/*************************************************************************
	> File Name: EventSeparation.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月19日 星期六 19时32分03秒
 ************************************************************************/

#include<cstring>

#include"EventSeparation.h"
#include"base/global.h"
#include"type.h"
#include"Event.h"
#include"EventMap.h"

namespace unet
{
    EventDemultiplexer::EventDemultiplexer() :
        u_start(false),
        u_wfds(0),
        u_rfds(0)
    {};

    EventDemultiplexer::EventDemultiplexer(EventDemultiplexer&& event) :
        u_start(false),
        u_wfds(event.u_rfds),
        u_rfds(0)
    {};
    
    EventDemultiplexer& EventDemultiplexer::operator=(EventDemultiplexer&& event)
    {
        if(event == *this)
            return *this;
        u_start = false;
        u_wfds = event.u_wfds;
        u_rfds = event.u_rfds;

        return *this;
    }
    
    //epoller
    EPoller::EPoller() : 
        EventDemultiplexer(),
        u_epollfd(::epoll_create(65536)),
        u_eventMap(),
        u_activeList(),
        u_openET(false)
    {
        if(u_epollfd < 0)
        {
            perror("epollfd create error!\n");
            unet::handleError(errno);
        }
        u_activeList.reserve(65536);
    }

    EPoller::EPoller(EPoller&& epoller) : EventDemultiplexer()
    {
        if((u_epollfd = ::dup(epoller.u_epollfd)) < 0)
        {
            perror("epollfd dup error!\n");
            unet::handleError(errno);
        }
        u_eventMap.swap(epoller.u_eventMap);
        u_activeList.clear();
        u_openET = false; 
    };
    
    EPoller& EPoller::operator=(EPoller&& epoller)
    {
        if(*this == epoller)
            return *this;
        u_start = false;
        u_wfds = epoller.u_wfds;
        u_rfds = 0;
        u_openET = epoller.u_openET;
        
        if((u_epollfd = ::dup(epoller.u_epollfd)) < 0)
        {
            perror("epollfd dup error!\n");
            unet::handleError(errno);
        }
        u_eventMap.swap(epoller.u_eventMap);
        u_activeList.clear();

        return *this;
    }

    EPoller::~EPoller()
    {
        EventDemultiplexer::~EventDemultiplexer();
        if(::close(u_epollfd) < 0)
        {
            perror("close epollfd error!\n");
            unet::handleError(errno);
        }
        for(auto iter=u_eventMap.begin();iter!=u_eventMap.end();++iter)
            delete iter->second;
    }
    
    uint32_t EPoller::switchTo(int event)
    {
        uint32_t sysEvent;

        if(event & U_EXCEPTION)
            sysEvent |= EPOLLPRI;   //检测带外数据
        if(event & EPOLLOUT)
            sysEvent |= EPOLLOUT;
        if(event & EPOLLIN)
            sysEvent |= EPOLLIN;
        sysEvent |= EPOLLONESHOT;   //默认开启ONTSHOT

        if(u_openET)
            sysEvent |= EPOLLET;
        return sysEvent;
    }

    void EPoller::addEvent(int fd,int wevent)
    {
        if(!u_start)
            return;
        
        struct epoll_event* event = NULL;
        auto iter = u_eventMap.find(fd);
        if((iter != u_eventMap.end()))
            event = iter->second;
        
        if(!event)
        {
            event = new epoll_event;
            memset(event,'\0',sizeof(struct epoll_event));
            event->events = switchTo(wevent); 
            event->data.fd = fd;
            ++u_wfds;
        }

        if(::epoll_ctl(u_epollfd,EPOLL_CTL_ADD,fd,event) < 0)
        {
            perror("epoll add error!\n");
            unet::handleError(errno);
        }
    }

    void EPoller::delEvent(int fd)
    {
        if(!u_start)
            return;
        
        auto iter = u_eventMap.find(fd);
        if(iter == u_eventMap.end())
            return;
        if(::epoll_ctl(u_epollfd,EPOLL_CTL_DEL,fd,NULL) < 0)
        {
            perror("epoll del error!\n");
            unet::handleError(errno);
        }

        delete iter->second;
        u_eventMap.erase(iter);
        --u_wfds;
    }
    
    void EPoller::poll(const EventMap& eventMap,std::vector<std::shared_ptr<Event>>& eventList)
    {
        if(!u_start)
            return;

        u_activeList.clear();
        u_rfds = ::epoll_wait(u_epollfd,&*u_activeList.begin(),65536,-1);
        if(u_rfds < 0)
        {
            perror("epoll_wait error!\n");
            unet::handleError(errno);
            return;
        }
        
        int revent;
        for(int i=0;i<u_rfds;++i)
        {
            revent = 0;

            if((u_activeList[i].events & EPOLLERR) || (u_activeList[i].events & EPOLLHUP) || (u_activeList[i].events & EPOLLRDHUP))
                revent |= U_EXCEPTION;
            if(u_activeList[i].events & EPOLLIN)
                revent |= U_READ;
            if(u_activeList[i].events & EPOLLOUT)
                revent |= U_WRITE;
            
            if(revent)
            {
                std::shared_ptr<Event> ptr = eventMap.find(u_activeList[i].data.fd);
                ptr->setREvent(revent);
                eventList.push_back(ptr);
            }
        }
    }

    //selector
    Selecter::Selecter() :
        EventDemultiplexer(),
        maxfd(0),
        u_set()
    {
        FD_ZERO(&u_readSet);
        FD_ZERO(&u_readSetSave);
        FD_ZERO(&u_writeSet);
        FD_ZERO(&u_writeSetSave);
        FD_ZERO(&u_exceptionSet);
        FD_ZERO(&u_exceptionSetSave);
    };

    Selecter::Selecter(Selecter&& select) :
        EventDemultiplexer(),
        u_readSet(select.u_readSet),
        u_writeSet(select.u_writeSet),
        u_exceptionSet(select.u_exceptionSet),
        u_readSetSave(select.u_readSetSave),
        u_writeSetSave(select.u_writeSetSave),
        u_exceptionSetSave(select.u_exceptionSetSave),
        maxfd(select.maxfd),
        u_set(select.u_set)
    {};

    Selecter& Selecter::operator=(Selecter&& select)
    {
        if(*this == select)
            return *this;
        u_readSet = select.u_readSet;
        u_readSetSave = select.u_readSetSave;
        u_writeSet = select.u_writeSet;
        u_writeSetSave = select.u_writeSetSave;
        u_exceptionSet = select.u_exceptionSet;
        u_exceptionSetSave = select.u_exceptionSetSave;
        maxfd = select.maxfd;
        u_set = select.u_set;

        return *this;
    }
    
    Selecter::~Selecter()
    {
        EventDemultiplexer::~EventDemultiplexer();
    }
    
    void Selecter::addEvent(int fd,int wevent)
    {
        if(!u_start)
            return;
        
        auto iter = u_set.find(fd);
        if(iter == u_set.end())
        {
            ++u_wfds;
            u_set.insert(fd);
        }

        if(wevent & U_EXCEPTION)
        {
            if(FD_ISSET(fd,&u_exceptionSet))
                FD_SET(fd,&u_exceptionSet);
            if(FD_ISSET(fd,&u_exceptionSetSave))
                FD_SET(fd,&u_exceptionSetSave);
        }

        if(wevent & U_WRITE)
        {
            if(FD_ISSET(fd,&u_writeSet))
                FD_SET(fd,&u_writeSet);
            if(FD_ISSET(fd,&u_writeSetSave))
                FD_SET(fd,&u_writeSetSave);
        }
        
        if(wevent & U_READ)
        {
            if(FD_ISSET(fd,&u_readSet))
                FD_SET(fd,&u_readSet);
            if(FD_ISSET(fd,&u_readSetSave))
                FD_SET(fd,&u_readSetSave);
        }

        if(fd > maxfd)
            maxfd = fd;
    }

    void Selecter::delEvent(int fd)
    {
        if(!u_start)
            return;
        auto iter = u_set.find(fd);
        if(iter == u_set.end())
            return;
        
        --u_wfds;
        u_set.erase(iter);

        if(FD_ISSET(fd,&u_readSet))
            FD_CLR(fd,&u_readSet);
        if(FD_ISSET(fd,&u_readSetSave))
            FD_CLR(fd,&u_readSetSave);
        if(FD_ISSET(fd,&u_writeSet))
            FD_CLR(fd,&u_writeSet);
        if(FD_ISSET(fd,&u_writeSetSave))
            FD_CLR(fd,&u_writeSetSave);
        if(FD_ISSET(fd,&u_exceptionSet))
            FD_CLR(fd,&u_exceptionSet);
        if(FD_ISSET(fd,&u_exceptionSetSave))
            FD_CLR(fd,&u_exceptionSetSave);
    }

    void Selecter::poll(const EventMap& eventMap,std::vector<std::shared_ptr<Event>>& eventList)
    {
        if(!u_start)
            return;

        u_rfds = ::select(maxfd+1,&u_readSet,&u_writeSet,&u_exceptionSet,NULL);
        if(u_rfds < 0)
        {
            perror("select error!\n");
            unet::handleError(errno);
            return;
        }

        int revent;
        for(auto iter=u_set.begin();iter!=u_set.end();++iter)
        {
            revent = 0;
            if(FD_ISSET(*iter,&u_readSetSave))
                revent |= U_READ;
            if(FD_ISSET(*iter,&u_writeSetSave))
                revent |= U_WRITE;
            if(FD_ISSET(*iter,&u_exceptionSetSave))
                revent |= U_EXCEPTION;
            
            if(revent)
            {
                std::shared_ptr<Event> ptr = eventMap.find(*iter);
                if(ptr)
                {
                    ptr->setREvent(revent);
                    eventList.push_back(ptr);
                }
            }
        }

        u_readSet = u_readSetSave;
        u_writeSet = u_writeSetSave;
        u_exceptionSet = u_exceptionSetSave;
    }

    //poller
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
