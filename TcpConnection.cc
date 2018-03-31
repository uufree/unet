/*************************************************************************
	> File Name: TcpConnection.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分36秒
 ************************************************************************/

#include"TcpConnection.h"
#include<iostream>

namespace unet
{
    TcpConnection::TcpConnection(int fd) : 
        _confd(fd),
        _outputBuffer(new base::StringBuffer(fd)),
        _inputBuffer(new base::StringBuffer(fd))
    {};

    TcpConnection::TcpConnection(TcpConnection&& lhs):
        _confd(std::move(lhs._confd)),
        _outputBuffer(std::move(lhs._outputBuffer)),
        _inputBuffer(std::move(lhs._inputBuffer)),
        _readCallBack(std::move(lhs._readCallBack)),
        _writeCallBack(std::move(lhs._writeCallBack)),
        _closeCallBack(std::move(lhs._closeCallBack))
    {};

    TcpConnection& TcpConnection::operator=(TcpConnection&& lhs)
    {
        if(*this == lhs)
            return *this;
        _confd = std::move(lhs._confd);
        _outputBuffer = std::move(lhs._outputBuffer);
        _inputBuffer = std::move(lhs._inputBuffer);
        _readCallBack = std::move(lhs._readCallBack);
        _writeCallBack = std::move(lhs._writeCallBack);
        _closeCallBack = std::move(lhs._closeCallBack);

        return *this;
    }

    int TcpConnection::read()
    {
        int n = _inputBuffer->readInSocket();
        if(n <= 0)
            _closeCallBack(_confd.getFd());
            
        return n;
    }

    void TcpConnection::handleRead()
    {//处理读事件   
        if(_readCallBack)
            _readCallBack(_inputBuffer,_outputBuffer);
        else
            perror("没有注册readcallback\n");
    }

    void TcpConnection::handleWrite()
    {//处理写事件
        if(_writeCallBack)
            _writeCallBack(_inputBuffer,_outputBuffer);
        else
            perror("没有注册writecallback");
    }

    void TcpConnection::handleClose()
    {   
        if(_closeCallBack)
            _closeCallBack(_confd.getFd());
        else
            perror("没有注册handlediedtcpconnection\n");
    }
}


