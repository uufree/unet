/*************************************************************************
	> File Name: TcpConnection.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分36秒
 ************************************************************************/

#include"TcpConnection.h"
#include<assert.h>
#include<iostream>

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
            if(readcallback)
                readcallback(&inputbuffer,&outputbuffer);
            else
                std::cout << "没有注册readcallback" << std::endl;
        }

        void TcpConnection::handleWrite()
        {//处理写事件
            if(writecallback)
                writecallback(&inputbuffer,&outputbuffer);
            else
                std::cout << "没有注册writecallback" << std::endl;
            outputbuffer.writeInSocket(confd.getFd());
        }

        bool TcpConnection::handleWriteForTcpServer()
        {//在TcpServer处理将死未死的连接时，用到这个函数，将buffer中的数据发出去，然后判断是否还有数据
            outputbuffer.writeInSocket(confd.getFd());
            return outputbuffer.getDataSize() == 0;
        }
    
        bool TcpConnection::handleReadForTcpClient()
        {
            inputbuffer.readInSocket(confd.getFd());
            return inputbuffer.getKey() == 0;
        }

        void TcpConnection::handleClose()
        {
            handlediedtcpconnection(confd.getFd());
        }

        void TcpConnection::handleChannel()
        {
            wheetchannel();
        }
    }
}





