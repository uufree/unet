/*************************************************************************
	> File Name: test.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月26日 星期日 15时28分22秒
 ************************************************************************/

#include"File.h"
#include<iostream>
#include<string>
using namespace std;
//#include<functional>
//#include<pthread.h>
//#include<unistd.h>
//#include"Thread.h"
//#include"Thread.cc"
//#include"Mutex.h"
//#include"Condition.h"
/*
void fun(int a,int b,int c)
{
    std::cout << a << b << c << std::endl;
}
*/


int main(int argc,char** argv)
{
    string filename("/home/uuchen/uuchen.jpeg");
    
    tool::File picture(filename);

    return 0;
}
