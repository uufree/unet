/*************************************************************************
	> File Name: ts.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月13日 星期四 19时27分56秒
 ************************************************************************/

#include<vector>
#include<iostream>
using namespace std;

int main(int argc,char** argv)
{
    vector<int> vec;
    
    {
        vector<int> bec;
        bec.push_back(1);
        bec.push_back(2);
        bec.swap(vec);
        std::cout << bec.size() << std::endl;
    }

    for(auto iter=vec.begin();iter!=vec.end();++iter)
        cout << *iter << endl;

    return 0;
}
