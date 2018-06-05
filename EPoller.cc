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
        if(u_epollfd < 0)
        {
            perror("epollfd create error!\n");
            unet::handleError(errno);
        }
        u_activeList.reserve(EPOLL_MAX_WATCH);/*为临时队列分配空间*/
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
        /*逐步清理epoll_event中的数据*/
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
    
    /*异步的添加事件*/
    void EPoller::addEvent(int fd,int wevent)
    {
        base::MutexLockGuard guard(u_admutex);
        u_addList.push_back({fd,wevent});
    }
    
    /*逐个将add的事件添加到epollfd中*/
    void EPoller::addEventCore()
    {
        base::MutexLockGuard guard(u_admutex);
        while(!u_addList.empty())
        {
            /*vector从尾部操作比较好*/
            auto pair = u_addList.back();
            u_addList.pop_back();
            
            /*判断这个事件是否出现在事件中*/
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
            
            /*将事件添加到epollfd中*/
            if(::epoll_ctl(u_epollfd,EPOLL_CTL_ADD,pair.first,event) < 0)
            {
                perror("epoll add error!\n");
                unet::handleError(errno);
            }
        }
    }
    
    /*由于设置了ONESHOT选项，del操作会保存，也不知道是不是已经真的从内核中移除了*/
    void EPoller::delEvent(int)
    {
/*        
        base::MutexLockGuard guard(u_admutex);
        u_eraseList.push_back(fd);
*/    
    }
    
    void EPoller::delEventCore()
    {
/*        
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
*/    
    }
    
    /*用于分离事件的统一的接口*/
    void EPoller::poll(const EventMap& eventMap,std::vector<std::shared_ptr<Event>>& eventList)
    {
        /*由于设计问题，重新关注当前的事件*/
        {
            base::MutexLockGuard guard(u_mutex);
            for(auto iter=u_stopSet.begin();iter!=u_stopSet.end();++iter)
            {
                if(*iter > 0)
                {
                    struct epoll_event* event = u_eventMap[*iter];
                    if(event == NULL)
                        continue;
                    
                    if(::epoll_ctl(u_epollfd,EPOLL_CTL_MOD,*iter,event) < 0)
                    {
                        perror("epoll add error!\n");
                        unet::handleError(errno);
                    }

                    u_stopSet.erase(iter);
                }
            }
        }
        
        /*在epoll_wait之前重新添加与删除事件*/
        addEventCore();
        delEventCore();
            
        u_activeList.clear();/*清理临时列表*/
        u_rfds = ::epoll_wait(u_epollfd,&*u_activeList.begin(),EPOLL_MAX_WATCH,EPOLL_TIMEOUT);
        
        if(u_rfds < 0)
        {
            perror("epoll_wait error!\n");
            unet::handleError(errno);
        }
        
        /*没有事件发生的话直接退出*/
        if(u_rfds == 0)
            return;
        
        int revent = 0;
        for(int i=0;i<u_rfds;++i)
        {
            /*将sys event重新转化为usr event*/
            revent = 0;
            if((u_activeList[i].events & EPOLLERR) || (u_activeList[i].events & EPOLLHUP) || (u_activeList[i].events & EPOLLRDHUP))
                revent |= U_EXCEPTION;
            if(u_activeList[i].events & EPOLLIN)
                revent |= U_READ;
            if(u_activeList[i].events & EPOLLOUT)
                revent |= U_WRITE;
    
            /*使用了一个小技巧，将暂时不需要关注的事件置为负号*/
            {
                base::MutexLockGuard guard(u_mutex);
                u_stopSet.insert(-u_activeList[i].data.fd);
            }

            if(revent)
            {
                std::shared_ptr<Event> ptr = eventMap.find(u_activeList[i].data.fd);
                /*将事件添加到事件处理队列*/
                if(ptr)
                {
                    ptr->setREvent(revent);
                    eventList.push_back(ptr);
                }
            }
        }
    }
    
    /*重置事件，异步过程*/
    void EPoller::resetEvent(int fd)
    {
        base::MutexLockGuard guard(u_mutex);
        auto iter = u_stopSet.find(-fd);
        if(iter == u_stopSet.end())
            return;
        
        /*将负号置为正的，在稍后重置完成*/
        u_stopSet.insert(fd);
        u_stopSet.erase(iter);
    }
}

