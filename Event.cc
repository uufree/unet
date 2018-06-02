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
#include"type.h"

namespace unet
{
    Event::Event(int fd,int type,int wevent) :
        u_fd(fd),
        u_type(type),
        u_wevent(wevent),
        u_revent(0)
    {
        if(u_type & U_CONNECT_SOCKET)
            u_event.u_socket = new ConnectSocketEvent(fd,type);
        else if(u_type & U_LISTEN_SOCKET)
            u_event.u_socket = new ListenSocketEvent(fd,type);
        else if(u_type & U_TIMER)
            u_event.u_timer = new TimerEvent();
        else if(u_type & U_SIGNAL)
            u_event.u_signal = new SignalEvent(); 
    };

    Event::~Event()
    {
        if(u_type & U_CONNECT_SOCKET)
            delete u_event.u_socket;
        else if(u_type & U_LISTEN_SOCKET)
            delete u_event.u_socket;
        else if(u_type & U_TIMER)
            delete u_event.u_timer;
        else if(u_type & U_SIGNAL)
            delete u_event.u_signal; 
    };
    
    void Event::handleEvent()
    {
        switch(u_type)
        {
            case U_CONNECT_SOCKET:
            {
                u_event.u_socket->handleEvent(u_revent & u_wevent);
                break;
            }
            case U_TIMER:
            {
                u_event.u_timer->handleEvent();
                break;
            }
            case U_LISTEN_SOCKET:
            {
                u_event.u_socket->handleEvent(u_revent & u_wevent);
                break;
            }
            case U_SIGNAL:
                break;
        }
        
        u_revent = 0;
    }
}
