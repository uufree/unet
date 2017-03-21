/*************************************************************************
	> File Name: Epoller.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时01分25秒
 ************************************************************************/

#include"Epoller.h"
#include<sys/epoll.h>
#include<assert.h>
#include<iostream>

namespace unet
{
    namespace net
    {
        Epoller::Epoller() :
            epollfd(::epoll_create(EPOLL_CLOEXEC))
        {//创建一个内核维护的epollfd
            if(epollfd < 0)
            {
                std::cerr << "epollfd create errno" << std::endl;
                exit(0);
            }
        }
        
        Epoller::~Epoller()
        {//关闭epollfd
            ::close(epollfd);
        }

        void Epoller::epoll(ChannelList* channels)
        {//将epoll中活动的事件对应的channel传递进channels
            eventlist.clear();
            int activeEvents = ::epoll_wait(epollfd,&*eventlist.begin(),static_cast<int>(channelmap.size()),timeoutMs);

            int savedErrno = errno;

            if(activeEvents > 0)
            {
                getActiveEvents(channels);//转换epollfd和channel
                std::cout << "get activeEvents" << activeEvents << std::endl;
            }
            else if(activeEvents == 0)
                std::cout << "nothing happended" << std::endl;
            else
            {
                if(savedErrno != EINTR)
                    std::cerr << errno << std::endl;
            }
        }


        void Epoller::getActiveEvents(ChannelList* channels)
        {
            int fd;
            for(EventList::iterator iter=eventlist.begin();iter!=eventlist.end();++iter)
            {   
                fd = iter->data.fd;//转换然后在map中寻找，加入list
                assert(channelmap.find(fd) != channelmap.end());
                channels->push_back(channelmap[fd]);
            }
        }
        
        void Epoller::updateChannel(Channel* channel_)
        {//更新已有channel中关注的事件
            const int index = channel_->getIndex();
            const int fd = channel_->getFd();

            assert(channelmap.find(fd) != channelmap.end());
            assert(channelmap[fd] == channel_);
            assert(fd == index);
            
            if(channel_->getEvent() == KNoneEvent)//如果不关注任何的事件就移除channel
            {
                removeChannel(channel_);
            }

            update(EPOLL_CTL_MOD,channel_);
        }

        void Epoller::addInChannelMap(Channel* channel_)
        {//将channel加入map，顺便设置回调
            const int fd = channel_->getFd();
            assert(channelmap.find(fd) == channelmap.end());
            channelmap[fd] = channel_;
            channel_->setIndex(fd);//设置在map中的索引
            channel_->setEvent();//默认关注可读与可写事件
            channel_->setUpdateCallBack(std::bind(&Epoller::updateChannel,this,std::placeholders::_1));//设置更新的回调
            update(EPOLL_CTL_ADD,channel_);//在epollfd中加入索引
        }

        void Epoller::update(int operation,Channel* channel_)
        {//在epollfd中增加表项
            struct epoll_event event;
            bzero(&event,sizeof(event));
            event.events = channel_->getEvent();
            int fd = event.data.fd = channel_->getFd();
            
            eventlist.push_back(event);//有争议的操作

            if(::epoll_ctl(epollfd,operation,fd,&event) < 0)
            {
                std::cerr << "epol_ctl erorr" << std::endl;
                exit(0);
            }
        }

        bool Epoller::hasChannel(Channel* channel_)
        {//判断是否有channel
            int fd = channel_->getFd();
            ChannelMap::const_iterator iter = channelmap.find(fd);
            return iter!=channelmap.end() && iter->second==channel_;
        }

//remove channelmap epollfd eventlist
        void Epoller::removeChannel(Channel* channel_)
        {//移除channel，分成三步
            int fd = channel_->getFd();

            assert(channelmap[fd] == channel_);
            assert(channel_->getEvent() == KNoneEvent);
//remove in epollfd
            update(EPOLL_CTL_DEL,channel_);
//remove in ChannelMap
            delete channelmap[fd];
            channelmap.erase(fd);
//remove in eventlist
            for(auto iter = eventlist.begin();iter!=eventlist.end();++iter)
            {//这个处理有问题
                if((*iter).data.fd == fd)
                {
                    eventlist.erase(iter);
                    break;
                }
            }
        }

    }
}

