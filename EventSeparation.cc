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
        switch(event)
        {
            case U_EXCEPTION: 
                sysEvent |= EPOLLPRI;   //带外数据 
            case U_WRITE:
                sysEvent |= EPOLLOUT;
            case U_READ:
                sysEvent |= EPOLLIN;
            default:
                sysEvent |= EPOLLONESHOT;
        }

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
        }

        if(::epoll_ctl(u_epollfd,EPOLL_CTL_ADD,fd,event) < 0)
        {
            perror("epoll add error!\n");
            unet::handleError(errno);
        }
        ++u_wfds;
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
        --u_wfds;
    }
    
    void EPoller::poll(const EventMap& eventMap,std::vector<std::shared_ptr<Event>>& eventList)
    {
        u_activeList.clear();
        u_rfds = ::epoll_wait(u_epollfd,&*u_activeList.begin(),65536,-1);
        if(u_rfds < 0)
        {
            perror("epoll_wait error!\n");
            unet::handleError(errno);
            return;
        }
        
        int revent = 0;
        for(int i=0;i<u_rfds;++i)
        {
            revent = 0;
            switch(u_activeList[i].events)
            {
                case EPOLLERR:
                case EPOLLHUP:
                case EPOLLRDHUP:
                {
                    revent |= U_EXCEPTION;
                    break;
                } 
                case EPOLLIN:
                    revent |= U_READ;
                case EPOLLOUT:
                    revent |= U_WRITE;
            }
            
            std::shared_ptr<Event> ptr = eventMap.find(u_activeList[i].data.fd);
        }

    }

}
