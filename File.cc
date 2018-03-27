/*************************************************************************
	> File Name: File.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月31日 星期五 18时24分30秒
 ************************************************************************/

#include"File.h"
#include<sys/stat.h>

namespace unet
{
    namespace base
    { 
        File::File(const char* filename,int type) noexcept :       
            _open(false),
            _gfilename(filename),
            _type(type)
        {init();}

        File::File(const std::string& filename,int type) noexcept : 
            _open(false),
            _gfilename(filename),
            _type(type)
        {init();}
             
        void File::init() noexcept
        {
            char* index = strrchr(const_cast<char*>(_gfilename.c_str()),'/');
            char buf[64];
            bzero(buf,64);
            
            if(index != NULL)
            {
                ++index;
                _filename = index;
            }
            else
            {
                perror("请输入全局地址");
                exit(1);
            }
            
            _fd = switchOperatorType(_type);     

            struct stat statBuf;
            if(fstat(_fd,&statBuf) < 0)
                unet::handleError(errno);

            _fileSize = statBuf.st_size;
        }
        
        int File::switchOperatorType(int type)
        {
            int fd = ::open(_gfilename.c_str(),type);
            
            if(fd < 0)
                unet::handleError(errno);
            return fd;
        }
        
        File::File(File&& lhs) : 
            _open(false),
            _filename(std::move(lhs._filename)),
            _gfilename(std::move(lhs._gfilename)),
            _type(lhs._type),
            _fileSize(lhs._fileSize)
        {
            _fd = dup(lhs._fd);
            _open = true;
            
            if(::close(lhs._fd) < 0)
                unet::handleError(errno);
            lhs._open = false;
        }
        
        File& File::operator=(File&& lhs)
        {
            if(*this == lhs)
                return *this;
            
            _filename = std::move(lhs._filename);
            _gfilename = std::move(lhs._gfilename);
            _fileSize = lhs._fileSize;
            _type = lhs._type;
            if(_open)
                ::close(_open);
            
            lhs._open ? _fd=dup(lhs._open) : _fd=switchOperatorType(_type);  
            _open = true;
            
            return *this;
        }
        
        File::~File() noexcept
        {
            if(_open)
                if(::close(_fd) < 0)
                    unet::handleError(errno);
        }
    }
}
