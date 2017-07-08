/*************************************************************************
	> File Name: TcpConnection.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分23秒
 ************************************************************************/

#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H

#include"Typedef.h"
#include"Buffer.h"
#include"Socket.h"

/* 设计宗旨：TcpConnection描述一个连接，内部状态机表征状态 
 * 1.管理socket的生命期
 * 2.

*/

namespace unet
{
    namespace net
    {  
        class TcpConnection final
        {
            typedef std::function<void (Buffer* inputbuffer_,Buffer* outputbuffer_)> MessageCallBack;
            typedef std::function<void(int)> CloseCallBack;

            public:        
                explicit TcpConnection(int fd_);
                TcpConnection(const TcpConnection& lhs) = delete;
                TcpConnection(TcpConnection&& lhs);
                TcpConnection& operator=(const TcpConnection& lhs) = delete;
                TcpConnection& operator=(TcpConnection&& lhs);
                ~TcpConnection();
                
                inline void setReadCallBack(const MessageCallBack& cb);
                
                inline void setWriteCallBack(const MessageCallBack& cb); 
                inline int getFd() const;

                void handleRead();//用于处理描述符上发生的事件
                void handleWrite();
                void handleClose(); 

            private:
                socket::Socket confd;
                Buffer outputBuffer;
                Buffer inputBuffer;
                MessageCallBack readCallBack,writeCallBack;
                CloseCallBack closeCallBack;
        };
    }
}
                

                    







































#endif

