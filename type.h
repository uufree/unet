/*************************************************************************
	> File Name: type.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月18日 星期五 18时24分31秒
 ************************************************************************/

#ifndef _TYPE_H
#define _TYPE_H

#include<cstdint>

//event type
#define U_READ 0x01
#define U_WRITE 0x02
#define U_EXCEPTION 0x04
#define U_TIMEOUT 0x08

//base type
#define U_UDP 0x01
#define U_TCP 0x02
#define U_LISTEN 0x04
#define U_CONNECT 0x08
#define U_SOCKET 0x10

//use type
#define U_TIMER 0x20
#define U_SIGNAL 0x40
#define U_CONNECT_SOCKET U_TCP|U_CONNECT|U_SOCKET
#define U_LISTEN_SOCKET U_TCP|U_LISTEN|U_SOCKET

//event demultiplexer
#define U_SELECT 0x01
#define U_POLL 0x02
#define E_EPOLL 0x04

#endif
