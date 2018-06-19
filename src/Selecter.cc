/*************************************************************************
	> File Name: Selecter.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 12时39分14秒
 ************************************************************************/

#include<sys/select.h>

#include"../include/Event.h"
#include"../include/EventMap.h"
#include"../include/Selecter.h"
#include"../include/Type.h"

namespace unet
{
    Selecter::Selecter() :
        EventDemultiplexer(),
        u_maxfd(0),
        u_set(),
        u_timeout(new timeval),
        u_mutex(),
        u_stopMap(),
        u_admutex(),
        u_addList(),
        u_eraseList()
    {
        FD_ZERO(&u_readSet);
        FD_ZERO(&u_readSetSave);
        FD_ZERO(&u_writeSet);
        FD_ZERO(&u_writeSetSave);
        FD_ZERO(&u_exceptionSet);
        FD_ZERO(&u_exceptionSetSave);
        
        u_timeout->tv_sec = 0;
        u_timeout->tv_usec = SELECT_TIMEOUT;
    };

    Selecter::Selecter(Selecter&& select) :
        EventDemultiplexer(),
        u_readSet(select.u_readSet),
        u_writeSet(select.u_writeSet),
        u_exceptionSet(select.u_exceptionSet),
        u_readSetSave(select.u_readSetSave),
        u_writeSetSave(select.u_writeSetSave),
        u_exceptionSetSave(select.u_exceptionSetSave),
        u_maxfd(select.u_maxfd),
        u_set(select.u_set),
        u_timeout(select.u_timeout),
        u_mutex(),
        u_stopMap(),
        u_admutex(),
        u_addList(),
        u_eraseList()
    {
        {
            base::MutexLockGuard guard(select.u_mutex);
            std::swap(u_stopMap,select.u_stopMap);
        }
        
        {
            base::MutexLockGuard guard(select.u_admutex);
            std::swap(u_addList,select.u_addList);
            std::swap(u_eraseList,select.u_eraseList);
        }
    };

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
        u_maxfd = select.u_maxfd;
        u_set = select.u_set;
        u_timeout = select.u_timeout;
        
        {
            base::MutexLockGuard guard(select.u_mutex);
            {
                base::MutexLockGuard guard(u_mutex);
                std::swap(u_stopMap,select.u_stopMap);
            }
        }
        
        {
            base::MutexLockGuard guard(select.u_admutex);
            {
                base::MutexLockGuard guard(u_admutex);
                std::swap(u_addList,select.u_addList);
                std::swap(u_eraseList,select.u_eraseList);
            }
        }

