#include "StreamTest.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

//////////////////////////////////////////////////////

void StreamTest00()
{
    int id = 10;
    std::string name = "zhangsan";
    double salary = 12.34;
    
    std::stringstream ss;
    ss << id << " " << name << " " << salary;
    std::string str = ss.str();
    std::cout << str << std::endl;

    std::string str1 = "11 lisi 22.33";
    int id1;
    std::string name1;
    double salary1;
    std::stringstream ss1(str1);
    ss1 >> id1 >> name1 >> salary1;
    std::cout << "id1: " << id1 << std::endl;
    std::cout << "name1: " << name1 << std::endl;
    std::cout << "salary1: " << salary1 << std::endl;
}

//////////////////////////////////////////////////////

void StreamTest01()
{
    std::stringstream ss;
    int x = 1234;  // =4D2
    ss << std::setfill('0') << std::setw(8) << std::hex << x;
    std::cout << ss.str() << std::endl;
    
    ss.clear();
    ss.str("");
    int y = 0x4d2;
    ss << std::setfill('0') << std::setw(8) << std::dec << y;
    std::cout << ss.str() << std::endl;
}

//////////////////////////////////////////////////////

void StreamTest02()
{
    unsigned char buf[] = {0x00, 0x11, 0x22, 0x33};
    std::stringstream ss;
    ss << std::hex;
    ss << std::setw(2);
    ss << std::setfill('0');
    for (size_t i = 0; i < sizeof(buf); i++) {
        ss << static_cast<int>(buf[i]);
    }
    std::cout << ss.str() << std::endl;
}

//////////////////////////////////////////////////////

void StreamTest03()
{
    unsigned int buf = 0x11223344;
    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << buf;
    std::cout << ss.str() << std::endl;  // 11223344

    ss.str("");
    unsigned char *ptr = reinterpret_cast<unsigned char *>(&buf);
    for (size_t i = 0; i < sizeof(buf); i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ptr[i]);
    }
    std::cout << ss.str() << std::endl; // 44332211
}

//////////////////////////////////////////////////////

/*
 * 1001 zhangsan 18
 * 1002 lisi     20
 * 1003 wangwu   19
 */
void StreamTest10()
{
    std::fstream file("file.txt", std::ios::in);
    if (!file.is_open()) {
        return;
    }
    
    const int LEN = 20;
    char buf[LEN] = {0};

    // file.getline(buf, LEN);
    // std::cout << buf << ", " << file.eof() << std::endl;
    // file.getline(buf, LEN);
    // std::cout << buf << ", " << file.eof() << std::endl;
    // file.getline(buf, LEN);
    // std::cout << buf << ", " << file.eof() << std::endl;

    while (!file.eof()) {
        file.getline(buf, LEN);
        std::cout << buf << std::endl;
    }

    //////////////////////////////////

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    file.close();
}

//////////////////////////////////////////////////////

void StreamTest11()
{
    std::ofstream out("file.txt");
    if (!out.is_open()) {
        std::cerr << "open file.txt failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    out << "abc def ghi";
    out.close();
}

//////////////////////////////////////////////////////

void StreamTest12()
{
    std::ifstream in("file.txt");
    if (!in.is_open()) {
        std::cerr << "open file.txt failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    const int LEN = 20;
    char buf[LEN] = {0};

    // in >> buf;
    // std::cout << buf << ", " << in.eof() << std::endl; // abc, 0
    // in >> buf;
    // std::cout << buf << ", " << in.eof() << std::endl; // def, 0
    // in >> buf;
    // std::cout << buf << ", " << in.eof() << std::endl; // ghi, 1

    while (!in.eof()) {
        in >> buf;
        std::cout << buf << std::endl;
    }

    in.close();
}

//////////////////////////////////////////////////////

/*
 * 1001 zhangsan 18
 * 1002 lisi     20
 * 1003 wangwu   19
 */
void StreamTest13()
{
    std::ifstream in("file.txt");
    if (!in.is_open()) {
        std::cerr << "open file.txt failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    const int LEN = 20;
    char buf[LEN] = {0};

    // in.getline(buf, LEN);
    // std::cout << buf << ", " << in.eof() << std::endl;
    // in.getline(buf, LEN);
    // std::cout << buf << ", " << in.eof() << std::endl;
    // in.getline(buf, LEN);
    // std::cout << buf << ", " << in.eof() << std::endl;

    while (!in.eof()) {
        in.getline(buf, LEN);
        std::cout << buf << std::endl;
    }

    in.close();
}

//////////////////////////////////////////////////////

void StreamTest14()
{
    std::ifstream in("file.txt");
    if (!in.is_open()) {
        std::cerr << "open file.txt failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    const int LEN = 20;
    char buf[LEN] = {0};

    // in.getline(buf, LEN, '|');
    // std::cout << buf << ", " << in.eof() << std::endl;
    // in.getline(buf, LEN, '|');
    // std::cout << buf << ", " << in.eof() << std::endl;
    // in.getline(buf, LEN, '|');
    // std::cout << buf << ", " << in.eof() << std::endl;

    while (!in.eof()) {
        in.getline(buf, LEN, '|');
        std::cout << buf << std::endl;
    }

    in.close();
}

//////////////////////////////////////////////////////

void StreamTest15()
{
    std::ofstream out("file.txt");
    if (!out.is_open()) {
        std::cerr << "open file.txt failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    char a = 'a';
    for (int i = 0; i < 26; i++) {
        out << static_cast<char>(a + i);
    }

    out.close();
}

//////////////////////////////////////////////////////

void StreamTest16()
{
    std::ifstream in("file.txt");
    if (!in.is_open()) {
        std::cerr << "open file.txt failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    char ch;
    // ch = in.get();
    // std::cout << ch << ", " << in.eof() << std::endl;
    // ch = in.get();
    // std::cout << ch << ", " << in.eof() << std::endl;
    // ch = in.get();
    // std::cout << ch << ", " << in.eof() << std::endl;
    // ch = in.get();
    // std::cout << (int)ch << ", " << in.eof() << std::endl;
    // ch = in.get();
    // std::cout << (int)ch << ", " << in.eof() << std::endl;
    while ((ch = in.get()) != EOF) {
        std::cout << ch << std::endl;
    }
    
    in.close();
}

//////////////////////////////////////////////////////

void StreamTest17()
{
    std::ifstream in("file.txt");
    if (!in.is_open()) {
        std::cerr << "open file.txt failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    const int LEN = 2;
    char buf[LEN] = {0};
    std::string str;
    
    while (!in.eof()) {
        in.read(buf, LEN);
        str.append(buf, LEN);
    }
    str += '\0';
    std::cout << str << std::endl;

    in.close();
}

//////////////////////////////////////////////////////

void StreamTest18()
{
    std::ofstream out("file.txt");
    if (!out.is_open()) {
        std::cerr << "open file.txt failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    char str[] = "abcd efgh ijk";
    out.write(str, sizeof(str));

    out.close();
}

//////////////////////////////////////////////////////
