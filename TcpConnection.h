/*************************************************************************
	> File Name: TcpConnection.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分23秒
 ************************************************************************/

#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H

/* 设计宗旨：TcpConnection描述一个连接，内部状态机表征状态 
 * 
 * 1.管理socket的生命期
 * 2.持有并且管理与Socket相对应的Buffer
 * 3.设置处理通信回调函数，若回调函数可重入，持有回调函数的引用即可
*/

#include<functional>
#include<memory>
#include"base/Buffer.h"
#include"base/Socket.h"

namespace unet
{
    class TcpConnection final
    {
        typedef std::shared_ptr<base::Buffer> BufferPtr;
        typedef std::function<void (BufferPtr& inputBuffer,BufferPtr& outputBuffer)> MessageCallBack;
        typedef std::function<void(int)> CloseCallBack;
//unqiue_ptr不支持多态??
        public:        
            explicit TcpConnection(int fd_);
            TcpConnection(const TcpConnection& lhs) = delete;
            TcpConnection(TcpConnection&& lhs);
            TcpConnection& operator=(const TcpConnection& lhs) = delete;
            TcpConnection& operator=(TcpConnection&& lhs);
            ~TcpConnection(){};
            
            bool operator==(const TcpConnection& con) {return _confd==con._confd;};
            void setReadCallBack(const MessageCallBack& cb){_readCallBack = cb;};
            void setWriteCallBack(const MessageCallBack& cb){_writeCallBack = cb;};
            void setCloseCallBack(const CloseCallBack& cb){_closeCallBack = cb;};
            int getFd() const {return _confd.getFd();};

            int read();
            void handleRead();//用于处理描述符上发生的事件
            void handleWrite();
            void handleClose(); 

        private:
            base::Socket _confd;
            BufferPtr _outputBuffer;
            BufferPtr _inputBuffer;
            MessageCallBack _readCallBack,_writeCallBack;
            CloseCallBack _closeCallBack;
    };
}
                

                    







































#endif

