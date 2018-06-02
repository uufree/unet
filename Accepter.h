/*************************************************************************
	> File Name: Accepter.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月02日 星期六 20时05分06秒
 ************************************************************************/

#ifndef _ACCEPTER_H
#define _ACCEPTER_H

#include"base/InetAddress.h"
#include"base/Socket.h"

#include<functional>
#include<memory>

namespace unet
{
    class Event;

    class Accepter final
    {
        typedef std::shared_ptr<Event> EventPtr;
        typedef std::function<void(int)> SaveListenCallBack;
        typedef std::function<void(int)> SaveConnectCallBack;
        typedef std::function<void(int)> EraseListenCallBack;
        public:
            explicit Accepter(base::InetAddress& serverAddr);
            Accepter(const Accepter&) = delete;
            Accepter(Accepter&&);
            Accepter& operator=(const Accepter&) = delete;
            Accepter& operator=(Accepter&&);
            ~Accepter();
            
            bool operator==(const Accepter& accepter){return u_listenfd==accepter.u_listenfd;};

            int startListen();
            int stopListen();
            bool isListen() const{return u_listen;};
            
            void setSaveConnectCallBack(const SaveConnectCallBack& cb)
            {u_saveConnectCallBack = cb;};
            void setSaveListenCallBack(const SaveListenCallBack& cb)
            {u_saveListenCallBack = cb;};
            void setEraseListenCallBack(const EraseListenCallBack& cb)
            {u_eraseListenCallBack = cb;};
        
        private:
            void handleRead();

        private:
            bool u_listen;
            base::InetAddress u_serverAddr;
            base::Socket u_listenfd;

            SaveConnectCallBack u_saveConnectCallBack;
            SaveListenCallBack u_saveListenCallBack;
            EraseListenCallBack u_eraseListenCallBack;
    };
}

#endif
