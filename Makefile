
test: msmart mtype
	./msmart

msmart: smart.cpp
	g++-4.9 -std=c++1y -o msmart smart.cpp

mtype: greet.h m.cpp demangle.h sortie.h rvalue.h
	g++-4.9 -std=c++1y -o mtype m.cpp
