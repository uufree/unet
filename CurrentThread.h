/*************************************************************************
	> File Name: CurrentThread.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月26日 星期日 16时19分37秒
 ************************************************************************/

#ifndef _CURRENTTHREAD_H
#define _CURRENTTHREAD_H

#include<sys/syscall.h>
#include<unistd.h>

namespace unet
{
    namespace current
    {
        __thread pthread_t threadid_;
        __thread pid_t pid_;

        inline pid_t pid()
        {
            pid_ = ::syscall(SYS_gettid);
            return pid_;
        }
    }
}




#endif

