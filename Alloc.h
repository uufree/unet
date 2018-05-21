/*************************************************************************
	> File Name: Alloc.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月21日 星期一 00时07分41秒
 ************************************************************************/

#ifndef _ALLOC_H
#define _ALLOC_H

#include<malloc.h>
#include<list>

#include"base/Mutex.h"

static const int LOG_BUF_SIZE = 4096*100-24;     
static const int USR_BUF_SIZE = 4096-24;    

#define U_USRBUFFER_FREE 0x01   //in pool
#define U_USRBUFFER_INUSE 0x02  //in use
#define U_USRBUFFER_DEAL 0x04   //已经有足够的数据，可以被处理了
#define U_USRBUFFER_WAIT 0x08   //数据还不够，稍后被处理

#define U_LOGBUFFER_FREE 0x01   //in poll   
#define U_LOGBUFFER_INUSE 0x02  //in use
#define U_LOGBUFFER_FULL 0x04   //正在被写入磁盘

#define LOG_INIT_SIZE 32
#define USR_INIT_SIZE 1024

namespace unet
{
    struct LogBuffer
    {
        LogBuffer* u_next;
        void* u_data;
        size_t u_length;
        int u_flag;
        char u_buf[LOG_BUF_SIZE];
        
        void init()
        {
            u_next = NULL;
            u_data = u_buf;
            u_length = 0;
            u_flag = 0;
            u_flag |= U_USRBUFFER_FREE;
        }
    };
    
    struct UsrBuffer
    {
        UsrBuffer* u_next;
        void* u_data;
        size_t u_length;
        int u_flag;
        char u_buf[USR_BUF_SIZE];
        
        void init()
        {
            u_next = NULL;
            u_data = u_buf;
            u_length = 0;
            u_flag = 0;
            u_flag |= U_USRBUFFER_FREE;
        }
    };

    class SimpleAllocator final
    {
        public:
            static void* allocate(size_t);
            static void deallocate(void*);
            static void* reallocate(void*,size_t);
    };

    
    /*
     * 维护16个LogBuffer和1024个UsrBuffer
     * */
    
    class Allocator final
    {
        public:
            explicit Allocator();
            Allocator(const Allocator&) = delete;
            Allocator& operator=(const Allocator&) = delete;
            Allocator(Allocator&&);
            Allocator& operator=(Allocator&&);
            ~Allocator();
            
            bool operator==(const Allocator& alloc) const {return u_logHead==alloc.u_logHead && u_usrHead==alloc.u_usrHead;};

            LogBuffer* allocLogBuffer();
            void deallocLogBuffer(LogBuffer*);
            UsrBuffer* allocUsrBuffer();
            void deallocUsrBuffer(UsrBuffer*);
            
        private:
            void init();
            void expandLogBuffer();
            void shrinkLogBuffer(){};
            void expandUsrBuffer();
            void shrinkUsrBuffer(){};

        private:
            int u_logBufferSize,u_logSize;
            int u_usrBufferSize,u_usrSize;
            LogBuffer* u_logHead,*u_logTail;    //[)
            UsrBuffer* u_usrHead,*u_usrTail;    //[)
            base::MutexLock u_logMutex;
            base::MutexLock u_usrMutex;
    };
    
    
    namespace alloc
    {
        class Allocator allocator;
        LogBuffer* allocLogBuffer(){return allocator.allocLogBuffer();};
        void deallocLogBuffer(LogBuffer* buf){allocator.deallocLogBuffer(buf);};
        UsrBuffer* allocUsrBuffer(){return allocator.allocUsrBuffer();};
        void deallocUsrBuffer(UsrBuffer* buf){allocator.deallocUsrBuffer(buf);};
    };
};


#endif

