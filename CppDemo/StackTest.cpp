#include "StackTest.h"
#include <iostream>
#include <stack>

////////////////////////////////////////////////////////////

void StackTest00()
{
    std::stack<int> s;
    std::cout << s.empty() << std::endl;
    s.push(1);
    std::cout << s.top() << std::endl;
    s.push(2);
    std::cout << s.top() << std::endl;
    s.push(3);
    std::cout << s.top() << std::endl;
}

////////////////////////////////////////////////////////////
