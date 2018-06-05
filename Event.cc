/*************************************************************************
	> File Name: Event.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月18日 星期五 17时18分52秒
 ************************************************************************/

#include"SocketEvent.h"
#include"SignalEvent.h"
#include"TimerEvent.h"
#include"Event.h"

namespace unet
{
    Event::Event(EventType type,int fd) :
        u_fd(fd),
        u_type(type),
        u_wevent(0),
        u_revent(0)
    {
        /*根据Type重置事件，并根据type设置观察事件*/
        if(u_type == U_CONNECT_SOCKET)
        {
            u_event.u_socket = new ConnectSocketEvent(fd,type);
            u_wevent = U_READ | U_WRITE | U_EXCEPTION;
        }
        else if(u_type == U_LISTEN_SOCKET)
        {
            u_event.u_socket = new ListenSocketEvent(fd,type);
            u_wevent = U_READ | U_EXCEPTION;
        }
        else if(u_type == U_TIMER)
        {
            u_event.u_timer = new TimerEvent();
            u_fd = u_event.u_timer->getTimerfd();
            u_wevent = U_READ | U_EXCEPTION;
        }
        else if(u_type == U_SIGNAL)
        {
            u_event.u_signal = new SignalEvent();
            u_fd = 0;   /*信号事件的描述符默认为0*/
            u_wevent = U_READ | U_EXCEPTION;
        }
    };

    Event::~Event()
    {
        if(u_type == U_CONNECT_SOCKET)
            delete u_event.u_socket;
        else if(u_type == U_LISTEN_SOCKET)
            delete u_event.u_socket;
        else if(u_type == U_TIMER)
            delete u_event.u_timer;
        else if(u_type == U_SIGNAL)
            delete u_event.u_signal; 
    };
    
    /*根据type处理事件*/
    void Event::handleEvent()
    {
        if(u_type == U_LISTEN_SOCKET)
            u_event.u_socket->handleEvent(u_revent & u_wevent);
        else if(u_type == U_CONNECT_SOCKET)
            u_event.u_socket->handleEvent(u_revent & u_wevent);
        else if(u_type == U_TIMER)
            u_event.u_timer->handleEvent();
        else if(u_type == U_SIGNAL)
            u_event.u_signal->handleEvent();
       
        if(!(u_revent & U_EXCEPTION))
        {
            if(u_resetCallBack)
                u_resetCallBack(u_fd);
            else
            {
                perror("There is no register resetCallBack!\n");
                unet::handleError(errno);
            }
        }
        u_revent = 0;
    }

    void Event::setListenReadCallBack(const ListenReadCallBack& cb)
    {
        if(u_type == U_LISTEN_SOCKET)
            u_event.u_socket->setReadCallBack(cb);
    }

    void Event::setListenCloseCallBack(const ListenCloseCallBack& cb)
    {
        if(u_type == U_LISTEN_SOCKET)
            u_event.u_socket->setCloseCallBack(cb);
    }

    void Event::setTcpConnectionPtr(const std::shared_ptr<TcpConnection>& ptr)
    {
        if(u_type == U_CONNECT_SOCKET)
            u_event.u_socket->setTcpConnectionPtr(ptr);
    }

    void Event::startTimerEvent()
    {
        if(u_type == U_TIMER)
            u_event.u_timer->start();
    }

    void Event::stopTimerEvent()
    {
        if(u_type == U_TIMER)
            u_event.u_timer->stop();
    }

    void Event::addTimerWithLock(std::shared_ptr<Timer> ptr)
    {
        if(u_type == U_TIMER)
            u_event.u_timer->addTimerWithLock(ptr);
    }
}
