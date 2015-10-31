test: main
	./main

main: g.h m.cpp
	g++ -o main m.cpp
