#include "MemoryTest.h"
#include <iostream>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////

struct User00 {
    int id;
    std::string name;
    int age;
};

void MemoryTest00()
{
    std::unique_ptr<User00> ptr1;
    std::shared_ptr<User00> ptr2;
    if (ptr1) {
        std::cout << ptr1->id << " " << ptr1->name << " " << ptr1->age << std::endl;
    }
    if (ptr2) {
        std::cout << ptr2->id << " " << ptr2->name << " " << ptr2->age << std::endl;
    }
}

////////////////////////////////////////////////////////////
