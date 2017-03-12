/*************************************************************************
	> File Name: Channel.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 20时56分13秒
 ************************************************************************/

#ifndef _CHANNEL_H
#define _CHANNEL_H

namespace unet
{
    namespace net
    {
        class Channel
        {
            public:
                Channel(int fd_,bool hasconnection);
                ~Channel() {};
                Channel(const Channel& lhs) = delete;
                Channel& operator=(const Channel& lhs)  = delete;
                
//public interface
                void handleEvent();
                void remove();

                void setReadCallBack(const ReadCallBack& cb)
                {readcallback = cb;};

                void setWriteCallBack(const WriteCallBack& cb)
                {writecallback = cb;};

                void setErrorCallBack(const ErrorCallBack& cb)
                {errorcallback = cb;};

                void setCloseCallBack(const CloseCallBack& cb)
                {closecallback = cb;};

                void setUpdateCallBack(const UpdateCallBack& cb)
                {updatecallback = cb;};

                void setRemoveCallBack(const RemoveCallBack& cb)
                {removecallback = cb;};

                bool isNewChannel() const {return isnewchannel;};
                int getIndex() const {return index;};
                int getFd() const {return fd;};
                void setIndex(int index_) {index = index_};
                
                int getEvent() const {return event;};
                void setRevent(int revent_) {revent = revent_;};

                bool isNoneEvent() const {return event == KNoneEvent;};
                bool isReading() const {return event == KReadEvent;};
                bool isWriting() const {return event == KWirteEvent;};

                void enableReading() 
                {
                    event|=KReadEvent;
                    updatecallback(this);
                };

                void enableWriting() 
                {
                    event|=KWriteEvent;
                    updatecallback(this);
                };

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

                TcpConnectionPtr getTcpConnectionPtr()
                {return tcpConnectionptr;};
            
            private:
                typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
                typedef std::function<void()> EventCallBack; 
                typedef std::function<void(Channel* channel_)> UpdateCallBack;
                typedef std::function<void(Channel* channel_)> RemoveCallBack;

                const int fd;
                int index;
                int event;
                int revent;
                bool isnewchannel;
                bool isinepoll;
                bool handleeventing;
                bool hasconnection;
                TcpConnectionPtr tcpconnectionptr;

                static const int KNoneEvent;
                static const int KReadEvent;
                static const int KWriteEvent;

                EventCallBack readcallback;
                EventCallBack writecallback;
                EventCallBack errorcallback;
                EventCallBack closecallback;
                UpdateCallBack updatecallback;
                RemoveCallBack removecallback;
        };

    }
}



#endif

