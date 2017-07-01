/*************************************************************************
	> File Name: Epoller.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时01分25秒
 ************************************************************************/

#include"Epoller.h"
#include"error.h"

namespace unet
{
    namespace net
    {
        Epoller::Epoller() : epollfd(::epoll_create(65536))
        {
            if(epollfd < 0)
                unet::handleError(errno);
        }                     
  
        Epoller::~Epoller()
        {
            ::close(epollfd);
        }

        void Epoller::epoll(ChannelList& channelList,ChannelMap& channelMap)
        {
            int activeEvents = ::epoll_wait(epollfd,&*eventList.begin(),static_cast<int>(eventList.size()),timeoutMs);

            if(activeEvents > 0)
                getActiveEvents(activeEvents,channelMap,channelList);
            else if(activeEvents == 0)
                std::cout << "nothing happened!" << std::endl; 
            else
                unet::handleError(errno);
        }

        void Epoller::getActiveEvents(int activeEvents,ChannelMap& channelMap,ChannelList& channeList)
        {
            for(int i=0;i<activeEvents;++i)
            {
                if(eventList[i].events & EPO)
            }
        }
        
        void Epoller::updateChannel(Channel* channel_)
        {//更新已有channel中关注的事件
            const int fd = channel_->getFd();
            
            
            assert(channelmap.find(fd) != channelmap.end());
            assert(channelmap[fd] == channel_);
            
            if(channel_->getEvent() == KNoneEvent)//如果不关注任何的事件就移除channel
            {
                removeChannel(channel_);
            }

//            update(EPOLL_CTL_MOD,channel_);
            update(channel_);
        }

        void Epoller::addInChannelMap(Channel* channel_)
        {//将channel加入map，顺便设置回调
            unet::thread::MutexLockGuard guard(mutex);
            const int fd = channel_->getFd();
            assert(channelmap.find(fd) == channelmap.end());
            channelmap[fd] = channel_;
            channel_->setEvent();//默认关注可读与可写事件
            channel_->setUpdateCallBack(std::bind(&Epoller::updateChannel,this,std::placeholders::_1));//设置更新的回调
//            update(channel_);//在epollfd中加入索引
            
            struct pollfd pfd;
            pfd.fd = channel_->getFd();
            pfd.events = channel_->getEvent();
            eventlist.push_back(pfd);
            
//            getInfo();
            channel_->handleDrived();//处理主动的事件;
        }
/*
        void Epoller::update(Channel* channel_)
        {//在epollfd中增加表项
            struct epoll_event event;
            bzero(&event,sizeof(event));
            event.events = channel_->getEvent();
            event.data.fd = channel_->getFd();
            int fd = channel_->getFd();   

            if(::epoll_ctl(epollfd,operation,fd,&event) < 0)
            {
                std::cerr << "epol_ctl erorr" << std::endl;
                int save = errno;
                if(save == EBADF)
                    std::cout << "EBADF" << std::endl;
                if(save == EEXIST)
                    std::cout << "EEXIST" << std::endl;
                if(save == EINVAL)
                    std::cout << "EINVAL" << std::endl;
                if(save == ENOENT)
                    std::cout << "ENONET" << std::endl;
                if(save == ENOMEM)
                    std::cout << "ENOMEM" << std::endl;
                if(save ==  EPERM)
                    std::cout << "EPERM" << std::endl;
            }
                exit(0);

            eventlist.push_back(event);//有争议的操作
        }
*/
        void Epoller::update(Channel* channel_)
        {
            int fd = channel_->getFd();
            for(auto iter=eventlist.begin();iter!=eventlist.end();++iter)
            {
                if(iter->fd == fd)
                {
                    iter->events = channel_->getEvent();
                    break;
                }
            }
        }

        void Epoller::getInfo() const
        {
            std::cout << "-----------------------------------------" << std::endl;
            std::cout << "pthread number: " << ::pthread_self() << std::endl; 
            std::cout << "channelmap size: " << channelmap.size() << std::endl;
            std::cout << "eventlist size: " << eventlist.size() << std::endl;
            std::cout << "---------------------------------" << std::endl;
        }


        bool Epoller::hasChannel(Channel* channel_) const
        {//判断是否有channel
            int fd = channel_->getFd();
            ChannelMap::const_iterator iter = channelmap.find(fd);
            return iter!=channelmap.end() && iter->second==channel_;
        }

//remove channelmap eventlist
        void Epoller::removeChannel(Channel* channel_)
        {//移除channel，分成2步
            int fd = channel_->getFd();

            assert(channelmap[fd] == channel_);
            assert(channel_->getEvent() == KNoneEvent);

//remove in ChannelMap
            delete channelmap[fd];
            channelmap.erase(fd);

//remove in eventlist
            for(auto iter = eventlist.begin();iter!=eventlist.end();++iter)
            {//这个处理有问题
                if(iter->fd == fd)
                {
                    eventlist.erase(iter);
                    break;
                }
            }
        }

    }
}

