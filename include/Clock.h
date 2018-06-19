/*************************************************************************
	> File Name: Clock.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月03日 星期六 12时40分22秒
 ************************************************************************/

#ifndef _CLOCK_H
#define _CLOCK_H

#include<string>
#include<vector>

/*对系统中的日期与时间做简单的封装*/
/*2018.05.28 测试完成*/

namespace unet
{
    namespace base
    {
        class Clock final
        {
            friend bool operator==(const Clock& lhs,const Clock& rhs);
        
            public:
                explicit Clock(){update();};
                Clock(const Clock& lhs) = delete;
                Clock(Clock&& lhs);
                Clock& operator=(const Clock& lhs) = delete;
                Clock& operator=(Clock&& lhs);
                ~Clock(){};
            
                const std::string& getHourAndMinutes() const
                {return u_hourAndMinutes;};
                const std::string& getMouthAndDays()const{return u_mouthAndDays;};
                const std::string& getMouth() const {return u_mouth;};
                const std::string& getWeak() const {return u_weak;};
                const std::string& getDay() const {return u_day;};
                const std::string& getTime() const{return u_time;};
                const std::string& getYear() const{return u_year;};
                void update();
        
            private:
                void split(const std::string& lhs,const std::string& rhs,std::vector<std::string>& vecStr);

            private:
                std::string u_mouth;
                std::string u_weak;
                std::string u_day;
                std::string u_time;
                std::string u_year;

                std::string u_hourAndMinutes;
                std::string u_mouthAndDays;
                std::vector<std::string> u_strList;
        };
    }

}

#endif
