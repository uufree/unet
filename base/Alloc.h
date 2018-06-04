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
#include<cstring>

#include"Global.h"
#include"Mutex.h"


#define LOG_INIT_SIZE 32
#define USR_INIT_SIZE 1024

/*2018.05.30 测试完成*/
/*在单线程，多线程下均工作良好，无内存泄露*/

static const int LOG_BUF_SIZE = 4096*100-24;     
static const int USR_BUF_SIZE = 4096-24;    

#define U_BUFFER_FREE 0x01   //in pool   
#define U_BUFFER_INUSE 0x02  //in use
#define U_BUFFER_FULL 0x04   //is full
#define U_BUFFER_DIRTY 0x08  //is dirty but not full

/*2018.05.30 测试完成*/

namespace unet
{
    namespace base
    {
        struct UsrBuffer
        {
            char* u_data;
            size_t u_length;
            int u_flag;
            char u_buf[USR_BUF_SIZE];
        };
        
        struct LogBuffer
        {
            char* u_data;
            size_t u_length;    //free length
            int u_flag;
            char u_buf[LOG_BUF_SIZE];
        };
         
        /*状态调整使用宏，Buffer操作使用模板函数*/
        /*需要一系列的宏或者模板函数对LogBuffer和UsrBuffer进行操作
        * 状态分析(宏)：
        *  1.分配Buffer时，设置Buffer为InUse（通用）
        *  2.归还Buffer时，设置Buffer为Free（通用）
        *  3.写入数据时，设置Buffer为Dirty（通用）
        *  4.写入数据时，测试Buffer是否已经为Full（Log+Usr）
        *  5.清理数据时，测试Buffer是否为Dirty（通用）
        */

#define BUFFER_CLEAR_FLAG(buf) (buf->u_flag=0)

#define BUFFER_SET_INUSE(buf) (buf->u_flag|=U_BUFFER_INUSE)
#define BUFFER_TEST_INUSE(buf) (buf->u_flag & U_BUFFER_INUSE)
#define BUFFER_DEL_INUSE(buf) (buf->u_flag &= ~U_BUFFER_INUSE)        

#define BUFFER_SET_FREE(buf) (buf->u_flag|=U_BUFFER_FREE)
#define BUFFER_TEST_FREE(buf) (buf->u_flag & U_BUFFER_FREE)
#define BUFFER_DEL_FREE(buf) (buf->u_flag &= ~U_BUFFER_FREE)

#define BUFFER_SET_DIRTY(buf) (buf->u_flag|=U_BUFFER_DIRTY)
#define BUFFER_TEST_DIRTY(buf) (buf->u_flag & U_BUFFER_DIRTY)
#define BUFFER_DEL_DIRTY(buf) (buf->u_flag &= ~U_BUFFER_DIRTY)

#define BUFFER_SET_FULL(buf) (buf->u_flag |= U_BUFFER_FULL)
#define BUFFER_TEST_FULL(buf) (buf->u_flag & U_BUFFER_FULL)
#define BUFFER_DEL_FULL(buf) (buf->u_flag &= ~U_BUFFER_FULL)

#define BUFFER_LENGTH(buf) (buf->u_length)
#define BUFFER_SET_LENGTH(buf,len) (buf->u_length=len)
#define BUFFER_CLEAR_DATA(buf) (buf->u_data=buf->u_buf)

#define LOG_BUFFER_HAS_ENOUTH_SPACE(buf) (buf->u_length > 200)
    
        /*需要一系列的模板函数对Buffer进行读写操作：
        * Log操作分析：
        *  1.向Buffer中写入数据（通用）
        *  2.将Buffers的数据写入fd（Log）
        */
    
        /* Functioality:
        *      向Buffer中写数据，根据Buffer的状态返回不同的值，供外部的参考
        * Parameters:
        *      0）[in] LogBuffer：数据即将写入的LogBuffer
        *      1）[in] str：日志消息
        *      2）[in] length：日志消息长度
        * Returned Value:
        *      0：True
        *      -1：memcpy错误，系统错误
        */ 
        int writeInLogBuffer(LogBuffer* buffer,const void* str,int length);
    
        /* Functionality:
        *      将Full/Dirty LogBuffer中的数据写入File
        * Parameters：
        *      0）[in] LogBuffer：待写入File的LogBuffer
        *      1）[in] fd：日志文件描述符
        * Returned Value：
        *      0：True
        *      1：Buffer状态不正确，当前没有设置INUSE
        *      2：参数不正确
        *      -1：write错误
        */
        int writeLogBufferInFile(LogBuffer* buffer,int fd);
    }
}


namespace unet
{
    namespace base
    {/*base start*/

    class SimpleAllocator final
    {
        public:
            static void* allocate(size_t);
            static void deallocate(void*);
            static void* reallocate(void*,size_t);
    };

    /*
     * 初始时维护16个LogBuffer和1024个UsrBuffer
     * 这个类会在全局出现一次，使用的时候会被多线程竞争，分析：
     * 初始化：在全局状态下初始化，无竞争
     * 使用：由锁保护内部状态，有竞争 有保护
     * 析构：锁的销毁时机在靠后（在工作线程的后面），有竞争无保护。但是如果保证，
     * 销毁时已经终止所有的工作线程，并且工作线程返回所有的使用内存，便不会发生内
     * 存泄露
     */
    
    class Allocator final
    {
        public:
            explicit Allocator();
            Allocator(const Allocator&) = delete;
            Allocator& operator=(const Allocator&) = delete;
            Allocator(Allocator&&);
            Allocator& operator=(Allocator&&);
            ~Allocator();
            
            bool operator==(const Allocator& alloc) const {return u_logBufferList.front() == alloc.u_logBufferList.front() && u_usrBufferList.front() == alloc.u_usrBufferList.front();};

            LogBuffer* allocLogBuffer();
            void deallocLogBuffer(LogBuffer*);
            UsrBuffer* allocUsrBuffer();
            void deallocUsrBuffer(UsrBuffer*);
            int getUsrBufferSize() const{return u_usrListSize;};
            int getLogBufferSize() const{return u_logListSize;};

        private:
            void init();
            void expandLogBuffer();
            void shrinkLogBuffer(){};
            void expandUsrBuffer();
            void shrinkUsrBuffer(){};

        private:
            int u_logListSize;
            int u_usrListSize;
            std::list<LogBuffer*> u_logBufferList;
            std::list<UsrBuffer*> u_usrBufferList;
            base::MutexLock u_logMutex;
            base::MutexLock u_usrMutex;
    };

    }/*base end*/

    /*allo namespace*/ 
    namespace alloc
    {
        static class base::Allocator allocator;
        base::LogBuffer* allocLogBuffer();
        void deallocLogBuffer(base::LogBuffer* buf);
        base::UsrBuffer* allocUsrBuffer();
        void deallocUsrBuffer(base::UsrBuffer* buf);
        int getLogBufferSize();
        int getUsrBufferSize();
    }
}


#endif

