/*************************************************************************
	> File Name: SocketEvent.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月18日 星期五 17时40分01秒
 ************************************************************************/

#include"../include/SocketEvent.h"
#include"../include/TcpConnection.h"

namespace unet
{
    //SocketEvent    
    SocketEvent::SocketEvent(int fd,EventType type) :
        u_fd(fd),
        u_type(type)
    {};
    
    SocketEvent::SocketEvent(SocketEvent&& event) :
        u_fd(event.u_fd),
        u_type(event.u_type)
    {};

    SocketEvent& SocketEvent::operator=(SocketEvent&& event)
    {
        if(*this == event)
            return *this;
        u_fd = event.u_fd;
        u_type = event.u_type;

        return *this;
    }
    
    //ListenSocketEvent
    ListenSocketEvent::ListenSocketEvent(int fd,EventType type) : 
        SocketEvent(fd,type)
    {};
    
    ListenSocketEvent::ListenSocketEvent(ListenSocketEvent&& event) :
        SocketEvent(event.u_fd,event.u_type),
        u_readCallBack(event.u_readCallBack),
        u_closeCallBack(event.u_closeCallBack)
    {};
    
    ListenSocketEvent& ListenSocketEvent::operator=(ListenSocketEvent&& event)
    {
        if(*this == event)
            return *this;
        u_fd = event.u_fd;
        u_type = event.u_type;
        u_readCallBack = event.u_readCallBack;
        u_closeCallBack = event.u_closeCallBack;

        return *this;
    }
    
    void ListenSocketEvent::handleEvent(int event)
    {
        if(u_type == U_LISTEN_SOCKET)
        {
            if(event & U_EXCEPTION)
            {
                if(CloseCallBack())
                    CloseCallBack();
                else
                    perror("This is no registration CloseCallBack!\n");
                return;
            }

            if(event & U_READ)
            {
                if(u_readCallBack)
                    u_readCallBack();
                else
                    perror("This is no registration ReadCallBack!\n");
            }
        }
    }
    
    //ConnectSocketEvent
    ConnectSocketEvent::ConnectSocketEvent(int fd,EventType type) :
        SocketEvent(fd,type),
        u_conWPtr()
    {};
    
    ConnectSocketEvent::ConnectSocketEvent(int fd,EventType type,const TcpConnectionPtr& ptr) :
        SocketEvent(fd,type),
        u_conWPtr(ptr)
    {};
    
    ConnectSocketEvent::ConnectSocketEvent(ConnectSocketEvent&& event) :
        SocketEvent(event.u_fd,event.u_type),
        u_conWPtr(event.u_conWPtr.lock())
    {};
    
    ConnectSocketEvent& ConnectSocketEvent::operator=(ConnectSocketEvent&& event)
    {
        if(*this == event)
            return *this;
        u_fd = event.u_fd;
        u_type = event.u_type;
        u_conWPtr = event.u_conWPtr;

        return *this;
    }

    void ConnectSocketEvent::handleEvent(int event)
    {
        if(u_type == U_CONNECT_SOCKET)
        {
            TcpConnectionPtr ptr = u_conWPtr.lock();
            if(!ptr)
                return;

            if(event & U_EXCEPTION)
            {
                ptr->handleClose();
                return;
            }
            
            if(event & U_READ)
                ptr->handleRead();
            if(event & U_WRITE)
                ptr->handleWrite();
        }
    }
}

