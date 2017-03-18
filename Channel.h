/*************************************************************************
	> File Name: Channel.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 20时56分13秒
 ************************************************************************/

#ifndef _CHANNEL_H
#define _CHANNEL_H

//确信讨论一点，当一个连接建立起来之后，关注的事件应该不会再变了
//confd关注可写和可读事件，listenfd关注可读事件,timefd关注可读事件

#include<memory>
#include<functional>

class EventLoop;

namespace unet
{
    namespace net
    {
        class Channel final
        {
            public:
                typedef std::weak_ptr<TcpConnection> TcpConnectionWptr;
                typedef std::function<void()> EventCallBack; 
                typedef std::function<void(Channel* channel_)> UpdateCallBack;
                typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
                
                Channel(int fd_,bool hasconnection);
                ~Channel() {};
                Channel(const Channel& lhs) = delete;
                Channel& operator=(const Channel& lhs)  = delete;
                
//public interface
                void handleEvent();
                void handleClose();
                void handleError();

                void setReadCallBack(const ReadCallBack& cb)
                {readcallback = cb;};

                void setUpdateCallBack(const UpdateCallBack& cb)
                {updatecallback = cb;};

                int getIndex() const {return index;};
                int getFd() const {return fd;};

                void setIndex(int index_) {index = index_};        

                void setEvent() {evnet |= KReadEvent & KWriteEvent;};
                int getEvent() const {return event;};
                void setRevent(int revent_) {revent = revent_;};

                bool isNoneEvent() const {return event== KNoneEvent;};
                bool isReading() const {return event == KReadEvent;};
                bool isWriting() const {return event == KWirteEvent;};

                void disableReading() 
                {
                    event|=~KReadEvent;
                    updatecallback(this);
                };

                void disableWriting() 
                {
                    event|=~KWriteEvent;
                    updatecallback(this);
                };

                void disableAll()
                {
                    event = KNoneEvent;
                    updatecallback(this);
                }

                TcpConnectionPtr getTcpConnectionPtr()
                {return tcpconnectionptr;};
            
                void resetChannelPtr()
                {tcpconnectionptr.reset();};
            
            private:
                EventLoop* loop;
                const int fd;
                int index;
                int event;//关注的事件
                int revent;//正在发生的事件
                bool handleeventing;
                bool hasconnection;
                TcpConnectionWptr tcpconnectionwptr;
                TcpConnectionPtr tcpconnectionptr;

                static const int KNoneEvent;
                static const int KReadEvent;
                static const int KWriteEvent;

                EventCallBack readcallback;
                UpdateCallBack updatecallback;
        };
    }
}

#endif

