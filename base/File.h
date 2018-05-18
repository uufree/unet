/*************************************************************************
	> File Name: File.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月27日 星期一 18时04分42秒
 ************************************************************************/
/*设计理念：使用封装解决琐碎的问题
 * 
 * 1. O_WRITE: write
 * 2. O_C_WRITE: clear write
 * 3. O_READ: read
 * 4. N_WRITE: new write
 * 
 * 5.使用分离式构造的方式完成基本的构造
 */

#ifndef _FILE_H
#define _FILE_H

#include"global.h"
#include<sys/stat.h>

namespace unet
{
    namespace base
    {
        static const int WRITE = O_WRONLY|O_APPEND|O_EXCL;
        static const int C_WRITE = O_WRONLY|O_TRUNC|O_APPEND|O_CREAT;
        static const int READ = O_RDONLY|O_EXCL;
        static const int N_WRITE = O_RDWR|O_CREAT|O_APPEND; 

        static const mode_t USER_RW = S_IRUSR | S_IWUSR;
        static const mode_t USER_RWX = USER_RW | S_IXUSR;
        static const mode_t GROUP_R = S_IRGRP;
        static const mode_t GROUP_RW = S_IRGRP | S_IWGRP;
        static const mode_t OTHER_R = S_IROTH;
        static const mode_t OTHER_RW = S_IROTH | S_IWOTH;

        class File final
        {
            public:
                explicit File(const char* filename_,int type_) noexcept;
                explicit File(const std::string& filename_,int type_) noexcept;

                File(const File& lhs) = delete;
                File(File&& lhs);
                File& operator=(const File& lhs) = delete;
                File& operator=(File&& lhs);
                ~File() noexcept;  
                
                bool operator==(const File& lhs) {return _fd == lhs._fd;};

                const std::string& getFilename() const {return _filename;};
                const std::string& getGlobalFilename() const {return _gfilename;};
                int getFd() const {return _fd;};
                bool isOpened() const {return _open;};    
                int getFileSize() const {return _fileSize;};
                int blockRead(char* buf,size_t buflen) {return readn(_fd,buf,buflen);};
                int blockRead(std::string& buf,size_t buflen) {return readn(_fd,buf,buflen);}
                int blockWrite(const char* buf,size_t buflen) {return writen(_fd,buf,buflen);};
                int blockWrite(const std::string& buf) {return writen(_fd,buf);}
                int close();

            private:
                void init() noexcept;
                int switchOperatorType(int type_);

            private:
                int _fd;
                bool _open;
                std::string _filename;
                std::string _gfilename;
                int _type;
                unsigned long _fileSize;
        };
    }
}
            
#endif


