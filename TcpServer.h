/*************************************************************************
	> File Name: TcpServer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月02日 星期六 18时55分29秒
 ************************************************************************/

#ifndef _TCPSERVER_H
#define _TCPSERVER_H

#include"base/InetAddress.h"
#include"base/Buffer.h"
#include"TcpConnectionMap.h"
#include"EventMap.h"
#include"EventDemultiplexer.h"

#include<functional>
#include<memory>

namespace unet
{
    class TcpServer final
    {
        typedef std::shared_ptr<base::Buffer> BufferPtr;
        typedef std::function<void(BufferPtr)> ReadCallBack;
        typedef std::function<void(BufferPtr)> WriteCallBack;

        public:
            explicit TcpServer(base::InetAddress& serverAddr);
            TcpServer(const TcpServer&) = delete;
            TcpServer(TcpServer&&);
            TcpServer& operator=(const TcpServer&) = delete;
            TcpServer& operator=(TcpServer&&);
            ~TcpServer();

            void setReadCallBack(const ReadCallBack& cb);
            void setWriteCallBack(const WriteCallBack& cb);

        private:
            base::InetAddress u_serverAddr;
            TcpConnectionMap u_connectionMap; 
            EventMap u_eventMap;
            int u_eventDemuType;
            EventDemultiplexer* u_eventDemu;
            
    
            ReadCallBack u_readCallBack;
            WriteCallBack u_writeCallBack;
    };
}

#endif
