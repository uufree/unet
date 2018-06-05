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
    
    /*处理Listen Socket，纳入事件处理框架*/
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
            int getFd() const{return u_listenfd.getFd();};

            void setSaveConnectCallBack(const SaveConnectCallBack& cb)
            {u_saveConnectCallBack = cb;};
            void setSaveListenCallBack(const SaveListenCallBack& cb)
            {u_saveListenCallBack = cb;};
            void setEraseListenCallBack(const EraseListenCallBack& cb)
            {u_eraseListenCallBack = cb;};
        
            void handleRead();

        private:
            bool u_listen;  /*是否处于listen*/
            base::InetAddress u_serverAddr; /*server address*/
            base::Socket u_listenfd;    /*listen socket*/

            /*获取一个连接之后，将连接保存在Server中的统一回调*/
            SaveConnectCallBack u_saveConnectCallBack;  
            /*将Listen Event注册到事件处理框架的回调*/
            SaveListenCallBack u_saveListenCallBack;
            /*将Listen Event从事件处理框架中删除*/
            EraseListenCallBack u_eraseListenCallBack;
    };
}

#endif
