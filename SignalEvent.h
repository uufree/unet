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
#include"type.h"

namespace unet
{

    class SignalEvent final
    {
        public:
            SignalEvent();
            SignalEvent(const SignalEvent& sig) = delete;
            SignalEvent(SignalEvent&& sig);
            SignalEvent& operator=(const SignalEvent& sig) = delete;
            SignalEvent& operator=(SignalEvent&& sig) = delete;
            ~SignalEvent();

            bool operator==(const SignalEvent& signal){return u_signalfd==signal.u_signalfd;};

            void addSignal(int sig);
            void eraseSignal(int sig);
            void start();
            void stop();
        
        private:
            void handleRead();

        private:
            int u_signalfd; 
    };
}

#endif
