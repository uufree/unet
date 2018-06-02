/*************************************************************************
	> File Name: Epoller.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时01分25秒
 ************************************************************************/

#include"EPoller.h"
#include"base/Global.h"
#include"Type.h"
#include"EventMap.h"
#include"Event.h"

namespace unet
{
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
        u_epollfd = epoller.u_epollfd;
        epoller.u_epollfd = -1;
        u_eventMap.swap(epoller.u_eventMap);
        u_activeList.clear();
        u_openET = false; 
    };
    
    EPoller& EPoller::operator=(EPoller&& epoller)
    {
        if(*this == epoller)
            return *this;
        u_wfds = epoller.u_wfds;
        u_rfds = 0;
        u_openET = epoller.u_openET;
        
        u_epollfd = epoller.u_epollfd;
        epoller.u_epollfd = -1;
        u_eventMap.swap(epoller.u_eventMap);
        u_activeList.clear();

        return *this;
    }

    EPoller::~EPoller()
    {
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
        uint32_t sysEvent = 0;

        if(event & U_EXCEPTION)
            sysEvent |= EPOLLPRI|EPOLLERR|EPOLLRDHUP;   //检测带外数据
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
            u_eventMap.insert({fd,event});
        }

        if(::epoll_ctl(u_epollfd,EPOLL_CTL_ADD,fd,event) < 0)
        {
            perror("epoll add error!\n");
            unet::handleError(errno);
        }
    }

    void EPoller::delEvent(int fd)
    {
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

            if((u_activeList[i].events & EPOLLERR) || (u_activeList[i].events & EPOLLHUP) || (u_activeList[i].events & EPOLLRDHUP))
                revent |= U_EXCEPTION;
            if(u_activeList[i].events & EPOLLIN)
                revent |= U_READ;
            if(u_activeList[i].events & EPOLLOUT)
                revent |= U_WRITE;
            
            if(revent)
            {
                std::shared_ptr<Event> ptr = eventMap.find(u_activeList[i].data.fd);
                if(ptr)
                {
                    ptr->setREvent(revent);
                    eventList.push_back(ptr);
                }
            }
        }
    }
}

