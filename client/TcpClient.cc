/*************************************************************************
	> File Name: TcpClient.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月21日 星期二 20时32分19秒
 ************************************************************************/

#include"TcpClient.h"
#include<stdio.h>

namespace unet
{
    namespace net
    {
        TcpClient::TcpClient(InetAddress* serveraddr_) :
            serveraddr(serveraddr_),
            connector(new ConnectorThread(serveraddr_)),
            ptr(nullptr)
        {
            connector->setConnectionCallBack(std::bind(&TcpClient::setTcpConnectionPtr,this,std::placeholders::_1));
            connector->createChannel();
            
            ptr->setHandleDiedTcpConnection(std::bind(&TcpClient::handleDiedTcpConnection,this,std::placeholders::_1));
            ptr->setHandleAsyncBuffer(std::bind(&TcpClient::handleAsyncBuffer,this,std::placeholders::_1));
        };

    }
}
        
            

        
