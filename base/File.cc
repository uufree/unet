/*************************************************************************
	> File Name: File.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月31日 星期五 18时24分30秒
 ************************************************************************/

#include"File.h"
#include<iostream>

/*非核心不见，建议使用C++标准流*/
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
            if(lhs.u_open)
                u_fd = dup(lhs.u_fd);
            else
                u_fd = switchOperatorType(u_type);
            u_open = true;
        
            if(u_open)
                lhs.close();
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
                close();

            lhs.u_open ? u_fd=dup(lhs.u_open) : u_fd=switchOperatorType(u_type);  
            u_open = true;
            
            if(lhs.u_open)
                lhs.close();

            return *this;
        }
        
        int File::close()
        {
            if(u_open)
                if(::close(u_fd) < 0)
                    unet::handleError(errno);
            u_open = false;
            return 1; 
        }

        File::~File() noexcept
        {
            close();
        }

        unsigned long File::getFileSize()
        {
            struct stat statBuf;
            if(fstat(u_fd,&statBuf) < 0)
                unet::handleError(errno);

            u_fileSize = statBuf.st_size;
            return u_fileSize;
        }
        
        /*fcntl无法对操作打开的文件的使用标志，只能使用这种方式*/
        void File::switchType(int type)
        {
            if(u_open)
            {
                close();
                u_fd = -1;
            }
            
            u_fd = switchOperatorType(type);
            u_type = type;
            u_open = true;
        }
    }
}
