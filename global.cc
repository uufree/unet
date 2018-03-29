/*************************************************************************
	> File Name: global.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年03月28日 星期三 02时33分57秒
 ************************************************************************/

#include"global.h"

namespace unet
{
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
                {
                    perror("readn error!\n");
                    exit(0);
                }
            }
            else if(nread == 0)
                break;
            nleft -= nread;
            cptr += nread;
        }

        readsize = nbytes - nleft;
        return readsize;
    }
        
    int readn(int fd,std::string& buf,size_t nbytes)
    {
        char cptr[nbytes];
        bzero(cptr,nbytes);
            
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
                {
                    perror("readn error!\n");
                    exit(0);
                }
            }
            else if(nread == 0)
                break;
                
            nleft -= nread;
        }
            
        readsize = nbytes - nleft;
        buf = cptr;
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
                {
                    perror("writen error!\n");
                    exit(0);
                }
            }
            nleft -= nwriten;
            cptr += nwriten;
        }
        
        writesize  = nbytes - nleft;
        return writesize;
    }
        
    int writen(int fd,const std::string& buf)
    {
        return writen(fd,buf.c_str(),buf.size());
    }
}