        return *this;
    }
    
    Selecter::~Selecter()
    {}
    
    /*异步的将关注的事件添加到关注列表*/
    void Selecter::addEvent(int fd,int wevent)
    {
        base::MutexLockGuard guard(u_admutex);
        u_addList.push_back({fd,wevent});
    }
    
    /*在Pool loop中主动的清空异步线程中的事件，将需要观察的事件添加到观察列表中*/
    void Selecter::addEventCore()
    {
        base::MutexLockGuard guard(u_admutex);
        while(!u_addList.empty())
        {
            auto iter = u_addList.back();
            u_addList.pop_back();

            if(u_set.find(iter.first) == u_set.end())
            {
                ++u_wfds;
                u_set.insert(iter.first);
            }

            if(iter.second & U_EXCEPTION)
            {
                FD_SET(iter.first,&u_exceptionSet);
                FD_SET(iter.first,&u_exceptionSetSave);
            }

            if(iter.second & U_WRITE)
            {
                FD_SET(iter.first,&u_writeSet);
                FD_SET(iter.first,&u_writeSetSave);
            }
        
            if(iter.second & U_READ)
            {
                FD_SET(iter.first,&u_readSet);
                FD_SET(iter.first,&u_readSetSave);
            }
            
            /*根据需要，调整maxfd*/
            if(iter.first > u_maxfd)
                u_maxfd = iter.first;
        }
    }
    
    /*异步的删除事件*/
    void Selecter::delEvent(int fd)
    {
        base::MutexLockGuard guard(u_admutex);
        u_eraseList.push_back(fd);
    }
    
    /*在Pool loop中主动地删除异步列表中想要删除的事件*/
    void Selecter::eraseEventCore()
    {
        base::MutexLockGuard guard(u_admutex);
        while(!u_eraseList.empty())
        {
            int fd = u_eraseList.back();
            u_eraseList.pop_back();

            auto iter = u_set.find(fd);
            if(iter == u_set.end())
                return;
        
            {
                base::MutexLockGuard guard(u_mutex);
                auto miter = u_stopMap.find(fd);
                if(miter != u_stopMap.end())
                    u_stopMap.erase(miter);
            
                miter = u_stopMap.find(-fd);
                if(miter != u_stopMap.end())
                    u_stopMap.erase(miter);
            }

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
    }

    void Selecter::poll(const EventMap& eventMap,std::vector<std::shared_ptr<Event>>& eventList)
    {
        /*重新将停止列表中的事件添加到关注列表中*/
        {
            base::MutexLockGuard guard(u_mutex);
            int revent = 0;
            for(auto iter=u_stopMap.begin();iter!=u_stopMap.end();++iter)
            {
                revent = 0;
                if(iter->first > 0)
                {
                    revent = iter->second;
                    if(revent & U_READ)
                    {
                        FD_SET(iter->first,&u_readSet);
                        FD_SET(iter->first,&u_readSetSave);
                    }
                    
                    if(revent & U_WRITE)
                    {
                        FD_SET(iter->first,&u_writeSet);
                        FD_SET(iter->first,&u_writeSetSave);
                    }

                    if(revent & U_EXCEPTION)
                    {
                        FD_SET(iter->first,&u_exceptionSet);
                        FD_SET(iter->first,&u_exceptionSetSave);
                    }

                    u_stopMap.erase(iter);
                }
            }
        }
        
        /*主动地添加与删除事件*/
        addEventCore();
        eraseEventCore();
        
        u_rfds = ::select(u_maxfd+1,&u_readSet,&u_writeSet,&u_exceptionSet,u_timeout);
        u_timeout->tv_usec = SELECT_TIMEOUT;/*select会清空填充的延迟时间*/
        
        if(u_rfds < 0)
        {
            perror("select error!\n");
            unet::handleError(errno);
            return;
        }
        
        /*因为select会清空关注列表，重新填充关注列表*/
        if(u_rfds == 0)
        {
            u_readSet = u_readSetSave;
            u_writeSet = u_writeSetSave;
            u_exceptionSet = u_exceptionSetSave;
            return;
        }

        int revent;
        for(auto iter=u_set.begin();iter!=u_set.end();++iter)
        {
            revent = 0;
            if(FD_ISSET(*iter,&u_readSet))
                revent |= U_READ;
            if(FD_ISSET(*iter,&u_writeSet))
                revent |= U_WRITE;
            if(FD_ISSET(*iter,&u_exceptionSet))
                revent |= U_EXCEPTION;
            
            if(revent)
            {
                std::shared_ptr<Event> ptr = eventMap.find(*iter);
                if(ptr)
                {
                    ptr->setREvent(revent);
                    eventList.push_back(ptr);
                }
                
                /*将关注事件fd变为-fd，然后清空关注的事件，等待重置*/
                {
                    base::MutexLockGuard guard(u_mutex);
                    u_stopMap.insert({-(*iter),revent});
                }

                if(revent & U_READ)
                    FD_CLR(*iter,&u_readSetSave);
                if(revent & U_WRITE)
                    FD_CLR(*iter,&u_writeSetSave);
                if(revent & U_EXCEPTION)
                    FD_CLR(*iter,&u_exceptionSetSave);
            }
        }

        u_readSet = u_readSetSave;
        u_writeSet = u_writeSetSave;
        u_exceptionSet = u_exceptionSetSave;
    }
    
    /*重置事件，删除-fd，重新添加+fd*/
    void Selecter::resetEvent(int fd)
    {
        base::MutexLockGuard guard(u_mutex);
        auto iter = u_stopMap.find(-fd);
        if(iter == u_stopMap.end())
            return;
        u_stopMap.insert({-(iter->first),iter->second});
        u_stopMap.erase(iter);
    }

}

