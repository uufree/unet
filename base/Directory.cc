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
        Directory& Directory::operator=(Directory&& lhs)
        {
            if(*this == lhs)
                return *this;
                
            _directoryPath = std::move(lhs._directoryPath);
            _directoryList.clear();

            update(_directoryPath);
            return *this;
        }

        void Directory::update(const std::string& lhs)
        {
            _directoryList.clear();
            struct dirent* drip = NULL;
            DIR* dp = ::opendir(lhs.c_str());
            if(dp == NULL)
                unet::handleError(errno);         

            while((drip=readdir(dp)) != NULL)
            {
                if((strcmp(drip->d_name,".")==0) || (strcmp(drip->d_name,"..")==0))
                    continue;
            
                _directoryList.push_back(drip->d_name);
                
                _directoryBuffer.append(drip->d_name);
                _directoryBuffer.append("\t");
            }
        }

        void Directory::addInDirectoryList(const std::string& filename)
        {
            _directoryList.push_back(filename);
            _directoryBuffer.append(filename);
            _directoryBuffer.append("\t");
        }
    }
}


    
