/*************************************************************************
	> File Name: SignalEvent.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年04月01日 星期日 02时08分32秒
 ************************************************************************/

#ifndef _SIGNALEVENT_H
#define _SIGNALEVENT_H

#include<sys/types.h>
#include<signal.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<bitset>
#include"base/global.h"

/* 0: SIGHUP 挂起进程的控制终端
 * 1: SIGPIPE 往一个关闭的管道写数据
 * 2: SIGURG 带外数据到达
 * 3: SIGCHLD 子进程状态发生变化
 * 4: SIGTERM 终止进程 
 * 5: SIGINT 键盘输入终止进程
*/

namespace unet
{

    class SignalEvent final
    {
        public:
            SignalEvent();
            SignalEvent(const SignalEvent& sig) = delete;
            SignalEvent(SignalEvent&& sig) = delete;
            SignalEvent& operator=(const SignalEvent& sig) = delete;
            SignalEvent& operator=(SignalEvent&& sig) = delete;
            ~SignalEvent();

            bool operator==(const SignalEvent& sig){return _pipefd[0]==sig._pipefd[0];};

            void addSignal(int sig);
            void eraseSignal(int sig);
            void start();
            void stop();
        
        private:
            void handleRead();

        private:
            int _pipefd[2]; 
            std::bitset<8> _bitmap;
    };
}

#endif
