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

#include"Type.h"

namespace unet
{
    class TcpConnection;

    /*unet关注的三种事件中一种：Socket Event*/
    /*以为Connect Socket与Listen Socket不完全相同，所以使用继承的方式抽象部分资
     * 源*/
    class SocketEvent 
    {
        protected:
            typedef std::function<void()> ReadCallBack;
            typedef std::function<void(int)> CloseCallBack;
            typedef std::weak_ptr<TcpConnection> TcpConnectionWPtr;
            typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

        public:
            /*Functionality：
             *      创建根据不同的Socket类型创建一个事件，不维护任何系统资源
             *Parameters：
             *      1)[in]：事件所依赖的Socket，不管理生命周期
             *      2)[in]：所关注的事件类型：
             *          U_CONNECT_SOCKET：Connect Socket
             *          U_LISTEN_SOCKET：Listen Socket
             */
            explicit SocketEvent(int,EventType);
            SocketEvent(const SocketEvent&) = delete;
            SocketEvent& operator=(const SocketEvent&) = delete;
            SocketEvent(SocketEvent&&);
            SocketEvent& operator=(SocketEvent&&);
            virtual ~SocketEvent(){};
            
            bool operator==(const SocketEvent& event){return u_fd==event.u_fd && u_type==event.u_type;};

            virtual void handleEvent(int){};
            int getType() const {return u_type;};
            int getFd() const{return u_fd;};
            virtual void setReadCallBack(const ReadCallBack&){};
            virtual void setCloseCallBack(const CloseCallBack&){};
            virtual void setTcpConnectionPtr(const TcpConnectionPtr&){};

        protected:
            int u_fd;
            EventType u_type;
    };
    
    class ListenSocketEvent final : public SocketEvent
    {
        public:
            explicit ListenSocketEvent(int fd,EventType type);
            ListenSocketEvent(const ListenSocketEvent&) = delete;
            ListenSocketEvent& operator=(const ListenSocketEvent&) = delete;
            ListenSocketEvent(ListenSocketEvent&&);
            ListenSocketEvent& operator=(ListenSocketEvent&&);
            ~ListenSocketEvent() override{};
            
            bool operator==(const ListenSocketEvent& event) {return u_fd == event.u_fd && u_type == event.u_type;};

            void setReadCallBack(const ReadCallBack& callBack)override {u_readCallBack = callBack;};
            void setCloseCallBack(const CloseCallBack& callBack) override {u_closeCallBack = callBack;};

            /*Functionality:
             *      处理事件的核心步骤
             *Parameters：
             *      1)[in]:
             *          U_READ：可读事件
             *          U_WRITE：可写事件
             *          U_EXCEPTION：异常事件
             */
            void handleEvent(int) override;

        private:
            ReadCallBack u_readCallBack;
            CloseCallBack u_closeCallBack;
    };

    class ConnectSocketEvent final : public SocketEvent
    {
        public:
            explicit ConnectSocketEvent(int fd,EventType type);
            ConnectSocketEvent(int fd,EventType type,const TcpConnectionPtr& ptr);
            ConnectSocketEvent(const ConnectSocketEvent&) = delete;
            ConnectSocketEvent& operator=(const ConnectSocketEvent&) = delete;
            ConnectSocketEvent(ConnectSocketEvent&&);
            ConnectSocketEvent& operator=(ConnectSocketEvent&&);
            ~ConnectSocketEvent() override {};
            
            bool operator==(const ConnectSocketEvent& event){return event.u_fd==u_fd && event.u_type==u_type;};

            void handleEvent(int) override;
            void setTcpConnectionPtr(const TcpConnectionPtr& ptr) override {u_conWPtr = ptr;};
            
        private:
            TcpConnectionWPtr u_conWPtr;
    };
}


#endif
