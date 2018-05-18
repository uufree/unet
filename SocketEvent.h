/*************************************************************************
	> File Name: SocketEvent.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月18日 星期五 17时39分54秒
 ************************************************************************/

#ifndef _SOCKETEVENT_H
#define _SOCKETEVENT_H

#include<memory>
#include<functional>

#include"TcpConnection.h"
#include"type.h"

namespace unet
{
    //class Event的1/3
    class SocketEvent final
    {
        typedef std::function<void(int)> ReadCallBack;
        typedef std::function<void(int)> WriteCallBack;
        typedef std::function<void(int)> CloseCallBack;
        typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

        public:
            SocketEvent(int _fd,int _type);
            SocketEvent(const SocketEvent&) = delete;
            SocketEvent& operator=(const SocketEvent&) = delete;
            SocketEvent(SocketEvent&&);
            SocketEvent& operator=(SocketEvent&&);
            ~SocketEvent();
            
            bool operator==(const SocketEvent& event){return u_fd==event.u_fd && u_type==event.u_type;};

            void handleEvent(int event);//由class Event告知SocketEvent发生了什么
            int getType() const {return u_type;};
            void setTcpConnectionPtr(const TcpConnectionPtr& ptr){u_conPtr = ptr;};
            void setReadCallBack(const ReadCallBack& callBack){u_readCallBack = callBack;};
            void setWriteCallBack(const WriteCallBack& callBack){u_writeCallBack = callBack;};
            void setCloseCallBack(const CloseCallBack& callBack){u_writeCallBack = callBack;};

        private:
            int u_fd;
            int u_type;
            TcpConnectionPtr u_conPtr;

            ReadCallBack u_readCallBack;
            WriteCallBack u_writeCallBack;
            CloseCallBack u_closeCallBack;
    };
}


#endif
