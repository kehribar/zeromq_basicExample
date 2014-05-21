.PHONY: client server

client:
	gcc zmq_client.c -lzmq -lczmq -o client -Wall

server:
	gcc zmq_server.c -lzmq -lczmq -o server -Wall

clean: 
	rm client server