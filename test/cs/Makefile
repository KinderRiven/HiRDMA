all: client server

server: server.cpp
	g++ -std=c++11 server.cpp -o run_server -I../../include -L../../build -lHIRDMA -libverbs

client: client.cpp
	g++ -std=c++11 client.cpp -o run_client -I../../include -L../../build -lHIRDMA -libverbs