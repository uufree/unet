/*************************************************************************
	> File Name: EventLoop.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 00时53分33秒
 ************************************************************************/

namespace unet 
{
    namespace net
    {
        EventLoop::EventLoop() : 
            looping(false),quit(false),eventHandling(false),
            threadId(current::tid())
        {};
    
        void loop()
        {
            
        

