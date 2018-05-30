/*************************************************************************
	> File Name: Directory.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年05月19日 星期五 16时33分18秒
 ************************************************************************/

#include"Directory.h"
#include"error.h"
#include<dirent.h>

namespace unet
{
    namespace base
    {
        Directory::Directory(const std::string& path) noexcept :
            u_directoryPath(path)
        {update(u_directoryPath);};
        
        Directory::Directory(Directory&& dir) :
            u_directoryPath(std::move(dir.u_directoryPath)),
            u_directoryList(std::move(dir.u_directoryList)),
            u_directoryBuffer(std::move(dir.u_directoryBuffer))
        {update(u_directoryPath);};

        Directory& Directory::operator=(Directory&& lhs)
        {
            if(*this == lhs)
                return *this;
                
            u_directoryPath = std::move(lhs.u_directoryPath);
            u_directoryList.clear();
            
            update(u_directoryPath);
            return *this;
        }

        void Directory::update(const std::string& lhs)
        {
            u_directoryList.clear();
            struct dirent* drip = NULL;
            DIR* dp = ::opendir(lhs.c_str());
            if(dp == NULL)
                unet::handleError(errno);         

            while((drip=readdir(dp)) != NULL)
            {
                if((strcmp(drip->d_name,".")==0) || (strcmp(drip->d_name,"..")==0))
                    continue;
            
                u_directoryList.push_back(drip->d_name);
                
                u_directoryBuffer.append(u_directoryPath);
                u_directoryBuffer.append("/");
                u_directoryBuffer.append(drip->d_name);
                u_directoryBuffer.append("\r\n");
            }
        }

        void Directory::addInDirectoryList(const std::string& filename)
        {
            u_directoryList.push_back(filename);
            u_directoryBuffer.append(filename);
            u_directoryBuffer.append("\r\n");
        }
    }
}


    
