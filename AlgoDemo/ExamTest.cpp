#include "ExamTest.h"
#include <iostream>
#include <string>

////////////////////////////////////////////////////////////

class Solution {
public:
    int EqualSubString(const std::string &s, const std::string &t, int maxCount)
    {
        int left = 0, right = 0;
        int sum = 0;
        int res = 0;
        
        while (right < s.size()) {
            sum += std::abs(s[right] - t[right]);
            right++;
            while (sum > maxCount) {
                sum -= std::abs(s[left] - t[left]);
                left++;
            }
            res = std::max(res, right - left);
        }

        return res;
    }
};

void ExamTest00()
{
    Solution s;
    // std::cout << s.EqualSubString("abcd", "bcdf", 3) << std::endl;
    std::cout << s.EqualSubString("abcd", "cdef", 3) << std::endl;
    // std::cout << s.EqualSubString("abcd", "acde", 0) << std::endl;
}

////////////////////////////////////////////////////////////
