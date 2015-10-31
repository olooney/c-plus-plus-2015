#include "g.h"
#include<cstring>
#include<string>

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

	return 0;
}
