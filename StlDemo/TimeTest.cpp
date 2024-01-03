#include "TimeTest.h"
#include <iostream>
#include <chrono>
#include <ctime>

////////////////////////////////////////////////////////////

static std::string CovertTimeStamp00(long long timeStamp, bool needMilliSecond)
{
    time_t sec = timeStamp / 1000;
    struct tm tp{0};

    if (localtime_r(&sec, &tp) == nullptr) {
        return "";
    }
    char buf[50]{0};
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tp);
    std::string result(buf);
    if (needMilliSecond) {
        long long ms = timeStamp % 1000;
        result += "." + std::to_string(ms);
    }

    return result;
}

void TimeTest00()
{
    auto ns = std::chrono::system_clock().now().time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ns);
    // auto sec = std::chrono::duration_cast<std::chrono::seconds>(ns);
    // time_t gmtTime = time(nullptr); == sec
    long long gmtTimeStamp = static_cast<long long>(ms.count());
    std::cout << "gmt time stamp=" << gmtTimeStamp << std::endl;
    std::cout << "local time stamp=" << CovertTimeStamp00(gmtTimeStamp, true) << std::endl;
    std::cout << "local time stamp=" << CovertTimeStamp00(gmtTimeStamp, false) << std::endl;
}

////////////////////////////////////////////////////////////
