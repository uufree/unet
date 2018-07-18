/*************************************************************************
	> File Name: FixedTaskPool.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年07月18日 星期三 20时55分00秒
 ************************************************************************/

#ifndef _FIXEDTASKPOOL_H
#define _FIXEDTASKPOOL_H

#include"FixedThreadPool.h"
#include"Mutex.h"
#include"Condition.h"

namespace unet
{
    class FixedTaskPool final
    {
        public:
            Fix
        private:
            const int u_size;
            FixedThreadPool u_threadPool;
    };
}

#endif
