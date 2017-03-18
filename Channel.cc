/*************************************************************************
	> File Name: Channel.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 20时56分25秒
 ************************************************************************/
#include"EventLoop.h"
#include"Channel.h"

namespace unet
{
    namespace net
    {
        const int Channel::KNoneEvent = 0;
        const int Channel::KReadEvent = EPOLLIN | EPOLLPRI;
        const int Channel::KWriteEvent = EPOLLOUT;
        
        Channel::Channel(EventLoop* loop_,int fd_,bool hasconnection) : loop(loop_),fd(fd_),index(-1),event(0),revent(0),handleeventing(false),hasconnection(hasconnection_)
        {
            if(hasconnection_)
            {
                tcpconnectionptr(new TcpConnection(loop,fd));
                tcpconnectionptr->setResetChannelPtr(std::bind(&Channel::resetChannelPtr,this));
                tcpconnectionwptr(tcpconnectionptr);
            }
        };

        Channel::~Channel()
        {
            assert(handleeventing);
            assert(!isinepoll);
        }

        void Channel::handleEvent()
        {
            if(hasconnection)
            {//have connectionptr handle ways
                handleeventing = true;
                if((revent & EPOLLHUP) && !(revent & EPOLLIN))
                {
                    handleClose();
                }
                if(revent & (EPOLLERR | EPOLLNVAL))
                {   
                    handleError();
                }
                if(revent & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
                {
                    if(tcpconnectionwptr.lock())
                        connectionptr->handleRead();
                    else
                        handleClose();
                }
                if(revent & EPOLLOUT)
                {
                    if(tcpconnectionwptr.lock())
                        connecionptr->handleWrite();
                    else
                        handleClose();
                }
                handleeventing = false;
            }
            else
            {//timefd and listenfd handle ways
                handleeventing = true;
                if((revent & EPOLLHUP) && !(revent & POLLIN))
                {
                        handleClose();
                }
                if(revent & (EPOLLERR | EPOLLNVAL))
                {      
                        handleError();
                }

                if(revent & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
                {
                    if(readcallback)
                        readcallback();
                }

                if(revent & EPOLLOUT)
                {
                    if(writecallback)
                        writecallback();
                }
                handleeventing = false;
            }
        }
        
        void Channel::handleClose()
        {
            TcpConnectionPtr conptr(tcpconnectionwptr.lock());
            conptr->handleClose();
            disableAll();
        }

        void Channel::handleError()
        {
            disableAll();
        }

    }
}





        
            
