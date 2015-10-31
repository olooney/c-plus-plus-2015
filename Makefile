test: main
	./main

main: greet.h m.cpp demangle.h
	g++-4.8 -std=c++1y -o main m.cpp
