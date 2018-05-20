/*************************************************************************
	> File Name: AllocTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 16时43分49秒
 ************************************************************************/

#include<iostream>

struct Buffer
{
    Buffer* next;
    void* data;
    size_t length;
};

struct LogBuffer : public Buffer
{
    char buf[409576];
};

int main(int argc,char** argv)
{
    std::cout << sizeof(struct LogBuffer) << std::endl;
    return 0;
}
