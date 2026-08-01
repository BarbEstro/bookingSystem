CC = gcc

all: client server

client: client.o comunicazioneSocket.o
	$(CC) -o $@ $^

server: server.o comunicazioneSocket.o gestione_login.o myfile.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c -o $@ $<

clean:
	rm -f client server *.o

.PHONY: all clean
