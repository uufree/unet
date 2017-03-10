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
                Channel();
                ~Channel();
                Channel(const Channel&) = delete;
                Channel& operator=(const Channel&)  =delete;
                
//public interface
                void handleEvent();
                void update();
                void remove();

                void setReadCallBack(const ReadCallBack& cb)
                {ReadCallBack = cb;};

                void setWriteCallBack(const WriteCallBack& cb)
                {WriteCallBack = cb;};

                void setErrorCallBack(const ErrorCallBack& cb)
                {ErrorCallBack = cb;};

                void setCloseCallBack(const CloseCallBack& cb)
                {CloseCallBack = cb;};


                bool isNewChannel() const {return isnewchannel;};
                int getIndex() const {return index;};
                int getFd() const {return fd;};
                void setIndex(int index_) {index = index_};
                
                int getEvent() const {return event;};
                void setRevent(int revent_) {revent = revent_;};

                bool isNoneEvent() const {return event == KNoneEvent;};
                bool isReading() const {return event == KReadEvent;};
                bool isWriting() const {return event == KWirteEvent;};

                void enableReading() {event|=KReadEvent;update();};
                void enableWriting() {event|=KWriteEvent;update();};
                void disableReading() {event|=~KReadEvent;update();};
                void disableWriting() {event|=~KWriteEvent;update();};
            
            private:
                typedef std::shared_ptr<Connection> ConnectionPtr;
                typedef std::function<void()> EventCallBack; 
                    
                EventLoop* loop;
                const int fd;
                int index;
                int event;
                int revent;
                bool isnewchannel;
                bool addinepoller;
                bool fdclose;
                bool handleeventing;
                
                static const int KNoneEvent;
                static const int KReadEvent;
                static const int KWriteEvent;

                EventCallBack ReadCallBack;
                EventCallBack WriteCallBack;
                EventCallBack ErrorCallBack;
                EventCallBack CloseCallBack;
        };

        class ChannelCon : public Channel final
        {
            public:
                ChannelCon(EventLoop* loop_);
                ~ChannelCon();
                ChannelCon(const ChannelCon&) = delete;
                ChannelCon& operator(const ChannleCon&) = delete;

            private:
                typedef shared_ptr<Connection> ConnectionPtr;
        
                ConnectionPtr Connection;
        };
    }
}



#endif

