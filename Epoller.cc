/*************************************************************************
	> File Name: Epoller.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时01分25秒
 ************************************************************************/

#include"Epoller.h"
#include<sys/epoll.h>
#include<poll.h>
#include<assert.h>

namespace unet
{
    namespace net
    {
        Epoller::Epoller(EventLoop* loop_) : loop(loop_)
            epollfd(::epoll_create(EPOLL_CLOEXEC))
        {
            if(epollfd < 0)
            {
                std::cout << "epollfd create errno" << std::endl;
                exit(0);
            }
        }
        
        Epoller::~Epoller()
        {
            ::close(epollfd);
        }

        Timestamp Epoller::epoll(int timeoutMs)
        {
            eventlist.clear();
            int activeEvents = ::epoll_wait(epollfd,&*eventlist.begin(),static_cast<int>(channelmap.size()),timeoutMs);

            Timestamp now(Timestamp::now());
            int savedErrno = errno;

            if(activeEvents > 0)
            {
                getActiveEvents(activeChannels);
                std::cout << "get activeEvents" << activeEvents << std::endl;
            }
            else if(activeEvents == 0)
                std::cout << "nothing happended" << std::endl;
            else
            {
                if(savedErrno != EINTR)
                    std::cerr << errno << std::endl;
            }
            return now;
        }


        void getActiveEvents()
        {//将获得结果写进EventLoop
            int fd;
            loop->clearActiveChannelList();
            ChannelList* loopChannelList = loop->getActiveChannelList;
            for(EventList::iterator iter=eventlist.begin();iter!=eventlist.end();++iter)
            {   
                fd = iter->data.fd;
                assert(channelmap.find(fd) != channelmap.end());
                loopChannelList_->push_back(channelmap[fd]);
            }
        }


        void Epoller::updateChannel(Channel* channel_)
        {//only change channel in events
            loop->assertInLoopThread();
            const int index = channel_->getIndex();
            const int fd = channel_->getFd();

            if(channel_->isNewChannel())//new
            {
                assert(channels.find(fd) == channels.end());
                channels[fd] = channel;
                channel_->setIsNewChannel(false);
                channel_->setIndex(fd);
                update(EPOLL_CTL_ADD,channel_);
            }
            else//change mode or delete
            {
                assert(channels.find(fd) != channels.end());
                assert(channels[fd] == channel);
                if(fd == index)//change mode
                {
                    update(EPOLL_CTL_MOD,channel_);
                }
                else//delete
                {
                    assert(fd == -index);
                    update(EPOLL_CTL_DEL,channel_)
                }
            }
        }

        void Epoller::update(int operation,Channel* channel_)
        {
            struct epoll_event event;
            bzero(&evnet,sizeof(event));
            event.events = channel_->getEvent();
            int fd = event.data.fd = channel_->getFd();

            if(::epoll_ctl(epollfd,operation,fd,&event) < 0)
            {
                std::cerr << "epol_ctl erorr" << std::endl;
                exit(0);
            }
        }

        bool Epoller::hasChannel(Channel* channel_)
        {
            loop->assertInLoopThread();
            int fd = channel_->getFd();
            ChannelMap::const_iterator iter = channelmap.find(fd);
            return iter!=channelmap.end() && iter.second==channel_;
        }

        void removeChannel(Channel* channel_)
        {
            loop->assertInLoopThread();
            int fd = channel_->getFd();
            assert(channelmap[fd] != channelmap.end());
            assert(channelmap[fd] == channel_);








        }

    }
}

