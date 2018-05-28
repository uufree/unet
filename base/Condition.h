/*************************************************************************
	> File Name: Condition.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月28日 星期二 16时04分46秒
 ************************************************************************/

#ifndef _CONDITION_H
#define _CONDITION_H

#include"Mutex.h"

namespace unet
{
    namespace base
    { 
        /*下次找找有没有封装的比较漂亮的条件变量...这个也太简陋了吧..*/
        /*代码读的少，轮子造的都不好看*/
        class Condition final
        {
            friend bool operator==(const Condition& lhs,const Condition& rhs);
            public:
                explicit Condition(MutexLock& mutex);
            
                Condition(const Condition& lhs) = delete;
                Condition(Condition&& lhs);
                Condition& operator=(const Condition& lhs) = delete;
                Condition& operator=(Condition&& lhs);
                ~Condition();
                
                void notify();
                void notifyAll();
                void wait();
    
            private:
                MutexLock& u_mutex;
                pthread_cond_t u_cond;
        };
    }
}

#endif

