/*************************************************************************
	> File Name: Clock.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月03日 星期六 12时40分28秒
 ************************************************************************/

#include"Clock.h"
#include<time.h>

namespace unet
{
    namespace base
    {
    /*将string字符串中的数据根据需求进行分割*/
    void Clock::split(const std::string& lhs,const std::string& rhs,std::vector<std::string>& vecStr)
    {
        vecStr.clear();
        size_t last = 0;
        size_t index = lhs.find_first_of(rhs,last);
        while(index != std::string::npos)
        {
            vecStr.push_back(lhs.substr(last,index-last));
            last = index + 1;
            index = lhs.find_first_of(rhs,last);
        }

        if((index-last) > 0)
            vecStr.push_back(lhs.substr(last,index-last));
    }
    
    Clock::Clock(Clock&& lhs) :
        u_mouth(std::move(lhs.u_mouth)),
        u_weak(std::move(lhs.u_weak)),
        u_day(std::move(lhs.u_day)),
        u_time(std::move(lhs.u_time)),
        u_year(std::move(lhs.u_year)),
        u_hourAndMinutes(std::move(lhs.u_hourAndMinutes)),
        u_mouthAndDays(std::move(lhs.u_mouthAndDays))
    {};

    Clock& Clock::operator=(Clock&& lhs)
    {
        if(*this == lhs)
            return *this;

        u_mouth = std::move(lhs.u_mouth);
        u_weak = std::move(lhs.u_weak);
        u_day = std::move(lhs.u_day);
        u_time = std::move(lhs.u_time);
        u_year = std::move(lhs.u_year);
        u_hourAndMinutes = std::move(lhs.u_hourAndMinutes);
        u_mouthAndDays = std::move(lhs.u_mouthAndDays);

        return *this;
    }
        
    /*更新系统中的时间*/
    void Clock::update()
    {
        time_t time_ = time(NULL);
        std::string strTime = ctime(&time_);
        
        split(strTime," ",u_strList);
        u_weak = u_strList[0];
        u_mouth = u_strList[1];
        u_day = u_strList[2];
        u_time = u_strList[3];
        u_year = u_strList[4];

        u_hourAndMinutes = u_strList[3];
        u_mouthAndDays = u_strList[0] + "," + u_strList[1] + " " + u_strList[2] + "th";
    };
    
    bool operator==(const Clock& lhs,const Clock& rhs)
    {
        if(lhs.u_hourAndMinutes == rhs.u_hourAndMinutes && 
                lhs.u_mouthAndDays == rhs.u_mouthAndDays)
            return true;
        return false;
    }
    
    }
}

