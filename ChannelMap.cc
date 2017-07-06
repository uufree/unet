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
            channelList(std::move(lhs.channelList)),
            channelMap(std::move(lhs.channelMap))
        {};

        ChannelMap& ChannelMap::operator=(ChannelMap&& lhs)
        {
            channelList = std::move(lhs.channelList);
            channelMap = std::move(lhs.channelMap);

            return *this;
        }

        int ChannelMap::size() const 
        {
            return channelList.size();
        }

        bool ChannelMap::empty() const
        {
            return channelList.empty();
        }

        void ChannelMap::insert(int fd,ChannelType type)
        {
            channelList.insert(fd);
            
            net::Channel channel_(fd,type);
            channelMap.insert({fd,std::move(channel_)});
        }

        void ChannelMap::erase(int fd)
        {
            channelList.erase(fd);
            channelMap.erase(fd);
        }

        Channel& ChannelMap::findChannel(int fd)
        {
           return channelMap[fd]; 
        }

    }
}

