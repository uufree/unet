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

#include"Global.h"
#include<sys/stat.h>

/*2018.05.28 测试完成*/

namespace unet
{
    namespace base
    {
        /*只保存两种类型，需要的时候对状态进行转换即可*/
        static const int WRITE = O_RDWR|O_APPEND|O_CREAT;
        static const int READ = O_RDWR;
//        static const int CLEAR_WRITE = O_RDWR|O_TRUNC|O_APPEND|O_CREAT;
//        static const int NEW_WRITE = O_RDWR|O_CREAT|O_APPEND; 

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
                
                bool operator==(const File& lhs) {return u_fd == lhs.u_fd;};

                const std::string& getFilename() const {return u_filename;};
                const std::string& getGlobalFilename() const {return u_gfilename;};
                int getFd() const {return u_fd;};
                bool isOpened() const {return u_open;};    
                unsigned long getFileSize();
                int blockRead(char* buf,size_t buflen) {return readn(u_fd,buf,buflen);};
                int blockWrite(const char* buf,size_t buflen) {return writen(u_fd,buf,buflen);};
                int close();
                void switchType(int);

            private:
                void init() noexcept;
                int switchOperatorType(int);

            private:
                int u_fd;
                bool u_open;
                std::string u_filename;
                std::string u_gfilename;
                int u_type;
                unsigned long u_fileSize;
        };
    }
}
            
#endif



