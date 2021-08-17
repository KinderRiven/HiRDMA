all: client server

client:
	g++ -std=c++11 server.cpp -o server -I../../include -L../../build -lHIRDMA -libverbs

server:
	g++ -std=c++11 client.cpp -o client -I../../include -L../../build -lHIRDMA -libverbs