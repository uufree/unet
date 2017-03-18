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
                
                Channel(int fd_,bool hasconnection = true);
                ~Channel();
                Channel(const Channel& lhs) = delete;
                Channel& operator=(const Channel& lhs)  = delete;
                
//public interface
                void handleEvent();
                void handleClose();
                void handleError();

                void setReadCallBack(const ReadCallBack& cb)
                {readcallback = cb;};//在没有TcpConnection的情况下由Listenfd和Timefd注册

                void setUpdateCallBack(const UpdateCallBack& cb)
                {updatecallback = cb;};//Epoller注册的

                int getIndex() const {return index;};//得到在epoller中的索引
                int getFd() const {return fd;};//得到fd

                void setIndex(int index_) {index = index_};//设置在epoller中的索引        

                //设置关注的事件，默认关注读写事件，看情况关闭一些事件
                void setEvent() {evnet |= KReadEvent & KWriteEvent;};
                
                //得到关注的事件
                int getEvent() const {return event;};

                //设置正在发生的事件
                void setRevent(int revent_) {revent = revent_;};

                bool isNoneEvent() const {return event== KNoneEvent;};
                bool isReading() const {return event == KReadEvent;};
                bool isWriting() const {return event == KWirteEvent;};

                void disableReading() 
                {
                    event|=~KReadEvent;
                    updatecallback(this);//更新操作是关键
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
            
                void resetChannelPtr()//由TcpServer调用
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

