#include "TupleTest.h"
#include <iostream>
#include <string>
#include <tuple>

////////////////////////////////////////////////////////////

void TupleTest00()
{
    std::pair<std::string, int> p1 = std::make_pair<std::string, int>("key1", 111);
    std::cout << p1.first << ": " << p1.second << std::endl;

    std::pair<int, int> p2 = std::make_pair<int, int>(2, 3);
    std::cout << p2.first << ": " << p2.second << std::endl;

    std::pair<int, std::string> p3(333, "key3");
    std::cout << p3.first << ": " << p3.second << std::endl;
}

////////////////////////////////////////////////////////////

static std::tuple<std::string, int, double> GetTuple01()
{
    return std::make_tuple<std::string, int, double>("key2", 33, 44.44);
}

void TupleTest01()
{
    // std::tuple<std::string, int, double> t1 = std::make_tuple<std::string, int, double>("key", 11, 22.22);
    // auto t1 = std::make_tuple<std::string, int, double>("key", 11, 22.22);
    auto t1 = std::make_tuple("key", 11, 22.22);
    // std::tuple<std::string, int, double> t1("key", 1, 2.2);
    std::get<0>(t1) = "key1";
    std::get<1>(t1) = 11;
    std::get<2>(t1) = 22.22;
    std::cout << std::get<0>(t1) << std::endl;
    std::cout << std::get<1>(t1) << std::endl;
    std::cout << std::get<2>(t1) << std::endl;

    std::string a;
    double c;
    std::tie(a, std::ignore, c) = GetTuple01();
    std::cout << a << std::endl;
    std::cout << c << std::endl;

    auto [x, y, z] = GetTuple01();
    std::cout << x << std::endl;
    std::cout << y << std::endl;
    std::cout << z << std::endl;
}

////////////////////////////////////////////////////////////

void TupleTest02()
{
    using MyTuple = std::tuple<std::string, int, float>;
    std::cout << std::tuple_size<MyTuple>::value << std::endl;
    std::tuple_element<1, MyTuple>::type a = 1; // int
    std::cout << a << std::endl;

    std::tuple<int, int> t1(24, 48);
    std::tuple<double, double> t2(24.0, 48.1);
    std::cout << (t1 < t2) << std::endl;

    std::tuple<int, std::string, float> t3(10, "test", 3.14);
    int n = 7;
    auto t4 = std::tuple_cat(t3, std::make_pair("Foo", "Bar"), t3, std::tie(n));
    n = 10;
    std::cout << std::get<0>(t4) << " " << std::get<1>(t4) << " " << std::get<2>(t4) << std::endl;
    std::cout << std::get<3>(t4) << " " << std::get<4>(t4) << " " << std::get<5>(t4) << std::endl;
    std::cout << std::get<6>(t4) << " " << std::get<7>(t4) << " " << std::get<8>(t4) << std::endl;
}

////////////////////////////////////////////////////////////

static std::tuple<std::string, int, double> GetTuple03(int i)
{
    if (i == 1) {
        return std::make_tuple("aaa", 12, 3.14);
    }
    return {};
}

void TupleTest03()
{
    auto [a1, b1, c1] = GetTuple03(1);
    auto [a2, b2, c2] = GetTuple03(2);
    std::cout << std::endl;
}

////////////////////////////////////////////////////////////

void TupleTest04()
{

}

////////////////////////////////////////////////////////////

void TupleTest05()
{

}

////////////////////////////////////////////////////////////

void TupleTest06()
{

}

////////////////////////////////////////////////////////////
