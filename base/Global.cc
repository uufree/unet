/*************************************************************************
	> File Name: Global.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月29日 星期二 04时47分00秒
 ************************************************************************/

#include"Global.h"

namespace unet
{
    /*你敢信，这就是我的异常处理机制....*/
    void handleError(int saveErrno)
    {
        char buf[256];
        bzero(buf,256);
        perror(strerror_r(saveErrno,buf,256));
        exit(1);
    }
    
    /*UNP和CSAPP上出现的阻塞读与阻塞写*/
    int readn(int fd,char* cptr,size_t nbytes)
    {
        int nleft,nread;
            
        int readsize = 0;
        nleft = nbytes;

        while(nleft > 0)
        {
            if((nread=read(fd,cptr,nleft)) < 0)
            {
                if(errno == EINTR)
                    nread = 0;
                else
                    return -1;
            }
            else if(nread == 0)
                break;
            nleft -= nread;
            cptr += nread;
        }

        readsize = nbytes - nleft;
        return readsize;
    }

    int writen(int fd,char const* cptr,size_t nbytes)
    {
        int nleft,nwriten;
    
        nleft = nbytes;
        int writesize = 0;
        while(nleft > 0)
        {
            if((nwriten=write(fd,cptr,nleft)) <= 0)
            {
                if(nwriten<0 && errno!=EINTR)
                    nwriten = 0;
                else
                    return -1;
            }
            nleft -= nwriten;
            cptr += nwriten;
        }
        
        writesize  = nbytes - nleft;
        return writesize;
    }
    
    /*UNXI上出现的进程后台化函数*/
    void daemonize()
    {
        pid_t pid = fork();
        if(pid < 0)
            handleError(errno);
        else if(pid > 0)
            exit(0);

        umask(0);
        pid_t sid = setsid();
        if(sid < 0)
            handleError(errno);
        
        if(chdir("/") < 0)
            handleError(errno);

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        open("/dev/null",O_RDONLY);
        open("/dev/null",O_RDWR);
        open("/dev/null",O_RDWR);
        
    }
    
}

