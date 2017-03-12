/*************************************************************************
	> File Name: TcpConnection.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分23秒
 ************************************************************************/

#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H

namespace unet
{
    namespace net
    {
        class TcpConnection final
        {
            public:
                TcpConnection(EventLoop* loop_,int fd_,const InetAddr& serveraddr_,const InetAddr& clientaddr_);
                TcpConnection(const TcpConnection& lhs) = delete;
                TcpConnection& operator=(const TcpConnection& lhs) = delete;
                ~TcpConnection();
//public interface
                void handleRead();
                void handleWrite();
                void handleError();
                void handleClose();

            private:
                EventLoop* loop;
                Socket consocket;
                InetAddr serveraddr,clientaddr;
                OutputBuffer outputbuffer;
                InputBuffer inputbuffer;
        };

    }
}
                

                    







































#endif

