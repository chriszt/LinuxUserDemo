#include "FuncTest.h"
#include <iostream>
#include <functional>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////

int g_minus00(int i, int j)
{
    return i - j;
}

void FuncTest00()
{
    std::function<int(int, int)> f = g_minus00;
    std::cout << f(1, 2) << std::endl;
}

////////////////////////////////////////////////////////////

template<class T>
T g_minus01(T i, T j)
{
    return i - j;
}

void FuncTest01()
{
    std::function<int(int, int)> f = g_minus01<int>;
    std::cout << f(1, 2) << std::endl;
}

////////////////////////////////////////////////////////////

auto g_minus02 = [](int i, int j) {
    return i - j;
};

void FuncTest02()
{
    std::function<int(int, int)> f = g_minus02;
    std::cout << f(1, 2) << std::endl;
}

////////////////////////////////////////////////////////////

class Minus03 {
public:
    int operator() (int i, int j)
    {
        return i - j;
    }
};

void FuncTest03()
{
    std::function<int(int, int)> f = Minus03();
    std::cout << f(1, 2) << std::endl;
}

////////////////////////////////////////////////////////////

template<typename T>
class Minus04 {
public:
    T operator() (T i, T j)
    {
        return i - j;
    }
};

void FuncTest04()
{
    std::function<int(int, int)> f = Minus04<int>();
    std::cout << f(1, 2) << std::endl;
}

////////////////////////////////////////////////////////////

class Math05 {
public:
    static int Minus(int i, int j)
    {
        return i - j;
    }
};

void FuncTest05()
{
    std::function<int(int, int)> f = &Math05::Minus;
    std::cout << f(1, 2) << std::endl;
}

////////////////////////////////////////////////////////////

class Math06 {
public:
    template <typename T>
    static T Minus(T i, T j)
    {
        return i - j;
    }
};

void FuncTest06()
{
    std::function<int(int, int)> f = &Math06::Minus<int>;
    std::cout << f(1, 2) << std::endl;
}

////////////////////////////////////////////////////////////

class Math07 {
public:
    int Minus(int i, int j)
    {
        return i - j;
    }
};

void FuncTest07()
{
    Math07 m;
    std::function<int(int, int)> f = std::bind(&Math07::Minus, &m, std::placeholders::_1, std::placeholders::_2);
    std::cout << f(1, 2) << std::endl;
}

////////////////////////////////////////////////////////////

class Math08 {
public:
    template<typename T>
    T Minus(T i, T j)
    {
        return i - j;
    }
};

void FuncTest08()
{
    Math08 m;
    std::function<int(int, int)> f = std::bind(&Math08::Minus<int>, &m, std::placeholders::_1, std::placeholders::_2);
    std::cout << f(1, 2) << std::endl;
}

////////////////////////////////////////////////////////////

static bool Filter09_1(const std::string &msg)
{
    bool ret = msg.empty();
    std::cout << "Filter09_1() ret=" << ret << std::endl;
    return ret;
}

static bool Filter09_2(const std::string &msg)
{
    bool ret = msg.size() > 0 && msg.size() < 3;
    std::cout << "Filter09_2() ret=" << ret << std::endl;
    return ret;
}

static bool Filter09_3(const std::string &msg)
{
    bool ret = msg.size() >= 3 && msg.size() < 6;
    std::cout << "Filter09_3() ret=" << ret << std::endl;
    return ret;
}

void FuncTest09()
{
    using FuncPointer = std::function<bool(const std::string &)>;
    std::vector<FuncPointer> funcVec;
    funcVec.push_back(Filter09_1);
    funcVec.push_back(Filter09_2);
    funcVec.push_back(Filter09_3);
    std::string msg = "abcde";
    int num = 0;
    bool isHit = std::any_of(funcVec.begin(), funcVec.end(), [&msg, &num](FuncPointer f) {
        num++;
        return f(msg);
    });
    std::cout << isHit << ", " << num << std::endl;
}

////////////////////////////////////////////////////////////
