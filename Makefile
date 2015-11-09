test: main
	./main

main: greet.h m.cpp demangle.h sortie.h rvalue.h
	g++-4.9 -std=c++1y -o main m.cpp
