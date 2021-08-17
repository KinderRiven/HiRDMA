all: client server

server:
	g++ -std=c++11 server.cpp -o run_server -I../../include -L../../build -lHIRDMA -libverbs

client:
	g++ -std=c++11 client.cpp -o run_client -I../../include -L../../build -lHIRDMA -libverbs