/*************************************************************************
	> File Name: TcpConnection.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分23秒
 ************************************************************************/

#ifndef _TCPCONNECTION_H
#define _TCPCONNECTION_H

#include<functional>
#include<memory>
#include"base/Buffer.h"
#include"base/Socket.h"

namespace unet
{
    /*关于TcpConnection状态的讨论：
     * 我们不需要为TcpConnection维护状态，因为自接收Confd起，TcpConnection就已经
     * 处于连接状态，TcpConnection只负责管理资源
     */

    /*关于TcpConnection回调函数处理的讨论：
     * 1.用户向TcpServer注册处理逻辑的回调
     * 2.TcpConnection需要自行处理IO
     */
    class TcpConnection final
    {
        typedef std::shared_ptr<base::Buffer> BufferPtr;
        typedef std::function<void (BufferPtr&)> ReadCallBack;
        typedef std::function<void (BufferPtr&)> WriteCallBack;
        typedef std::function<void(int)> CloseCallBack;
        
        public:        
            explicit TcpConnection(int);
            explicit TcpConnection(base::Socket&&);
            TcpConnection(const TcpConnection& lhs) = delete;
            TcpConnection(TcpConnection&& lhs);
            TcpConnection& operator=(const TcpConnection& lhs) = delete;
            TcpConnection& operator=(TcpConnection&& lhs);
            ~TcpConnection();
            
            bool operator==(const TcpConnection& con){return u_confd==con.u_confd;};
            void setReadCallBack(const ReadCallBack& cb){u_readCallBack = cb;};
            void setWriteCallBack(const WriteCallBack& cb){u_writeCallBack = cb;};
            void setCloseCallBack(const CloseCallBack& cb){u_closeCallBack = cb;};
            
            /*因为TcpConnection全方位的管理着Socket，所以需要将Socket函数转接*/ 
            int getFd() const {return u_confd.getFd();};
            bool isReuseAddr() const {return u_confd.isReuseAddr();};
            bool isLinger() const {return u_confd.isLinger();};
            bool isKeepAlive() const {return u_confd.isKeepAlive();};
            bool isNoDelay() const {return u_confd.isNoDelay();};
            bool isNonBlockAndCloseOnExec() const {return u_confd.isNonBlockAndCloseOnExec();};
            
            /*随时调整Socket Buffer的数据*/
            int setSendBuf(int set) {return u_confd.setSendBuf(set);};    
            int setRecvBuf(int set)  {return u_confd.setRecvBuf(set);};
            int getSendBuf() const {return u_confd.getSendBuf();};
            int getRecvBuf() const {return u_confd.getRecvBuf();};
            
            /*随时Socket的低水平线*/
            int getRecvLowAt() const {return u_confd.getRecvLowAt();};
            int getSendLowAt() const {return u_confd.getSendLowAt();};
            
            int setCloseRST() {return u_confd.setCloseRST();};
            int setCloseDelay(int del) {return u_confd.setCloseDelay(del);};
            int setCloseNormal() {return u_confd.setCloseNormal();};
            int setKeepAlive() {return u_confd.setKeepAlive();};
            int setNodelay() {return u_confd.setNodelay();};
            int setNonBlockAndCloseOnExec() {return u_confd.setNonBlockAndCloseOnExec();};
            int delNonBlockAndCloseOnExec() {return u_confd.delNonBlockAndCloseOnExec();}; 

            void handleRead();//用于处理描述符上发生的事件
            void handleWrite();
            void handleClose(); 

        private:
            base::Socket u_confd;
            BufferPtr u_buffer;
            ReadCallBack u_readCallBack;
            WriteCallBack u_writeCallBack;
            CloseCallBack u_closeCallBack;
    };
}


#endif

