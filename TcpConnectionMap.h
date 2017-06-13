/*************************************************************************
	> File Name: TcpConnectionMap.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月12日 星期一 22时34分26秒
 ************************************************************************/

#ifndef _TCPCONNECTIONMAP_H
#define _TCPCONNECTIONMAP_H

#include"TcpConnection.h"
#include<map>

namespace unet
{
    namespace net
    {
        class TcpConnectionMap final
        {
            public:
                TcpConnectionMap();
                TcpConnectionMap(const TcpConnectionMap&) = delete;
                TcpConnectionMap(TcpConnectionMap&& lhs);
                TcpConnectionMap& operator=(const TcpConnectionMap&) = delete;
                TcpConnectionMap& operator=(TcpConnectionMap&& lhs);
                ~TcpConnectionMap();

                void swap(TcpConnectionMap& lhs) = delete;

                void size() const;
                bool empty() const;
                void insert(int fd);
                void erase(int fd);

            private:
                std::map<int,TcpConnection&&> tcpConnectionMap;
        };
    }
}



#endif
