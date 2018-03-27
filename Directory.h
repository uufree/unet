/*************************************************************************
	> File Name: Directory.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年05月19日 星期五 16时32分27秒
 ************************************************************************/

/*设计理念：封装本地的目录对象
 * 
 * 1.二重构造来设计整个对象的构造过程
 * 2.使用STL保证数据安全性
 */

#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include"File.h"
#include<list>

namespace unet
{
    namespace base
    {
        class Directory final
        {
            public:
                explicit Directory(const char* path) noexcept :
                    _directoryPath(path)
                {update(_directoryPath);};

                explicit Directory(const std::string& path) noexcept :
                    _directoryPath(path)
                {update(_directoryPath);};
            
                Directory(const Directory& lhs) = delete;
                Directory(Directory&& lhs) : 
                    _directoryPath(std::move(lhs._directoryPath))
                {update(_directoryPath);};

                Directory& operator=(const Directory& lhs) = delete;
                Directory& operator=(Directory&& lhs);
                ~Directory() noexcept {_directoryList.clear();};
                
                bool operator==(const Directory& dir) {return _directoryPath == dir._directoryPath;};
                const std::string& getDirBuffer() const {return _directoryBuffer;};
                void addInDirectoryList(const std::string& filename);

            private:
                void update(const std::string& lhs);

            private:
                std::string _directoryPath;
                std::list<std::string> _directoryList;
                std::string _directoryBuffer;
        };
    }
}

#endif
