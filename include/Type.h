/*************************************************************************
	> File Name: type.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月18日 星期五 18时24分31秒
 ************************************************************************/

#ifndef _TYPE_H
#define _TYPE_H

#include<cstdint>

namespace unet
{
    enum EventType{U_TIMER,U_SIGNAL,U_CONNECT_SOCKET,U_LISTEN_SOCKET};
    enum EventDemultiplexerType{U_SELECT,U_POLL,U_EPOLL};
//event type
#define U_READ 0x01
#define U_WRITE 0x02
#define U_EXCEPTION 0x04
}

#endif
