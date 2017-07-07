/*************************************************************************
	> File Name: AsyncAcceptor.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月17日 星期一 20时40分14秒
 ************************************************************************/

#ifndef _ASYNCACCEPTOR_H
#define _ASYNCACCEPTOR_H

#include<functional>
#include"../Channel.h"


namespace unet
{
    namespace net
    {
        class AsyncAcceptor final
        {
            typedef std::function<void(Channel&&)> HandleChannelCallBack;
            
            public:
                explicit AsyncAcceptor(socket::InetAddress& addr_);
                AsyncAcceptor(const AsyncAcceptor& lhs) = delete;
                AsyncAcceptor& operator=(const AsyncAcceptor& lhs) = delete;
                AsyncAcceptor(AsyncAcceptor&& lhs);
                AsyncAcceptor& operator=(AsyncAcceptor&& lhs);
                ~AsyncAcceptor();

                void listen();
                inline bool listened() const;
                inline void setHandleChannelCallBack(const HandleChannelCallBack& cb);

            private:
                void handleRead();
            
            private:
                socket::InetAddress& serveraddr;
                int listenfd;
                bool listening;

                HandleChannelCallBack handleChannelCallBack;
        };
    }
}



















#endif

