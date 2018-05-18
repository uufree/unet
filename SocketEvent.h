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
    class SocketEvent 
    {
        protected:
            typedef std::function<void(int)> ReadCallBack;
            typedef std::function<void(int)> CloseCallBack;
            typedef std::weak_ptr<TcpConnection> TcpConnectionWPtr;
            typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

        public:
            SocketEvent(int _fd,int _type);
            SocketEvent(const SocketEvent&) = delete;
            SocketEvent& operator=(const SocketEvent&) = delete;
            SocketEvent(SocketEvent&&);
            SocketEvent& operator=(SocketEvent&&);
            virtual ~SocketEvent(){};
            
            bool operator==(const SocketEvent& event){return u_fd==event.u_fd && u_type==event.u_type;};

            virtual void handleEvent(int){};
            int getType() const {return u_type;};
            int getFd() const{return u_fd;};

        protected:
            int u_fd;
            int u_type;
    };
    
    class ListenSocketEvent final : public SocketEvent
    {
        public:
            ListenSocketEvent(int fd,int type);
            ListenSocketEvent(const ListenSocketEvent&) = delete;
            ListenSocketEvent& operator=(const ListenSocketEvent&) = delete;
            ListenSocketEvent(ListenSocketEvent&&);
            ListenSocketEvent& operator=(ListenSocketEvent&&);
            ~ListenSocketEvent() override {};
            
            bool operator==(const ListenSocketEvent& event) {return u_fd == event.u_fd && u_type == event.u_type;};

            void setReadCallBack(const ReadCallBack& callBack){u_readCallBack = callBack;};
            void setCloseCallBack(const CloseCallBack& callBack){u_closeCallBack = callBack;};
            void handleEvent(int) override;

        private:
            ReadCallBack u_readCallBack;
            CloseCallBack u_closeCallBack;
    };

    class CommonSocketEvent final : public SocketEvent
    {
        public:
            CommonSocketEvent(int fd,int type);
            CommonSocketEvent(int fd,int type,const TcpConnectionPtr& ptr);
            CommonSocketEvent(const CommonSocketEvent&) = delete;
            CommonSocketEvent& operator=(const CommonSocketEvent&) = delete;
            CommonSocketEvent(CommonSocketEvent&&);
            CommonSocketEvent& operator=(CommonSocketEvent&&);
            ~CommonSocketEvent() override {};
            
            bool operator==(const CommonSocketEvent& event){return event.u_fd==u_fd && event.u_type==u_type;};

            void handleEvent(int) override;
            void setTcpConnectionPtr(const TcpConnectionPtr& ptr){u_conWPtr = ptr;};
            
        private:
            TcpConnectionWPtr u_conWPtr;
    };
}


#endif
