/*************************************************************************
	> File Name: TcpConnection.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分36秒
 ************************************************************************/

#include"TcpConnection.h"
#include"Buffer.h"
#include"Socket.h"
#include"EventLoop.h"
#include<assert.h>

namespace unet
{
    namespace net
    {
        TcpConnection::TcpConnection(EventLoop* loop_,int fd_) :
            loop(loop_),confd(fd_)
        {};

//if inputbuffer over highwater,handle it and put it in outputbuffer,otherwize,update inputbuffer and outputbuffer
        void TcpConnection::handleRead()
        {   
            inputbuffer.readInSocket(confd.getFd());
            messagecallback(&inputbuffer,&outputbuffer);
        }

        void TcpConnection::handleWrite()
        {
            outputbuffer.writeInSocket(confd.getFd());
            messagecallback(&inputbuffer,&outputbuffer);
        }

        bool TcpConnection::handleWriteForTcpServer()
        {
            outputbuffer.writeInSocket(confd.getFd());
            return outputbuffer.getDataSize == 0;
        }

        void TcpConnection::handleClose()
        {
            int fd = confd.getFd();

            if(outputbuffer.getDataSize() != 0)
            {
                outputbuffer.writeInSocket(fd);    
                if(outputbuffer.getDataSize() != 0)
                {
                    socket::shutdownRead(fd);
                    changetcpmapindex(fd);
                }
                else
                {
                    socket::close(fd);
                }
            }
        }

    }
}





