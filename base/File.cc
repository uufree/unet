/*************************************************************************
	> File Name: File.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月31日 星期五 18时24分30秒
 ************************************************************************/

#include"File.h"
#include<iostream>

namespace unet
{
    namespace base
    { 
        File::File(const char* filename,int type) noexcept :       
            u_open(false),
            u_gfilename(filename),
            u_type(type)
        {
            init();
        }

        File::File(const std::string& filename,int type) noexcept : 
            u_open(false),
            u_gfilename(filename),
            u_type(type)
        {
            init();
        }
             
        void File::init() noexcept
        {
            char* index = strrchr(const_cast<char*>(u_gfilename.c_str()),'/');
            char buf[64];
            bzero(buf,64);
            
            if(index != NULL)
            {
                ++index;
                u_filename = index;
            }
            else
            {
                perror("请输入全局地址");
                exit(1);
            }
            
            u_fd = switchOperatorType(u_type);     
            u_open = true;
            struct stat statBuf;
            if(fstat(u_fd,&statBuf) < 0)
                unet::handleError(errno);

            u_fileSize = statBuf.st_size;
        }
        
        int File::switchOperatorType(int type)
        {
            int fd = ::open(u_gfilename.c_str(),type,USER_RW | GROUP_R | OTHER_R);
             
            if(fd < 0)
                unet::handleError(errno);
            return fd;
        }
        
        File::File(File&& lhs) : 
            u_open(false),
            u_filename(std::move(lhs.u_filename)),
            u_gfilename(std::move(lhs.u_gfilename)),
            u_type(lhs.u_type),
            u_fileSize(lhs.u_fileSize)
        {
            u_fd = dup(lhs.u_fd);
            u_open = true;
            
            if(::close(lhs.u_fd) < 0)
                unet::handleError(errno);
            lhs.u_open = false;
        }
        
        File& File::operator=(File&& lhs)
        {
            if(*this == lhs)
                return *this;
            
            u_filename = std::move(lhs.u_filename);
            u_gfilename = std::move(lhs.u_gfilename);
            u_fileSize = lhs.u_fileSize;
            u_type = lhs.u_type;
            if(u_open)
                ::close(u_open);
            
            lhs.u_open ? u_fd=dup(lhs.u_open) : u_fd=switchOperatorType(u_type);  
            u_open = true;
            
            return *this;
        }
        
        int File::close()
        {
            if(u_open)
                if(::close(u_fd) < 0)
                    unet::handleError(errno);
            return 1; 
        }

        File::~File() noexcept
        {
            close();
        }
    }
}
