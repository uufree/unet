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
        
        Channel::Channel(EventLoop* loop_,int fd_,bool hasconnection_ = true) : loop(loop_),fd(fd_),index(-1),event(0),revent(0),handleeventing(false),hasconnection(hasconnection_)
        {
            if(hasconnection_)
            {
                tcpconnectionptr(new TcpConnection(loop,fd));//创建ptr
                tcpconnectionptr->setResetChannelPtr(std::bind(&Channel::resetChannelPtr,this));//注册reset掉ptr的函数
                tcpconnectionwptr(tcpconnectionptr);//将weak_ptr绑定的shared_ptr上
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
                    if(tcpconnectionwptr.lock())
                        tcpconnectionptr->handleRead();
                    else
                        handleError();
                }
                if(revent & EPOLLOUT)
                {
                    if(tcpconnectionwptr.lock())
                        tcpconnecionptr->handleWrite();
                    else
                        handleError();
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

                if(revent & EPOLLOUT)
                {
                    if(writecallback)
                        writecallback();
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





        
            
