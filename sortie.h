#pragma once

#include <functional>
#include <algorithm>
#include <vector>
#include <cmath>
#include "demangle.h"

#include <iterator>

template<typename Container, typename Predicate>
Container filter(const Container& con, Predicate pred) {
    Container output;
    copy_if(con.begin(), con.end(), std::back_inserter(output), pred);
    return output;
}

bool isPrime(int n) {
    if ( n <= 3 ) return true;
    
    for (int i=2; i < n; i++ ) {
        if ( n % i == 0 ) return false;
    }
    return true;
}

void mmm() {
    // vector to sort
    std::vector<int> nums = { 4,8,7,-6,2,1,9,0,5,-3 };

    // form a closure over a single variable, captured by reference. We
    // could give the function type explicitly and capture the result in
    // a std::function, but we'll just use auto instead.
    int exponent = 2;
    //std::function<int(int)> measure = [&](int x) { return pow(x, exponent); };
    auto square = [&](int x) { return pow(x, exponent); };
    std::cout << real_type_name(square) << std::endl;
    
    // don't need any capture arguments, here.
    auto cube = [](int x) { return pow(x, 3); };
    std::cout << real_type_name(square) << std::endl;

    //std::function<bool(int, int)> cmp = [&](int lhs, int rhs) -> bool { return measure(lhs) < measure(rhs); };
    auto square_cmp= [&](int lhs, int rhs) { return square(lhs) < square(rhs); };
    std::cout << real_type_name(square_cmp) << std::endl;

    auto printV = [](const decltype(nums)& nums) {
        //std::cout << nums << std::endl;
        for ( auto i = nums.begin(); i != nums.end(); i++ ) {
            if ( i != nums.begin() ) std::cout << ", ";
            std::cout << *i;
        }
        std::cout << std::endl;
    };
    
    sort(nums.begin(), nums.end(), square_cmp);
    printV(nums);
    
    // anonymous lambda
    sort(nums.begin(), nums.end(), [&](int lhs, int rhs) { 
        return cube(lhs) < cube(rhs); 
    });
    printV(nums);
    
    // instantiate a copy of the same type as nums.
    decltype(nums) positiveNums{};
    
    // wrapper around the verbose copy_if boilerplate, any predidate to vector
    auto isPositive = [](int n) { return n > 0; };
    positiveNums = filter(nums, isPositive);
    printV(positiveNums);
    
    // ditto, but now, because the return value is a temporary,
    // printV needs to be able to accept an rvalue, which it can
    // because it uses const decltype()&.
    printV(filter(nums, [](int n) { return n % 2 == 0; }));
    
    // of course, you can keep lambdas around in std::functions, if you want to
    std::function<bool(int)> isOdd = [](int n) { return n % 2 == 1; };
    
    // object to repeatedly filter
    auto ultimateNums(nums);
    
    // storing all kinds of functions in a single vector
    std::vector< std::function<bool(int)> > 
    predicates = {
        isPositive, // named lambda
        isOdd, // std::function
        [=](int n) { return n > ultimateNums.size()/2; }, // value binding, remembers original length
        [&](int n) { return n > ultimateNums.size(); }, // reference binding, detects current length
        isPrime // real function
    };
    
    // apply the chain of predicates in sequence
    std::for_each(predicates.begin(), predicates.end(), [&](std::function<bool(int)> predicate) {
        ultimateNums = filter(ultimateNums, predicate);
        printV(ultimateNums);
    });
}
