/*************************************************************************
	> File Name: TcpConnection.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分23秒
 ************************************************************************/

#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H

#include<functional>

class Buffer;
class Socket;
class EventLoop;

namespace unet
{
    namespace net
    {
        class TcpConnection final
        {
            public:
                typedef std::function<void (Buffer* inputbuffer_,Buffer* outputbuffer_)> MessageCallBack;
                typedef std::function<void()> ResetChannelPtr;
                typedef std::function<void(int fd)> ChangeTcpMapIndex;
                
                TcpConnection(EventLoop* loop_,int fd_);
                TcpConnection(const TcpConnection& lhs) = delete;
                TcpConnection& operator=(const TcpConnection& lhs) = delete;
                ~TcpConnection();
//public interface
                void setReadCallBack(const MessageCallBakc& cb)
                {readcallback = cb;};

                void setWriteCallBack(const MessageCallBack& cb)
                {writecallback = cb;};

                void setResetChannelPtr(const ResetChannelPtr& cb)
                {resetchannelptr = cb;};
                
                void setChangeTcpMapIndex(const ChangeTcpMapIndex& cb)
                {changetcpmapindex = cb;};

                bool handleWriteForTcpServer();

                void resetChannelPtr()
                {resetchannelptr();};

                void handleRead();
                void handleWrite();
                void handleClose();

            private:

                EventLoop* loop;
                Socket confd;
                Buffer outputbuffer;
                Buffer inputbuffer;
                MessageCallBack readcallback;
                MessageCallBack writecallback;
                ResetChannelPtr resetchannelptr;
                ChangeTcpMapIndex changeTcpMapIndex;
        };

    }
}
                

                    







































#endif

