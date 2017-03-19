/*************************************************************************
	> File Name: TcpConnection.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分23秒
 ************************************************************************/

#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H

#include"Buffer.h"
#include"Socket.h"
#include"Typedef.h"

namespace unet
{
    namespace net
    {  
        class TcpConnection final
        {
            typedef std::function<void (Buffer* inputbuffer_,Buffer* outputbuffer_)> MessageCallBack;
            typedef std::function<void()> ResetChannelPtr;
            typedef std::function<void(int fd)> ChangeTcpMapIndex;

            public:        
                TcpConnection(int fd_);
                TcpConnection(const TcpConnection& lhs) = delete;
                TcpConnection& operator=(const TcpConnection& lhs) = delete;
                ~TcpConnection() {};
//public interface
                void setReadCallBack(const MessageCallBack& cb)
                {readcallback = cb;};//由TcpServer注册

                void setWriteCallBack(const MessageCallBack& cb)
                {writecallback = cb;};//由TcpServer注册

                void setResetChannelPtr(const ResetChannelPtr& cb)
                {resetchannelptr = cb;};//由Channel注册，目的是为了reset掉Channel中的shared_ptr
                
                void setChangeTcpMapIndex(const ChangeTcpMapIndex& cb)
                {changetcpmapindex = cb;};//由TcpServer注册

                bool handleWriteForTcpServer();

                void resetChannelPtr()
                {resetchannelptr();};//由Channel注册，使用这个函数可以reset掉Channel中的shared_ptr。这个函数由TcpServer使用
                //最精华的是注册的顺序，真的很精彩啊

                void handleRead();
                void handleWrite();
                void handleClose();

            private:
                socket::Socket confd;//处理已连接套接字的关闭问题
                Buffer outputbuffer;//消息输出的buffer
                Buffer inputbuffer;//消息输入的buffer
                MessageCallBack readcallback, writecallback;
                ResetChannelPtr resetchannelptr;
                ChangeTcpMapIndex changetcpmapindex;//处理在TcpServer中的索引问题
        };
    }
}
                

                    







































#endif

