#include "greet.h"
#include<cstring>
#include<string>
#include<vector>
#include<utility>

#include <typeinfo>
#include "demangle.h"

template<typename container_t, typename index_t>
auto value_at(const container_t container, index_t index)
//   -> decltype(container[index])
{
	return container[index];
}


int main(int argc, char** argv) {
	// const literal
	greet("World");

	// non-const literal
	char thing[100];
	strcpy(thing, "Thing");
	greet(thing);

	// string class
	std::string object("Object");
	greet(object);

	greet(add(std::string("LEFT"), std::string("RIGHT")));
	greet(add(std::string("LEFT"), object));
	greet(add(std::string("LEFT"), thing));

	// auto, type deduction
	auto stringy = add(std::string("LEFT"), object);
	std::cout << real_type_name(stringy) << std::endl;

	// auto, initializers
	auto a = { 11, 23, 9 };
	std::cout << real_type_name(a) << std::endl;

	// generic conversion to vector
	std::vector<decltype(a)::value_type> va = a;
	std::cout << real_type_name(va) << std::endl;

	auto a1 = value_at(va, 1);
	std::cout << a1 << std::endl;
	std::cout << real_type_name(a1) << std::endl;


	return 0;
}


