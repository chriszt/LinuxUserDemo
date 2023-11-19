#include "SetTest.h"
#include <iostream>
#include <set>

////////////////////////////////////////////////////////////

void SetTest00()
{
    std::set<int> numSet;
    numSet.emplace(2);
    numSet.emplace(1);
    numSet.emplace(4);
    numSet.emplace(3);
    numSet.emplace(1);
    std::cout << numSet.size() << std::endl;
    for (auto num : numSet) {
        std::cout << num << std::endl;
    }
    for (auto iter = numSet.begin(); iter != numSet.end(); iter++) {
        std::cout << *iter << std::endl;
    }
}

////////////////////////////////////////////////////////////
