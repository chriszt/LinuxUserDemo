#ifndef LOGIN_DEF_H
#define LOGIN_DEF_H

#include <iostream>
#include <string>

enum LOGIN_TYPE {
    NETWORK = 3,
    SERVICE = 5,
};

enum LOGIN_STATUS {
    LOGIN_SUCCESS = 1,
    LOGIN_FAIL = 2,
    LOGOUT = 3
};

struct LOGIN_INFO {
    int type;
    unsigned long time;
    bool bLoginSuccess;
    LOGIN_TYPE loginType;
    std::string clientIp;
    std::string clientWorkstation;
    std::string loginUser;
    std::string loginDomain;
    LOGIN_STATUS status;
    std::string userId;
    std::string clientPort;
    std::string subjectUserId;

    void Show()
    {
        std::cout << "type=" << type
                  << ", time=" << time
                  << ", bLoginSuccess=" << bLoginSuccess
                  << ", loginType=" << loginType
                  << ", clientIp=" << clientIp
                  << ", clientWorkstation=" << clientWorkstation
                  << ", loginUser=" << loginUser
                  << ", loginDomain=" << loginDomain
                  << ", status=" << status
                  << ", userId=" << userId
                  << ", clientPort=" << clientPort
                  << ", subjectUserId=" << subjectUserId
                  << std::endl;
    }
};

#endif
