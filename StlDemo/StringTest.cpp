#include "StringTest.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <arpa/inet.h>

////////////////////////////////////////////////////////////

//   0238A8C0:0017    0138A8C0:BC06       01     45031
//   192.168.56.2:23  192.168.56.1:48134

static std::string ConvertIp00(const std::string &ipStr)
{
    in_addr_t ipAddr = 0;
    try {
        ipAddr = std::stol(ipStr, nullptr, 16);
    } catch (const std::exception &e) {
        std::cerr << "convert ipStr to ipAddr failed: " << e.what() << std::endl;
        return "";
    }
    char buf[INET_ADDRSTRLEN] = {0};
    if (!inet_ntop(AF_INET, &ipAddr, buf, INET_ADDRSTRLEN)) {
        std::cerr << "convert ipAddr to array failed" << std::endl;
        return "";
    }
    return buf;
}

static unsigned short ConvertPort00(const std::string &portStr)
{
    unsigned short port = 0;
    try {
        port = std::stol(portStr, nullptr, 16);
    } catch (const std::exception &e) {
        std::cerr << "convert portStr to port failed: " << e.what() << std::endl;
        return 0;
    }
    return port;
}

void StringTest00()
{
    // std::string text = "0238A8C0:0017";
    // std::string text = "0138A8C0:BC06";
    std::string text = "FFFFFFFF:FFFF";
    size_t idx = text.find(':');
    std::string ipStr = text.substr(0, idx);
    std::string portStr = text.substr(idx + 1);
    std::string ip = ConvertIp00(ipStr);
    unsigned short port = ConvertPort00(portStr);
    std::cout << ip << ":" << port << std::endl;
}

////////////////////////////////////////////////////////////

//   0000000000000000FFFF00000238A8C0:59EF    0000000000000000FFFF00000138A8C0:ACB6    01    42477
//   FFFF::192.168.56.2:23023                 FFFF::192.168.56.1:44214

static std::string ConvertIp01(const std::string &ipv6Str)
{
    in6_addr addr;
    for (size_t i = 0; i < ipv6Str.size(); i += 2) {
        std::string byteStr = ipv6Str.substr(i, 2);
        addr.s6_addr[i / 2] = std::stoul(byteStr, nullptr, 16);
    }
    char buf[INET6_ADDRSTRLEN] = {0};
    if (!inet_ntop(AF_INET6, &addr, buf, INET6_ADDRSTRLEN)) {
        std::cerr << "convert ipAddr to array failed" << std::endl;
        return "";
    }
    return buf;
}

void StringTest01()
{
    std::string text = "0000000000000000FFFF00000238A8C0:59EF";
    size_t idx = text.find(':');
    std::string ipv6Str = text.substr(0, idx);
    std::string portStr = text.substr(idx + 1);
    std::string ip = ConvertIp01(ipv6Str);
    unsigned short port = ConvertPort00(portStr);
    std::cout << ip << ":" << port << std::endl;
}

////////////////////////////////////////////////////////////

void StringTest02()
{
    std::string str;
    std::cout << str << std::endl;
}

////////////////////////////////////////////////////////////
