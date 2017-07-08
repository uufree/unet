/*************************************************************************
	> File Name: File.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月31日 星期五 18时24分30秒
 ************************************************************************/

#include"File.h"
#include"error.h"
#include<iostream>

namespace unet
{
    namespace file
    { 
        File::File(const char* filename_,OperatorType type_) :       opened(false),
            g_filename(filename_),
            type(type_)
        {
            init();
        }

        File::File(const std::string& filename_,OperatorType type_) : opened(false),
        g_filename(filename_),
        type(type_)
        {
            init();
        }
             
        void File::init()
        {
            char* index = strrchr(const_cast<char*>(g_filename.c_str()),'/');
            char buf[64];
            bzero(buf,64);
            if(index != NULL)
            {
                ++index;
                filename = index;
            }
            else
            {
                perror("请输入全局地址");
                exit(1);
            }
            
            fd = switchOperatorType(type);     
  
        }
        
        int File::switchOperatorType(OperatorType type_)
        {
            int fd;
            switch (type_)
            {
                case O_WRITE:
                {
                    fd = ::open(g_filename.c_str(),O_RDWR|O_APPEND);    
                    break;
                }
                case O_READ:
                {
                    fd = ::open(g_filename.c_str(),O_READ);
                    break;
                }
                case O_C_WRITE:
                {
                    fd = ::open(g_filename.c_str(),O_TRUNC|O_APPEND|O_RDWR);
                    break;
                }
                case N_WRITE:
                {
                    fd = ::open(g_filename.c_str(),O_CREAT|O_RDWR|O_APPEND);
                    break;
                }
            }

            if(fd < 0)
                unet::handleError(errno);
            opened = true;
            return fd;
        }
        
        File::File(File&& lhs) : 
            opened(false),
            filename(std::move(lhs.filename)),
            g_filename(std::move(lhs.filename)),
            type(lhs.type)
        {
            fd = switchOperatorType(type);
            opened = true;
            
            if(::close(lhs.fd) < 0)
                unet::handleError(errno);
            lhs.opened = false;
        }
        
        File& File::operator=(File&& lhs)
        {
            if(*this == lhs)
                return *this;

            if(::close(this->fd) < 0)
                unet::handleError(errno);
            this->opened = false;
            
            if(::close(lhs.fd) < 0)
                unet::handleError(errno);
            this->opened = false;

            this->filename = std::move(lhs.filename);
            this->g_filename = std::move(lhs.g_filename);
            this->fd = switchOperatorType(this->type);
            this->opened = true;

            return *this;
        }
        
        File::~File()
        {
            if(opened)
            {
                if(::close(fd) < 0)
                    unet::handleError(errno);
            }
        }
       
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
            
            std::cout << cptr << std::endl;
            std::cout << "3" << std::endl;
            readsize = nbytes - nleft;
            buf = cptr;
            return readsize;
        }
        
        void readn(const File& lhs,char* cptr,size_t nbytes)
        {
            if(lhs.isOpened())
                readn(lhs.getFd(),cptr,nbytes);
        }
        void readn(const File& lhs,std::string& buf,size_t nbytes)
        {
            if(lhs.isOpened())
                readn(lhs.getFd(),buf,nbytes);
        }
            
        int writen(int fd,const char* cptr,size_t nbytes)
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
        
        int writen(int fd,const std::string& buf,size_t nbytes)
        {
            int nleft,nwriten;
            const char* cptr = buf.c_str();

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

        void writen(const File& lhs,const char* cptr,size_t nbytes)
        {
            if(lhs.isOpened())
                writen(lhs.getFd(),cptr,nbytes);
        }
        
        void writen(const File& lhs,const std::string& cptr,size_t nbytes)
        {
            if(lhs.isOpened())
                writen(lhs.getFd(),cptr.c_str(),nbytes);
        }
        
        bool operator==(const File& lhs,const File& rhs)
        {
            if(lhs.fd == rhs.fd && lhs.g_filename == rhs.g_filename )
                return true;
            else
                return false;
        }
    }
}
