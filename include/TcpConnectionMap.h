 /*************************************************************************
	> File Name: TcpConnectionMap.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月12日 星期一 22时34分26秒
 ************************************************************************/

//由Server统一管理的数据集,表示连接

#ifndef _TCPCONNECTIONMAP_H
#define _TCPCONNECTIONMAP_H

#include"Mutex.h"
#include"TcpConnection.h"

#include<map>
#include<memory>

/*2018.06.01 测试完成*/

namespace unet
{
    /*由TcpServer统一管理TcpConnect资源，为删除，添加，查找创建遍历*/
    class TcpConnectionMap final
    {
        typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
        friend bool operator==(const TcpConnectionMap& lhs,const TcpConnectionMap& rhs);

        public:
            TcpConnectionMap(){};
            TcpConnectionMap(const TcpConnectionMap&) = delete;
            TcpConnectionMap(TcpConnectionMap&& lhs);
            TcpConnectionMap& operator=(const TcpConnectionMap&) = delete;
            TcpConnectionMap& operator=(TcpConnectionMap&& lhs);
            ~TcpConnectionMap(){};
            
            int size() const {return u_tcpConnectionMap.size();};
            bool empty() const {return u_tcpConnectionMap.empty();};
            void insert(int fd);
            void insert(const TcpConnectionPtr& lhs);
            void erase(int fd);
            TcpConnectionPtr& find(int fd);

        private:
            base::MutexLock u_mutex;
            std::map<int,TcpConnectionPtr> u_tcpConnectionMap;
    };
    
    bool operator==(const TcpConnectionMap& lhs,const TcpConnectionMap& rhs);
}



#endif
