CC=g++ -std=c++17 
# note that g++ MUST refer to the gcc C++ compiler,
# and not be an alias for clang as on Mac OS X>

#FLAGS=-Wall -Wno-c++11-extensions
FLAGS=-fmax-errors=3 -Wall -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_ERROR_CODE_HEADER_ONLY
FORTUNE=/usr/games/fortune

test: test_all

test_zip: zip unzip
	seq 5 | xargs -Iz $(FORTUNE) > noise.txt
	./zip < noise.txt > noise.zip
	./unzip < noise.zip > noise2.txt
	diff noise.txt noise2.txt

%: %.cpp
	$(CC) $(FLAGS) $< -o $@

zip: zip.cpp compress.h
unzip: unzip.cpp compress.h

test_img: img
	./img test.png

img: img.cpp compress.h
	$(CC) $(FLAGS) -o img img.cpp lib/lodepng.cpp
	

test_kv: kv
	./kv set answer 42
	./kv get answer
	./kv del answer

kv: kv.cpp
	$(CC) $(FLAGS) -o kv kv.cpp

test_smart: msmart mtype
	./msmart

msmart: smart.cpp
	$(CC) $(FLAGS) -o msmart smart.cpp

mtype: greet.h m.cpp demangle.h sortie.h rvalue.h
	$(CC) $(FLAGS) -o mtype m.cpp

test_all: test_smart test_kv test_img test_zip test_server
# test_server is not included, since it blocks

test_server: server
	python3 test_server.py

server: server.cpp
	$(CC) $(FLAGS) -lboost_system -lboost_thread -o $@ $<

clean:
	rm img kv msmart mtype noise.txt noise.zip noise2.txt

init:
	apt-get update && apt-get install -y fortune libboost-system-dev libboost-thread-dev

