/*************************************************************************
	> File Name: TcpConnectionMap.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月12日 星期一 22时34分35秒
 ************************************************************************/

#include"TcpConnectionMap.h"

namespace unet
{
    namespace net
    {
        TcpConnectionMap::TcpConnectionMap()
        {};

        TcpConnectionMap::TcpConnectionMap(TcpConnectionMap&& lhs) :
            tcpConnectionMap(std::move(lhs.tcpConnectionMap))
        {};

        TcpConnectionMap& TcpConnectionMap::operator=(TcpConnectionMap&& lhs)
        {
            tcpConnectionMap = std::move(lhs.tcpConnectionMap);
            return *this;
        }

        int TcpConnectionMap::size() const
        {
            return tcpConnectionMap.size();
        }

        bool TcpConnectionMap::empty() const
        {
            return tcpConnectionMap.empty();
        }

        void TcpConnectionMap::insert(int fd)
        {
            TcpConnection tcp(fd);
            tcpConnectionMap.insert({fd,std::move(tcp)});
        }

        void TcpConnectionMap::erase(int fd) 
        {
            tcpConnectionMap.erase(fd);
        }
        
        TcpConnection& TcpConnectionMap::find(int fd)
        {
            return tcpConnectionMap[fd];
        }

    }
}

