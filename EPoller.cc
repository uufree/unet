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
        u_epollfd(::epoll_create(EPOLL_MAX_WATCH)),
        u_eventMap(),
        u_activeList(),
        u_openET(false),
        u_mutex(),
        u_stopSet(),
        u_admutex(),
        u_addList(),
        u_eraseList()
    {
        std::cout << "epollfd: " << u_epollfd << std::endl;
        if(u_epollfd < 0)
        {
            perror("epollfd create error!\n");
            unet::handleError(errno);
        }
        u_activeList.reserve(EPOLL_MAX_WATCH);
    }

    EPoller::EPoller(EPoller&& epoller) : 
        EventDemultiplexer(),
        u_epollfd(epoller.u_epollfd),
        u_eventMap(std::move(epoller.u_eventMap)),
        u_activeList(std::move(epoller.u_activeList)),
        u_openET(epoller.u_openET),
        u_mutex(),
        u_stopSet(),
        u_admutex(),
        u_addList(),
        u_eraseList()
    {
        epoller.u_epollfd = -1;
        {
            base::MutexLockGuard guard(epoller.u_mutex);
            std::swap(u_stopSet,epoller.u_stopSet);
        }

        {
            base::MutexLockGuard guard(epoller.u_admutex);
            std::swap(u_addList,epoller.u_addList);
            std::swap(u_eraseList,epoller.u_eraseList);
        }
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
        
        {
            base::MutexLockGuard guard(epoller.u_mutex);
            {
                base::MutexLockGuard guard(u_mutex);
                std::swap(u_stopSet,epoller.u_stopSet);
            }
        }

        {
            base::MutexLockGuard guard(epoller.u_admutex);
            {
                base::MutexLockGuard guard(epoller.u_admutex);
                std::swap(u_addList,epoller.u_addList);
                std::swap(u_eraseList,epoller.u_eraseList);
            }
        }

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
        if(event & U_WRITE)
            sysEvent |= EPOLLOUT;
        if(event & U_READ)
            sysEvent |= EPOLLIN;
        sysEvent |= EPOLLONESHOT;   //默认开启ONTSHOT

        if(u_openET)
            sysEvent |= EPOLLET;
        return sysEvent;
    }

    void EPoller::addEvent(int fd,int wevent)
    {
        base::MutexLockGuard guard(u_admutex);
        u_addList.push_back({fd,wevent});
    }
    
    void EPoller::addEventCore()
    {
        base::MutexLockGuard guard(u_admutex);
        while(!u_addList.empty())
        {
            auto pair = u_addList.back();
            u_addList.pop_back();

            auto iter = u_eventMap.find(pair.first);
            struct epoll_event* event = NULL;
            if((iter != u_eventMap.end()))
                event = iter->second;
        
            if(!event)
            {
                event = new epoll_event;
                memset(event,'\0',sizeof(struct epoll_event));
                event->events = switchTo(pair.second); 
                event->data.fd = pair.first;
                ++u_wfds;
                u_eventMap.insert({pair.first,event});
            }
            else
                event->events = switchTo(pair.second);
            
            std::cout << "epollfd: " << u_epollfd << std::endl;
            std::cout << "event fd: " << pair.first << std::endl;
            if(::epoll_ctl(u_epollfd,EPOLL_CTL_ADD,pair.first,event) < 0)
            {
                perror("epoll add error!\n");
                unet::handleError(errno);
            }
        }
    }

    void EPoller::delEvent(int fd)
    {
        base::MutexLockGuard guard(u_admutex);
        u_eraseList.push_back(fd);
    }
    
    void EPoller::delEventCore()
    {
        base::MutexLockGuard guard(u_admutex);
        while(!u_eraseList.empty())
        {
            int fd = u_eraseList.back();
            u_eraseList.pop_back();

            auto iter = u_eventMap.find(fd);
            if(iter == u_eventMap.end())
                continue;
            if(::epoll_ctl(u_epollfd,EPOLL_CTL_DEL,fd,NULL) < 0)
            {
                perror("epoll del error!\n");
                unet::handleError(errno);
            }

            {
                base::MutexLockGuard guard(u_mutex);
                auto iter = u_stopSet.find(-fd);
                if(iter != u_stopSet.end())
                    u_stopSet.erase(iter);

                iter = u_stopSet.find(fd);
                if(iter != u_stopSet.find(fd))
                    u_stopSet.erase(iter);
            }   

            delete iter->second;
            u_eventMap.erase(iter);
            --u_wfds;
        }
    }

    void EPoller::poll(const EventMap& eventMap,std::vector<std::shared_ptr<Event>>& eventList)
    {
        {
            base::MutexLockGuard guard(u_mutex);
            for(auto iter=u_stopSet.begin();iter!=u_stopSet.end();++iter)
            {
                if(*iter > 0)
                {
                    struct epoll_event* event = u_eventMap[*iter];
                    if(event == NULL)
                        continue;
                    
                    if(::epoll_ctl(u_epollfd,EPOLL_CTL_ADD,*iter,event) < 0)
                    {
                        perror("epoll add error!\n");
                        unet::handleError(errno);
                    }

                    u_stopSet.erase(iter);
                }
            }
        }
        
        addEventCore();
        delEventCore();

        u_activeList.clear();
        u_rfds = ::epoll_wait(u_epollfd,&*u_activeList.begin(),EPOLL_MAX_WATCH,EPOLL_TIMEOUT);
        std::cout << "u_rfds: " << u_rfds << std::endl;
        if(u_rfds < 0)
        {
            perror("epoll_wait error!\n");
            unet::handleError(errno);
        }

        if(u_rfds == 0)
            return;
        
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

            {
                base::MutexLockGuard guard(u_mutex);
                u_stopSet.insert(-u_activeList[i].data.fd);
            }

            if(revent)
            {
                std::cout << "event fd: " << u_activeList[i].data.fd << std::endl;
                if(revent & U_READ)
                    std::cout << "read    ";
                if(revent & U_WRITE)
                    std::cout << "write    ";
                if(revent & U_EXCEPTION)
                    std::cout << "exception    ";
                std::cout << std::endl;

                std::shared_ptr<Event> ptr = eventMap.find(u_activeList[i].data.fd);
                if(ptr)
                {
                    ptr->setREvent(revent);
                    eventList.push_back(ptr);
                }
            }
        }
    }
    
    void EPoller::resetEvent(int fd)
    {
        base::MutexLockGuard guard(u_mutex);
        auto iter = u_stopSet.find(-fd);
        if(iter == u_stopSet.end())
            return;
        
        u_stopSet.insert(fd);
        u_stopSet.erase(iter);
    }
}

