#CC=g++ -std=c++1y 
CC=g++ -std=c++14 

FLAGS=-fmax-errors=3

test: test_zip

test_zip: zip unzip
	seq 5 | xargs -Iz /usr/games/fortune > noise.txt
	./zip < noise.txt > noise.zip
	./unzip < noise.zip > noise2.txt
	diff noise.txt noise2.txt

%: %.cpp
	g++ $(FLAGS) $< -o $@

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

test_all: test_smart test_kv test_img test_zip
# test_server is not included, since it blocks

test_server: server
	./server 8181

server: server.cpp
	$(CC) $(FLAGS) -lboost_system -lboost_thread-mt -o $@ $<
