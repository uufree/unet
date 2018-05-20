/*************************************************************************
	> File Name: Selecter.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 12时39分14秒
 ************************************************************************/

#include<sys/select.h>

#include"Event.h"
#include"EventMap.h"
#include"Selecter.h"
#include"EventMap.h"
#include"type.h"

namespace unet
{
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
}

