#include "DqueueTest.h"
#include <iostream>
#include <queue>

////////////////////////////////////////////////////////////

void DqueueTest00()
{
    std::queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    std::cout << q.front() << ", " << q.back() << std::endl;
    q.pop();
    std::cout << q.front() << ", " << q.back() << std::endl;
}

////////////////////////////////////////////////////////////
