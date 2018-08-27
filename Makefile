all: build_server build_client
build_server:
	g++ -std=c++14 -c *.cpp
	g++ -pthread *.o -o tcp_server
build_client:
	gcc -ggdb -o tcp_client tcp_client.c
