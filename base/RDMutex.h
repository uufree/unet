/*************************************************************************
	> File Name: RDMutex.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月29日 星期二 05时21分07秒
 ************************************************************************/

#ifndef _RDMUTEX_H
#define _RDMUTEX_H

#include<pthread.h>
#include"Global.h"

namespace unet
{
    namespace base
    {
        /*RDLock存在三种状态，无法使用Guard进行保护*/
        class RDMutexLock final
        {
            friend bool operator==(const RDMutexLock&,const RDMutexLock&);
            
            public:
                explicit RDMutexLock();
                RDMutexLock(const RDMutexLock&& ) = delete;
                RDMutexLock& operator=(const RDMutexLock&) = delete;
                RDMutexLock(RDMutexLock&&);
                RDMutexLock& operator=(RDMutexLock&&);
                ~RDMutexLock();
                
                bool isLockInThisThread(const RDMutexLock& lock) const
                {return u_tid == lock.u_tid;};
                
                pthread_t getLockThreadId() const{return u_tid;};

                void rdLock();
                void wrLock();
                void unlock();
            private:
                pthread_t u_tid;
                pthread_rwlock_t* u_mutex;
        };
    }
}

#endif
