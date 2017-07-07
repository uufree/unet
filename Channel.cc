/*************************************************************************
	> File Name: Channel.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 20时56分25秒
 ************************************************************************/
#include"Channel.h"

namespace unet
{
    namespace net
    {
        
        Channel::Channel(int fd_,ChannelType type_) : 
            fd(fd_),
            event(0),
            revent(0),
            handleEventing(false),
            type(type_)
        {};

        Channel::Channel(Channel&& lhs) : 
            fd(lhs.fd),
            event(lhs.event),
            revent(lhs.revent),
            handleEventing(false),
            type(lhs.type)
        {};

        Channel& Channel::operator=(Channel&& lhs)
        {
            fd = lhs.fd;
            event = lhs.event;
            revent = lhs.revent;
            handleEventing = lhs.handleEventing;
            type = lhs.type;
            
            return *this;
        }

        Channel::~Channel()
        {};

        void Channel::handleEvent(TcpConnectionMap& tcpconnectionMap)
        {
            if(type == LISTEN || type == CLOCK)
            {
                handleEventing = true;
                if((revent & EPOLLHUP) || (revent & EPOLLRDHUP) || (revent & EPOLLERR))
                {
                    if(closeCallBack)
                        closeCallBack(fd);
                    else
                        perror("没有注册CloseCallBack\n");
                }
                else if(revent & EPOLLIN)
                {
                    if(readCallBack)
                        readCallBack();
                    else
                        perror("没有注册ReadCallBack\n");
                }
                else
                {
                    if(closeCallBack)
                        closeCallBack(fd);
                    else
                        perror("没有注册CloseCallBack\n");
                }

                revent = 0;
                handleEventing = false;
            }
            else if(type == CONNECT)
            {
                handleEventing = true;
                TcpConnection& connection = tcpconnectionMap.find(fd);
                
                if((revent & EPOLLHUP) || (revent & EPOLLRDHUP) || (revent & EPOLLERR))
                {
                    if(closeCallBack)
                        closeCallBack(fd);
                    else
                        perror("没有注册CloseCallBack\n");
                }
                else if(revent & EPOLLIN)
                {//存在无法正常关闭connection的问题
                    connection.handleRead();
                }
                else if(revent & EPOLLOUT)
                {
                    connection.handleWrite();
                }
                else
                {
                    if(closeCallBack)
                        closeCallBack(fd);
                    else
                        perror("没有注册CloseCallBack\n");
                }

                revent = 0;
                handleEventing = false;
            }
            else
            {
                perror("此Channel没有注册类型");
            }
        }
        
        int Channel::getFd() const
        {return fd;};

        void Channel::setEvent()
        {event = KWriteEvent & KReadEvent;};

        int Channel::getEvent() const
        {return event;};

        void Channel::setRevent(int revent_)
        {revent = revent_;};
        
        int Channel::getType() const
        {return type;};

        void Channel::setReadCallBack(const ReadCallBack& lhs)
        {
            if(type == LISTEN || type == CLOCK)
                readCallBack = lhs;
        };

        void Channel::setcloseCallBack(const CloseCallBack& lhs)
        {closeCallBack = lhs;};

        bool Channel::isNoneEvent() const
        {return event == KNoneEvent;};

        bool Channel::isReading() const
        {return event == KReadEvent;};

        bool Channel::isWriting() const
        {return event == KWriteEvent;};
    }
}





        
            
