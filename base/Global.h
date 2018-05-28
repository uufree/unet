/*************************************************************************
	> File Name: global.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年03月28日 星期三 02时30分35秒
 ************************************************************************/

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include<string>
#include<fcntl.h>
#include<cstring>
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/stat.h>

namespace unet
{
    void handleError(int saveErrno);
    inline pthread_t tid() {return ::pthread_self();};
    inline pid_t pid() {return ::syscall(SYS_gettid);};
    
    int readn(int fd,char* cptr,size_t nbytes);
    int writen(int fd,char const* cptr,size_t nbytes);
    void daemonize();
};

#endif
