test: test_img

test_img: img
	./img test.png

img: img.cpp
	g++-4.9 -std=c++1y -o img img.cpp lib/lodepng.cpp
	

test_kv: kv
	./kv set answer 42
	./kv get answer
	./kv del answer

kv: kv.cpp
	g++-4.9 -std=c++1y -o kv kv.cpp

test_smart: msmart mtype
	./msmart

msmart: smart.cpp
	g++-4.9 -std=c++1y -o msmart smart.cpp

mtype: greet.h m.cpp demangle.h sortie.h rvalue.h
	g++-4.9 -std=c++1y -o mtype m.cpp
