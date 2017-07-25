/*************************************************************************
	> File Name: deque.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月24日 星期一 10时57分05秒
 ************************************************************************/

#include<iostream>
#include<deque>

int main(int argc,char** argv)
{
    std::deque<int> deq;
    for(int i=0;i<10;++i)
        deq.push_back(i);

    int i = deq.front();
}
