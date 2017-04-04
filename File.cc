/*************************************************************************
	> File Name: File.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月31日 星期五 18时24分30秒
 ************************************************************************/

#include"File.h"

namespace unet
{
    File::File(const char* filename_)
    {
        char* index = strrchr(const_cast<char*>(filename_),'/');
        char buf[64];
        bzero(buf,64);
        if(index != NULL)
        {
            ++index;
            filename = index;

            int size = strlen(filename_) - strlen(index);
            strncpy(buf,filename_,size);
        }
        else
        {
            filename = filename_;
            buf[0] = '/';
        }
        
        bool isoldfile = false;
        DIR* dp;
        struct dirent* dirp;

        if((dp=opendir(buf)) != NULL)
        {
            while((dirp=readdir(dp)) != NULL)
            {
                if((strcmp(dirp->d_name,index)) == 0)
                    isoldfile = true;
            }
        }

        
        if(!isoldfile)
            fd = ::open(filename_,O_RDWR|O_APPEND|O_CLOEXEC|O_CREAT|O_EXCL|O_TRUNC);
        else
            fd = ::open(filename_,O_RDWR|O_APPEND);
        
        assert(fd >= 0);
        
        opened = true;
        closed = false;

    }
    
    void File::readn(void* buf,size_t nbytes)
    {
        int nleft,nread;
        char* cptr;
        
        readsize = 0;
        cptr = static_cast<char*>(buf);
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
    }
            
    void File::writen(void* buf,size_t nbytes)
    {
        int nleft,nwriten;
        char* cptr;
    
        cptr = static_cast<char*>(buf);
        nleft = nbytes;
        
        writesize = 0;
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
    }
    
}
