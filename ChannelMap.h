/*************************************************************************
	> File Name: ChannelMap.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月12日 星期一 22时34分47秒
 ************************************************************************/

#ifndef _CHANNELMAP_H
#define _CHANNELMAP_H

#include"Channel.h"
#include"Mutex.h"
#include<vector>
#include<map>

namespace unet
{
    namespace net
    {
        class ChannelMap final
        {
            explicit ChannelMap();
            ChannelMap(const ChannelMap& lhs) = delete;
            ChannelMap(ChannelMap&& lhs);
            ChannelMap& operator=(const ChannelMap& lhs) = delete;
            ChannelMap& operator=(ChannelMap&& lhs);
            ~ChannelMap();

            void swap(ChannelMap& lhs) = delete;
            
            int size() const;
            bool empty() const;
            void insert(int fd);
            void erase(int fd);

            private:
                std::vector<int> channelList;
                std::map<int,Channel> channelMap;
                mutable thread::MutexLock lock;
        };
    }
}






















#endif
