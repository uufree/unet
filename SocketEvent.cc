/*************************************************************************
	> File Name: SocketEvent.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月18日 星期五 17时40分01秒
 ************************************************************************/

#include"SocketEvent.h"
#include"type.h"
#include"TcpConnection.h"

namespace unet
{
    //SocketEvent    
    SocketEvent::SocketEvent(int fd,int type) :
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
    ListenSocketEvent::ListenSocketEvent(int fd,int type) : 
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
        if(u_type & U_LISTEN_SOCKET)
        {
            if((event & U_EXCEPTION))
            {
                if(u_closeCallBack)
                    u_closeCallBack(u_fd);
                else
                    perror("This is no registration CloseCallBack!\n");
            }
            else if(event & U_READ)
            {
                if(u_readCallBack)
                    u_readCallBack(u_fd);
                else
                    perror("This is no registration ReadCallBack!\n");
            }
            else if(event & U_WRITE)
                perror("ListenSocketEvent is no registration WriteCallBack!\n");
        }
    }
    
    //CommonSocketEvent
    CommonSocketEvent::CommonSocketEvent(int fd,int type) :
        SocketEvent(fd,type),
        u_conWPtr()
    {};
    
    CommonSocketEvent::CommonSocketEvent(int fd,int type,const TcpConnectionPtr& ptr) :
        SocketEvent(fd,type),
        u_conWPtr(ptr)
    {};
    
    CommonSocketEvent::CommonSocketEvent(CommonSocketEvent&& event) :
        SocketEvent(event.u_fd,event.u_type),
        u_conWPtr(event.u_conWPtr.lock())
    {};
    
    CommonSocketEvent& CommonSocketEvent::operator=(CommonSocketEvent&& event)
    {
        if(*this == event)
            return *this;
        u_fd = event.u_fd;
        u_type = event.u_type;
        u_conWPtr = event.u_conWPtr;

        return *this;
    }

    void CommonSocketEvent::handleEvent(int event)
    {
        if(u_type & U_COMMON_SOCKET)
        {
            TcpConnectionPtr ptr = u_conWPtr.lock();
            if(!ptr)
                return;

            if(event & U_READ)
                ptr->handleRead();
            if(event & U_WRITE)
                ptr->handleWrite();
            if(event & U_EXCEPTION)
                ptr->handleClose();
        }
    }
}

