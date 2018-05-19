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
#define U_LISTEN 0x01
#define U_COMMON 0x02
#define U_UDP 0x04
#define U_TCP 0x08

//Event type
//all event
#define U_READ 0x0001
#define U_WRITE 0x0002
#define U_EXCEPTION 0x0200

//timer event 
#define U_TIMEOUT 0x0004


//epoll operator type
#define U_EPOLL_LT 0x01
#define U_EPOLL_ET 0x02

#endif
