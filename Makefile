CC = gcc
CFLAGS =

all: client server

client: client.c comunicazioneSocket.c
	$(CC) $(CFLAGS) -o client client.c comunicazioneSocket.c

server: server.c comunicazioneSocket.c
	$(CC) $(CFLAGS) -o server server.c comunicazioneSocket.c

clean:
	rm -f client server
