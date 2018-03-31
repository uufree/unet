 /*************************************************************************
	> File Name: TcpConnectionMap.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月12日 星期一 22时34分26秒
 ************************************************************************/

//由Server统一管理的数据集,表示连接

#ifndef _TCPCONNECTIONMAP_H
#define _TCPCONNECTIONMAP_H

#include"base/Mutex.h"
#include"TcpConnection.h"

#include<map>
#include<memory>

namespace unet
{
        
    class TcpConnectionMap final
    {
        typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
        
        public:
            TcpConnectionMap(){};
            TcpConnectionMap(const TcpConnectionMap&) = delete;
            TcpConnectionMap(TcpConnectionMap&& lhs);
            TcpConnectionMap& operator=(const TcpConnectionMap&) = delete;
            TcpConnectionMap& operator=(TcpConnectionMap&& lhs);
            ~TcpConnectionMap(){};

            int size() const {return _tcpConnectionMap.size();};
            bool empty() const {return _tcpConnectionMap.empty();};
            void insert(int fd);
            void insert(const TcpConnectionPtr& lhs);
            void erase(int fd);
            TcpConnectionPtr& find(int fd){return _tcpConnectionMap[fd];};

        private:
            base::MutexLock _mutex;
            std::map<int,TcpConnectionPtr> _tcpConnectionMap;
    };
}



#endif
