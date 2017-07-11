/*************************************************************************
	> File Name: Directory.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年05月19日 星期五 16时32分27秒
 ************************************************************************/

#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include"File.h"
#include<vector>

namespace unet
{
    namespace file
    {
        class Directory final
        {
            friend bool operator==(const Directory& lhs,const Directory& rhs);

            public:
                explicit Directory(const char* path);
                explicit Directory(const std::string& lhs);

                Directory(const Directory& lhs);
                Directory(Directory&& lhs);
                Directory& operator=(const Directory& lhs);
                Directory& operator=(Directory&& lhs);
                ~Directory();
            
                const std::string& getDirBuffer() const;
                void addInDirectoryList(const std::string& filename);

            private:
                void update(const std::string& lhs);

            private:
                std::string directorypath;
                std::vector<std::string> directorylist;
                std::string directoryBuffer;
        };
        
        bool operator==(const Directory& lhs,const Directory& rhs);
    }
}

#endif
