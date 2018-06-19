/*************************************************************************
	> File Name: Directory.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年05月19日 星期五 16时32分27秒
 ************************************************************************/

#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include"File.h"
#include<list>
#include<string>

/*2018.05.28 测试完成*/

namespace unet
{
    namespace base
    {
        /*封装本地目录对象*/
        class Directory final
        {
            public:
                explicit Directory(const std::string&) noexcept;
                Directory(const Directory&) = delete;
                Directory(Directory&&); 
                Directory& operator=(const Directory& lhs) = delete;
                Directory& operator=(Directory&& lhs);
                ~Directory() noexcept {u_directoryList.clear();};
                
                bool operator==(const Directory& dir) {return u_directoryPath == dir.u_directoryPath;};
                const std::string& getDirBuffer() const {return u_directoryBuffer;};
                void addInDirectoryList(const std::string& filename);

            private:
                void update(const std::string& lhs);

            private:
                std::string u_directoryPath;
                std::list<std::string> u_directoryList;
                std::string u_directoryBuffer;
        };
    }
}

#endif
