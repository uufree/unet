/*************************************************************************
	> File Name: Alloc.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月21日 星期一 00时08分11秒
 ************************************************************************/

#include"../include/Alloc.h"

namespace unet
{
    namespace base
    {/*base start*/

    /*一级内存分配器*/
    void* SimpleAllocator::allocate(size_t bytes)
    {
        void* result = ::malloc(bytes);
        if(result == NULL)
            result = ::malloc(bytes);
        return result;
    }

    void SimpleAllocator::deallocate(void* ptr)
    {
        if(ptr)
            ::free(ptr);
    }

    void* SimpleAllocator::reallocate(void* ptr,size_t bytes)
    {
        if(ptr == NULL)
            return NULL;
        void* newPtr = ::realloc(ptr,bytes);
        if(newPtr == NULL)
            newPtr = ::realloc(ptr,bytes);
        return newPtr;
    }

    /*二级内存分配器*/
    Allocator::Allocator() : 
        u_logListSize(LOG_INIT_SIZE),
        u_usrListSize(USR_INIT_SIZE),
        u_logBufferList(),
        u_usrBufferList(),
        u_logMutex(),
        u_usrMutex()
    {
        init();
    }
    
    /*不知道这样用锁是否正确，与std::move是否正确*/
    Allocator::Allocator(Allocator&& alloc) :
        u_logMutex(),
        u_usrMutex()
    {
        {
            base::MutexLockGuard guard(alloc.u_logMutex);
            std::swap(u_logBufferList,alloc.u_logBufferList);
            std::swap(u_logListSize,alloc.u_logListSize);
        }
        
        {
            base::MutexLockGuard guard(alloc.u_usrMutex);
            std::swap(u_usrBufferList,alloc.u_usrBufferList);
            std::swap(u_usrListSize,alloc.u_usrListSize);
        }
    };
         
    
    Allocator& Allocator::operator=(Allocator&& alloc)
    {
        if(*this == alloc)
            return *this;
        {
            base::MutexLockGuard guard(u_logMutex);
            {
                base::MutexLockGuard guard(alloc.u_logMutex);
                std::swap(u_logBufferList,alloc.u_logBufferList);
                std::swap(u_logListSize,alloc.u_logListSize);
            }
        }

        {
            base::MutexLockGuard guard(u_usrMutex);
            {
                base::MutexLockGuard guard(u_usrMutex);
                std::swap(u_usrBufferList,alloc.u_usrBufferList);
                std::swap(u_usrListSize,alloc.u_usrListSize);
            }
        }
        return *this;
    }
    
    /*回收内存时，使用Alloc::中的函数进行缓存回收*/
    /*析构期间不敢用锁保护状态*/
    Allocator::~Allocator()
    {
        LogBuffer* logTemp = NULL;
        UsrBuffer* usrTemp = NULL;
        while(!u_logBufferList.empty())
        {
            logTemp = u_logBufferList.front();
            u_logBufferList.pop_front();
            SimpleAllocator::deallocate(logTemp);
        }

        while(!u_usrBufferList.empty())
        {
            usrTemp = u_usrBufferList.front();
            u_usrBufferList.pop_front();
            SimpleAllocator::deallocate(usrTemp);
        }
    }
    
    /*初始化期间同样，不能使用锁保护状态*/
    void Allocator::init()
    {
        LogBuffer* logTemp = NULL;
        UsrBuffer* usrTemp = NULL;
        for(int i=0;i<u_logListSize;++i)
        {
            logTemp = (LogBuffer*)SimpleAllocator::allocate(sizeof(LogBuffer));
            u_logBufferList.push_back(logTemp);
        }
        
        for(int i=0;i<u_usrListSize;++i)
        {
            usrTemp = (UsrBuffer*)SimpleAllocator::allocate(sizeof(UsrBuffer));
            u_usrBufferList.push_back(usrTemp);
        }
    }
    
    LogBuffer* Allocator::allocLogBuffer()
    {
        /*not-thread safety*/
        if(u_logBufferList.empty())
            expandLogBuffer();
        
        LogBuffer* res = NULL;
        
        /*正常的缓存分配*/
        {
            base::MutexLockGuard guard(u_logMutex);
            res = u_logBufferList.front();
            u_logBufferList.pop_front();
            --u_logListSize;
        }
        
        /*在分配内存时设置标志位*/
        BUFFER_CLEAR_FLAG(res);
        BUFFER_SET_INUSE(res);
        BUFFER_SET_FREE(res);
        BUFFER_SET_LENGTH(res,LOG_BUF_SIZE);
        BUFFER_CLEAR_DATA(res);

        return res;
    }
    
