#pragma once
#include<iostream>

template<typename T>
void greet(const T& entity) {
    std::cout << "Hello, " << entity << std::endl;
}

template<typename T>
T add(const T& lhs, const T& rhs) {
	return lhs + rhs;
}

template<typename T, typename T2>
T add(const T& lhs, const T2& rhs) {
	return lhs + rhs;
}
