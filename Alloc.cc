/*************************************************************************
	> File Name: Alloc.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月21日 星期一 00时08分11秒
 ************************************************************************/

#include"Alloc.h"

namespace unet
{
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

    Allocator::~Allocator()
    {
        LogBuffer* logTemp = NULL;
        UsrBuffer* usrTemp = NULL;
        while(!u_logBufferList.empty())
        {
            logTemp = u_logBufferList.front();
            u_logBufferList.pop_front();
            delete logTemp;
        }

        while(!u_usrBufferList.empty())
        {
            usrTemp = u_usrBufferList.front();
            u_usrBufferList.pop_front();
            delete usrTemp;
        }
    }
    
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
        if(!u_logBufferList.empty())
            expandLogBuffer();
        
        LogBuffer* res = NULL;
        
        {
            base::MutexLockGuard guard(u_logMutex);
            res = u_logBufferList.front();
            u_logBufferList.pop_front();
        }
        
        BUFFER_CLEAR_FLAG(res);
        BUFFER_SET_INUSE(res);
        BUFFER_SET_LENGTH(res,LOG_BUF_SIZE);
        BUFFER_CLEAR_DATA(res);

        return res;
    }

    void Allocator::deallocLogBuffer(LogBuffer* buf)
    {
        base::MutexLockGuard guard(u_logMutex);
        u_logBufferList.push_back(buf);
    }
    
    UsrBuffer* Allocator::allocUsrBuffer()
    {
        if(!u_usrBufferList.empty())
            expandUsrBuffer();

        UsrBuffer* res = NULL;

        {
            base::MutexLockGuard guard(u_usrMutex);
            u_usrBufferList.front();
            u_usrBufferList.pop_front();
        }
        
        BUFFER_CLEAR_FLAG(res);
        BUFFER_SET_INUSE(res);
        BUFFER_SET_LENGTH(res,LOG_BUF_SIZE);
        BUFFER_CLEAR_DATA(res);
        
        return res;
    }
    
    void Allocator::deallocUsrBuffer(UsrBuffer* buf)
    {
        base::MutexLockGuard guard(u_usrMutex);
        u_usrBufferList.push_back(buf);
    }

    void Allocator::expandLogBuffer()
    {
        std::list<LogBuffer*> logBufferList;
        LogBuffer* logTemp = NULL;
        for(int i=0;i<u_logListSize;++i)
        {
            logTemp = (LogBuffer*)SimpleAllocator::allocate(sizeof(LogBuffer));
            logBufferList.push_back(logTemp);
        }
        
        {
            base::MutexLockGuard guard(u_logMutex);
            u_logBufferList.splice(u_logBufferList.end(),logBufferList,logBufferList.begin(),logBufferList.end());
            u_logListSize += u_logListSize;
        }
    }
    
    void Allocator::expandUsrBuffer()
    {
        std::list<UsrBuffer*> usrBufferList;
        UsrBuffer* usrTemp = NULL;

        for(int i=0;i<u_usrListSize;++i)
        {
            usrTemp = (UsrBuffer*)SimpleAllocator::allocate(sizeof(UsrBuffer));
            u_usrBufferList.push_back(usrTemp);
        }
        
        {
            base::MutexLockGuard guard(u_usrMutex);
            u_usrBufferList.splice(u_usrBufferList.end(),usrBufferList,usrBufferList.begin(),usrBufferList.end());
            u_usrListSize += u_usrListSize;
        }
    }
}
