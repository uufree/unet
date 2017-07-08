/*************************************************************************
	> File Name: ChannelMap.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月12日 星期一 22时34分54秒
 ************************************************************************/

#include"ChannelMap.h"

namespace unet
{
    namespace net
    {
        ChannelMap::ChannelMap()
        {};

        ChannelMap::ChannelMap(ChannelMap&& lhs) : 
            channelMap(std::move(lhs.channelMap))
        {};

        ChannelMap& ChannelMap::operator=(ChannelMap&& lhs)
        {
            channelMap = std::move(lhs.channelMap);

            return *this;
        }

        int ChannelMap::size() const 
        {
            return channelMap.size();
        }

        bool ChannelMap::empty() const
        {
            return channelMap.empty();
        }

        void ChannelMap::insert(int fd,ChannelType type)
        {
            thread::MutexLockGuard guard(mutex);
            net::Channel channel_(fd,type);
            channelMap.insert({fd,std::move(channel_)});
        }

        void ChannelMap::insert(Channel&& channel)
        {
            thread::MutexLockGuard gurad(mutex);
            channelMap.insert({channel.getFd(),std::move(channel)});
        }

        void ChannelMap::erase(int fd)
        {
            thread::MutexLockGuard guard(mutex);
            channelMap.erase(fd);
        }

        Channel& ChannelMap::findChannel(int fd)
        {
           return channelMap[fd]; 
        }

    }
}

