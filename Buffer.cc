/*************************************************************************
	> File Name: Buffer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月14日 星期二 19时28分46秒
 ************************************************************************/

namespace unet
{
    namespace net
    {
        void Buffer::readInSocket(int fd)
        {
            std::iterator<Buffer> iter = buffer.end();
            int n = ::read(fd,&*iter,level);
            index += n;
        }

        void Buffer::writeInSocket(int fd)
        {
            std::iterator<Buffer> iter = buffer.begin();
            void* buf = memcpy()
            int n = ::write(fd,&*iter,index);
            index = buffer.size();
        }

        void Buffer::appendInBuffer(const void* message)
        {
            buffer.push_back(*(static_cast<const char*>(message)),)
