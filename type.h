/*************************************************************************
	> File Name: type.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月18日 星期五 18时24分31秒
 ************************************************************************/

#ifndef _TYPE_H
#define _TYPE_H

#include<cstdint>

//socket type
#define LISTEN 0x01
#define COMMON 0x02
#define UDP 0x04
#define TCP 0x08

//Event type
//all event
#define U_READ 0x0001
#define U_WRIET 0x0002

//timer event 
#define U_TIMEOUT 0x0004

//signal event
#define U_SIGHUP 0x0008
#define U_SIGPIPE 0x0010
#define U_SIGURG 0x0020
#define U_SIGCHLD 0x0040
#define U_SIGTERM 0x0080
#define U_SIGINT 0x0100

//epoll operator type
#define U_EPOLL_LT 0x01
#define U_EPOLL_ET 0x02

#endif
