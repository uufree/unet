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
        u_logBufferSize(0),
        u_logSize(LOG_INIT_SIZE),
        u_usrBufferSize(0),
        u_usrSize(USR_INIT_SIZE),
        u_logHead(NULL),
        u_logTail(NULL),
        u_usrHead(NULL),
        u_usrTail(NULL),
        u_logMutex(),
        u_usrMutex()
    {
        init();
    }

    Allocator::Allocator(Allocator&& alloc) :
        u_logBufferSize(alloc.u_logBufferSize),
        u_logSize(alloc.u_logSize),
        u_usrBufferSize(alloc.u_usrBufferSize),
        u_usrSize(alloc.u_usrSize),
        u_logHead(alloc.u_logHead),
        u_logTail(alloc.u_logTail),
        u_usrHead(alloc.u_usrHead),
        u_usrTail(alloc.u_usrTail),
        u_logMutex(),
        u_usrMutex()
    {
        {
            base::MutexLockGuard guard(alloc.u_logMutex);
            alloc.u_logHead = alloc.u_logTail = NULL;
            alloc.u_logBufferSize = 0;
        }
        
        {
            base::MutexLockGuard guard(alloc.u_usrMutex);
            alloc.u_usrHead = alloc.u_usrTail = NULL;
            alloc.u_usrBufferSize = 0;
        }
    };
         
    
    Allocator& Allocator::operator=(Allocator&& alloc)
    {
        if(*this == alloc)
            return *this;
        {
            base::MutexLockGuard guard(u_logMutex);
            u_logBufferSize = alloc.u_logBufferSize;
            u_logSize = alloc.u_logSize;
            u_logHead = alloc.u_logHead;
            u_logTail = alloc.u_logTail;
            {
                base::MutexLockGuard guard(alloc.u_logMutex);
                alloc.u_logHead = alloc.u_logTail = NULL;
                alloc.u_logBufferSize = 0;
            }
        }
        {
            base::MutexLockGuard guard(u_usrMutex);
            u_usrBufferSize = alloc.u_usrBufferSize;
            u_usrSize = alloc.u_usrSize;
            u_usrHead = alloc.u_usrHead;
            u_usrTail = alloc.u_usrTail;
            {
                base::MutexLockGuard guard(u_usrMutex);
                alloc.u_usrHead = alloc.u_usrTail = NULL;
                alloc.u_usrBufferSize = 0;
            }
        }
        return *this;
    }

    Allocator::~Allocator()
    {
        LogBuffer* logTemp = NULL;
        UsrBuffer* usrTemp = NULL;
        while(u_logHead)
        {
            logTemp = u_logHead;
            u_logHead = u_logHead->u_next;
            delete logTemp;
        }

        while(u_usrHead)
        {
            usrTemp = u_usrHead;
            u_usrHead = u_usrHead->u_next;
            delete usrTemp;
        }
    }
    
    void Allocator::init()
    {
        LogBuffer* head = (LogBuffer*)SimpleAllocator::allocate(sizeof(LogBuffer));
        head->init();
        u_logHead = head;
        head = head->u_next;
        for(int i=0;i<u_logSize-1;++i)
        {
            head = (LogBuffer*)SimpleAllocator::allocate(sizeof(LogBuffer));
            head->init();
            head = u_logHead->u_next;
        }
        u_logTail = head;
        u_logBufferSize = u_logSize;
       
        UsrBuffer* haed = (UsrBuffer*)SimpleAllocator::allocate(sizeof(UsrBuffer));
        haed->init();
        u_usrHead = haed;
        haed = haed->u_next;
        for(int i=0;i<u_usrSize-1;++i)
        {
            haed = (UsrBuffer*)SimpleAllocator::allocate(sizeof(UsrBuffer));
            haed->init();
            haed = haed->u_next;
        }
        u_usrTail = haed;
        u_usrBufferSize = u_usrSize;
    }
    
    LogBuffer* Allocator::allocLogBuffer()
    {
        if(!u_logBufferSize)
            expandLogBuffer();

        LogBuffer* res = u_logHead;
        
        {
            base::MutexLockGuard guard(u_logMutex);
            u_logHead = u_logHead->u_next;
            --u_logBufferSize;

            if(u_logBufferSize == 0)
                u_logHead = u_logTail = NULL;
        }
        return res;
    }

    void Allocator::deallocLogBuffer(LogBuffer* buf)
    {
        base::MutexLockGuard guard(u_logMutex);
        u_logTail = buf;
        if(!u_logBufferSize)    //NULL，需要重新调整指针位置
            u_logHead = buf;
        ++u_logBufferSize;
    }
    
    UsrBuffer* Allocator::allocUsrBuffer()
    {
        if(!u_usrBufferSize)
            expandUsrBuffer();

        UsrBuffer* res = u_usrHead;

        {
            base::MutexLockGuard guard(u_usrMutex);
            u_usrHead = u_usrHead->u_next;
            --u_usrBufferSize;

            if(u_usrBufferSize == 0)
                u_usrHead = u_usrTail = NULL;
        }
        return res;
    }
    
    void Allocator::deallocUsrBuffer(UsrBuffer* buf)
    {
        base::MutexLockGuard guard(u_usrMutex);
        u_usrTail = buf;
        if(!u_usrBufferSize)
            u_usrHead = buf;
        ++u_usrBufferSize;
    }

    void Allocator::expandLogBuffer()
    {
        LogBuffer* head = (LogBuffer*)SimpleAllocator::allocate(sizeof(LogBuffer));
        head->init();
        LogBuffer* heads = head;
        head = head->u_next;
        for(int i=0;i<u_logSize-1;++i)
        {
            head = (LogBuffer*)SimpleAllocator::allocate(sizeof(LogBuffer));
            head->init();
            head = head->u_next;
        }
        
        {
            base::MutexLockGuard guard(u_logMutex);
            u_logHead = heads;
            u_logTail = head;
            u_logBufferSize = u_logSize;
            u_logSize += u_logSize;
        }
    }
    
    void Allocator::expandUsrBuffer()
    {
        UsrBuffer* head = (UsrBuffer*)SimpleAllocator::allocate(sizeof(UsrBuffer));
        head->init();
        UsrBuffer* heads = head;
        head = head->u_next;
        for(int i=0;i<u_usrSize-1;++i)
        {
            head = (UsrBuffer*)SimpleAllocator::allocate(sizeof(UsrBuffer));
            head->init();
            head = head->u_next;
        }

        {
            base::MutexLockGuard guard(u_usrMutex);
            u_usrHead = heads;
            u_usrTail = head;
            u_usrBufferSize = u_usrSize;
            u_usrSize += u_usrSize;
        }
    }
}
