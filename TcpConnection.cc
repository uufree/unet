/*************************************************************************
	> File Name: TcpConnection.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分36秒
 ************************************************************************/

#include"TcpConnection.h"
#include<assert.h>

namespace unet
{
    namespace net
    {
        TcpConnection::TcpConnection(int fd_) : confd(fd_)
        {};

//if inputbuffer over highwater,handle it and put it in outputbuffer,otherwize,update inputbuffer and outputbuffer
        void TcpConnection::handleRead()
        {//处理读事件   
            inputbuffer.readInSocket(confd.getFd());
            readcallback(&inputbuffer,&outputbuffer);
        }

        void TcpConnection::handleWrite()
        {//处理写事件
            outputbuffer.writeInSocket(confd.getFd());
            writecallback(&inputbuffer,&outputbuffer);
        }

        bool TcpConnection::handleWriteForTcpServer()
        {//在TcpServer处理将死未死的连接时，用到这个函数，将buffer中的数据发出去，然后判断是否还有数据
            outputbuffer.writeInSocket(confd.getFd());
            return outputbuffer.getDataSize() == 0;
        }
    
        bool TcpConnection::handleReadForTcpClient()
        {
            inputbuffer.readInSocket(confd.getFd());
            return inputbuffer.getDataSize();
        }

        void TcpConnection::handleClose()
        {
            int fd = confd.getFd();//得到fd

            if(outputbuffer.getDataSize() != 0)
            {
                outputbuffer.writeInSocket(fd);//将数据发送出去    
                handleWriteForTcpServer();
                socket::close(fd);
                handlediedtcpconnection(this);
/*
                if(!handleWriteForTcpServer())
                {//如果发送完毕之后还有残余数据，关闭可读事件
                    socket::shutdownRead(fd);
                    handlereaddiedtcpconnection(shared_from_this());//改变外面TcpServer中的索引
                }
                else
                {
                    socket::close(fd);//如果发送完毕了，直接关闭套接字
                    handlediedtcpconnection(shared_from_this());
                }
*/
            }
        }

    }
}





