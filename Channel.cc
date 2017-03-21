/*************************************************************************
	> File Name: Channel.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 20时56分25秒
 ************************************************************************/
#include<sys/epoll.h>
#include<assert.h>
#include"Channel.h"

namespace unet
{
    namespace net
    {
        const int Channel::KNoneEvent = 0;//关注的事件的处理方式
        const int Channel::KReadEvent = EPOLLIN | EPOLLPRI;
        const int Channel::KWriteEvent = EPOLLOUT;
        
        Channel::Channel(int fd_,bool hasconnection_) : 
        fd(fd_),
        index(-1),
        event(0),
        revent(0),
        handleeventing(false),
        hasconnection(hasconnection_),
        tcpconnectionptr(new TcpConnection(3)),
        tcpconnectionwptr(tcpconnectionptr)
        {
            if(!hasconnection_)
            {
                tcpconnectionptr.reset();
                tcpconnectionwptr.reset();        
            }
        };

        Channel::~Channel()
        {
            assert(handleeventing);
        }

        void Channel::handleEvent()
        {
            if(hasconnection)
            {//处理有TcpConnectionPtr的情况
                handleeventing = true;
                if((revent & EPOLLHUP) && !(revent & EPOLLIN))
                {
                    handleClose();
                }
                if(revent & (EPOLLERR))
                {   
                    handleError();
                }
                if(revent & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
                {
                    {
                        TcpConnectionPtr wptr = tcpconnectionwptr.lock();
                        if(wptr)
                            wptr->handleRead();
                        else
                            handleError();
                    }
                }
                if(revent & EPOLLOUT)
                {
                    {
                        TcpConnectionPtr wptr = tcpconnectionwptr.lock();
                        if(wptr)
                            wptr->handleWrite();
                        else
                            handleError();
                    }
                }
                handleeventing = false;
            }
            else
            {//处理没有TcpConnection的情况，Listenfd等
                handleeventing = true;
                if((revent & EPOLLHUP) && !(revent & EPOLLIN))
                {
                        handleClose();
                }
                if(revent & (EPOLLERR))
                {      
                        handleError();
                }

                if(revent & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
                {
                    if(readcallback)
                        readcallback();
                }
                handleeventing = false;
            }
        }
        
        void Channel::handleClose()
        {//处理正常关闭的情况
            if(hasconnection)
            {
                TcpConnectionPtr conptr(tcpconnectionwptr.lock());
                conptr->handleClose();
            }
            disableAll();//这个函数里面会有一个更新的操作
        }

        void Channel::handleError()
        {//处理错误的情况
            disableAll();
        }

    }
}





        
            
