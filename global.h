/*************************************************************************
	> File Name: global.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年03月28日 星期三 02时30分35秒
 ************************************************************************/

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include<string>
#include<unistd.h>
#include<fcntl.h>
#include<cstring>
#include<cerrno>

namespace unet
{
    int readn(int fd,char* cptr,size_t nbytes);
    int readn(int fd,std::string& buf,size_t nbytes);
        
    int writen(int fd,char const* cptr,size_t nbytes);
    int writen(int fd,const std::string& buf);
    
    inline void handleError(int saveErrno)
    {
        char buf[256];
        bzero(buf,256);
        perror(strerror_r(saveErrno,buf,256));
        exit(1);
    }
};

#endif
