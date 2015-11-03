#pragma once

#include <functional>
#include <algorithm>
#include <vector>
#include <cmath>
#include "demangle.h"

void mmm() {
    // vector to sort
    std::vector<int> nums = { 4,8,7,6,2,1,9,0,5,3 };

    // form a closure over a single variable, captured by reference. We
    // could give the function type explicitly and capture the result in
    // a std::function, but we'll just use auto instead.
    int exponent = 2;
    //std::function<int(int)> measure = [&](int x) { return pow(x, exponent); };
    auto measure = [&](int x) { return pow(x, exponent); };
    std::cout << real_type_name(measure) << std::endl;

    //std::function<bool(int, int)> cmp = [&](int lhs, int rhs) -> bool { return measure(lhs) < measure(rhs); };
    auto cmp = [&](int lhs, int rhs) { return measure(lhs) < measure(rhs); };
    std::cout << real_type_name(cmp) << std::endl;

    sort(nums.begin(), nums.end(), cmp);

    //std::cout << nums << std::endl;
    for ( auto i = nums.begin(); i != nums.end(); i++ ) {
        std::cout << *i;
    }
    std::cout << std::endl;
}