    /*设置标志，回收缓存*/
    void Allocator::deallocLogBuffer(LogBuffer* buf)
    {
        BUFFER_DEL_INUSE(buf);
        {
            base::MutexLockGuard guard(u_logMutex);
            u_logBufferList.push_back(buf);
            ++u_logListSize;
        }
    }
    
    /*与分配LogBuffer的过程类似，不在赘述*/
    UsrBuffer* Allocator::allocUsrBuffer()
    {
        if(u_usrBufferList.empty())
            expandUsrBuffer();

        UsrBuffer* res = NULL;

        {
            base::MutexLockGuard guard(u_usrMutex);
            res = u_usrBufferList.front();
            u_usrBufferList.pop_front();
            --u_usrListSize;
        }
        
        BUFFER_CLEAR_FLAG(res);
        BUFFER_SET_INUSE(res);
        BUFFER_SET_FREE(res);
        BUFFER_SET_LENGTH(res,USR_BUF_SIZE);
        BUFFER_CLEAR_DATA(res);

        return res;
    }
    
    void Allocator::deallocUsrBuffer(UsrBuffer* buf)
    {
        BUFFER_DEL_INUSE(buf);
        base::MutexLockGuard guard(u_usrMutex);
        u_usrBufferList.push_back(buf);
        ++u_usrListSize;
    }
    
    /*将维护的缓冲区扩再次扩充一个INIT的大小，×2式的扩充有点浪费*/
    void Allocator::expandLogBuffer()
    {
        std::list<LogBuffer*> logBufferList;
        LogBuffer* logTemp = NULL;
        for(int i=0;i<LOG_INIT_SIZE;++i)
        {
            logTemp = (LogBuffer*)SimpleAllocator::allocate(sizeof(LogBuffer));
            logBufferList.push_back(logTemp);
        }
        
        {
            base::MutexLockGuard guard(u_logMutex);
            u_logBufferList.splice(u_logBufferList.end(),logBufferList,logBufferList.begin(),logBufferList.end());
            u_logListSize += LOG_INIT_SIZE;
        }
    }
    
    void Allocator::expandUsrBuffer()
    {
        std::list<UsrBuffer*> usrBufferList;
        UsrBuffer* usrTemp = NULL;

        for(int i=0;i<USR_INIT_SIZE;++i)
        {
            usrTemp = (UsrBuffer*)SimpleAllocator::allocate(sizeof(UsrBuffer));
            u_usrBufferList.push_back(usrTemp);
        }
        
        {
            base::MutexLockGuard guard(u_usrMutex);
            u_usrBufferList.splice(u_usrBufferList.end(),usrBufferList,usrBufferList.begin(),usrBufferList.end());
            u_usrListSize += USR_INIT_SIZE;
        }
    }
    
    }/*base end*/

    /*allo namespace*/ 
    namespace alloc
    {
        base::LogBuffer* allocLogBuffer(){return allocator.allocLogBuffer();};
        void deallocLogBuffer(base::LogBuffer* buf){allocator.deallocLogBuffer(buf);};
        base::UsrBuffer* allocUsrBuffer(){return allocator.allocUsrBuffer();};
        void deallocUsrBuffer(base::UsrBuffer* buf){allocator.deallocUsrBuffer(buf);};
        int getLogBufferSize(){return allocator.getLogBufferSize();};
        int getUsrBufferSize(){return allocator.getUsrBufferSize();};
    }
}

namespace unet
{
    namespace base
    {
        /*写Log Buffer的步骤：
         * 1.测试flag是否正常
         * 2.将数据copy进去
         * 3.调整buffer中的指针与剩余空间
         * 4.调整标志*/
        int writeInLogBuffer(LogBuffer* buffer,const void* str,int length)
        {
            if(str == NULL || length <=0 || length >= 200)
                return -1;
            if(!BUFFER_TEST_INUSE(buffer))
                return -1;
            if(BUFFER_TEST_FULL(buffer))
                return -1;
            if(!LOG_BUFFER_HAS_ENOUTH_SPACE(buffer))
            {
                BUFFER_SET_FULL(buffer);
                return -1;
            }

            memcpy(buffer->u_data,str,length);
            buffer->u_data = buffer->u_data + length;
            buffer->u_length -= length;
            BUFFER_SET_DIRTY(buffer);
            return 0;
        }
        
        /*使用阻塞写，将Buffer中的数据写入File*/
        int writeLogBufferInFile(LogBuffer* buffer,int fd)
        {
            if(buffer == NULL || fd < 0)
                return 2;
            if(!BUFFER_TEST_INUSE(buffer))
                return 1;
            if(!BUFFER_TEST_DIRTY(buffer))
                return 0;
            
            int length = writen(fd,buffer->u_buf,(buffer->u_data-buffer->u_buf));
            if(length == -1 && length == 0)
                return -1;
            return 0;
        }
        
    }
}

