#include "VectorTest.h"
#include <iostream>
#include <vector>
#include <algorithm>

////////////////////////////////////////////////////////////

static bool Comp00(int a, int b)
{
    std::cout << a << ", " << b << std::endl;
    return a < b;
}

class Compare00 {
public:
    bool operator() (int a, int b)
    {
        return a > b;
    }
};

void VectorTest00()
{
    int nums[8] = {32, 71, 12, 45, 26, 80, 53, 33};
    std::vector<int> vec(nums, nums + 8);
    std::sort(nums, nums + 8);
    std::sort(vec.begin(), vec.begin() + 4, Comp00);
    std::sort(vec.begin() + 4, vec.end(), Compare00());
    std::sort(vec.begin(), vec.end(), std::greater<int>());
    for (auto &num : vec) {
        std::cout << num << ", ";
    }
    std::cout << std::endl;
}

////////////////////////////////////////////////////////////

struct Student01 {
    int id;
    int height;
    int weight;
};

static bool Comp01(Student01 a, Student01 b)
{
    if (a.height == b.height) {
        return a.weight > b.weight;
    }
    return a.height < b.height;
}

void VectorTest01()
{
    std::vector<Student01> stuVec{
        {1, 170, 50},
        {2, 160, 60},
        {3, 160, 40},
        {4, 170, 60},
        {5, 180, 70}
    };
    std::sort(stuVec.begin(), stuVec.end(), Comp01);
    for (auto &stu : stuVec) {
        std::cout << stu.id << ", " << stu.height << ", " << stu.weight << std::endl;
    }
}

////////////////////////////////////////////////////////////

void VectorTest02()
{
    std::vector<int> vec1{1, 3, 5, 7, 9};
    std::vector<int> vec2(vec1.begin(), vec1.begin() + 3);
    for (auto item : vec2) {
        std::cout << item << std::endl;
    }
}

////////////////////////////////////////////////////////////

void VectorTest03()
{
    std::vector<int> vec{1, 5, 7};
    auto iter = std::find(vec.begin(), vec.end(), 7);
    if (iter != vec.end()) {
        std::cout << "Found Index: " << std::distance(vec.begin(), iter) << std::endl;
        std::cout << "Found Index: " << vec.begin() - iter << std::endl;
    } else {
        std::cout << "Not Found" << std::endl;
    }
}

////////////////////////////////////////////////////////////

void VectorTest04()
{
    std::vector<int> vec{3, 5, 2, 1, 3, 7, 3, 6};
    std::sort(vec.begin(), vec.end());
    auto iter = std::unique(vec.begin(), vec.end());
    if (iter != vec.end()) {
        vec.erase(iter, vec.end());
    }
}

////////////////////////////////////////////////////////////

void VectorTest05()
{
    std::vector<int> vec1{1, 3, 2, 5, 7};
    std::vector<int> vec2{7, 5};
    std::vector<int> res;
    std::sort(vec1.begin(), vec1.end());
    std::sort(vec2.begin(), vec2.end());
    std::set_intersection(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::back_inserter(res));
    for (auto i : res) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
}

////////////////////////////////////////////////////////////

void VectorTest06()
{
    std::vector<int> vec1{5, 7, 3};
    std::vector<int> vec2{4, 6, 2};
    std::vector<int> res;
    std::sort(vec1.begin(), vec1.end());
    std::sort(vec2.begin(), vec2.end());
    std::set_union(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::back_inserter(res));
    for (auto i : res) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
}

////////////////////////////////////////////////////////////

void VectorTest07()
{
    std::vector<int> ages{22, 19, 48, 75, 54, 19, 27, 66, 61, 33, 22, 19};
    int minAge = 18;
    bool isNone = std::none_of(ages.begin(), ages.end(), [minAge](int age) {
        return age < minAge;
    });
    std::cout << isNone << std::endl;
    bool isAny = std::any_of(ages.begin(), ages.end(), [minAge](int age) {
        return age < minAge;
    });
    std::cout << isAny << std::endl;
    int goodAge = 100;
    bool isAll = std::all_of(ages.begin(), ages.end(), [goodAge](int age) {
        return goodAge > age;
    });
    std::cout << isAll << std::endl;
}

////////////////////////////////////////////////////////////

void VectorTest08()
{
    std::vector<int> ages{22, 19, 48, 75, 54, 19, 27, 66, 61, 33, 22, 19};
    int age = 19;
    auto num = std::count(ages.begin(), ages.end(), age);
    std::cout << num << std::endl;
    int maxAge = 60;
    num = std::count_if(ages.begin(), ages.end(), [maxAge](int age) {
        return age > maxAge;
    });
    std::cout << num << std::endl;
}

////////////////////////////////////////////////////////////

void VectorTest09()
{
    std::vector<int> ages{2, 1, 7, 3, 9, 3};
    for (auto iter = ages.begin(); iter != ages.end();) {
        if (*iter == 3) {
            iter = ages.erase(iter);
            continue;
        }
        iter++;
    }
    for (auto age : ages) {
        std::cout << age << ", ";
    }
    std::cout << std::endl;
}

////////////////////////////////////////////////////////////

class TestDemo10 {
public:
    TestDemo10(int num): m_num(num)
    {
        std::cout << "TestDemo10(int num)" << std::endl;
    }

    TestDemo10(const TestDemo10 &demo): m_num(demo.m_num)
    {
        std::cout << "TestDemo10(const TestDemo10 &demo)" << std::endl;
    }

    TestDemo10(TestDemo10 &&demo): m_num(demo.m_num)
    {
        std::cout << "TestDemo10(TestDemo10 &&demo)" << std::endl;
    }

private:
    int m_num;
};

void VectorTest10()
{
    std::vector<TestDemo10> vec1;
    vec1.push_back(2);
    std::cout << "--------------------" << std::endl;
    std::vector<TestDemo10> vec2;
    vec1.emplace_back(2);
}

////////////////////////////////////////////////////////////
