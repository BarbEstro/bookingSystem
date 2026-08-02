CC = gcc
CFLAGS = -Wall -Iinclude -g 



mio_programma: main.o funzioni.o
	$(CC) $(CFLAGS) -o mio_programma main.o funzioni.o

tests_login: test_gestione_login.o gestione_login.o
	$(CC) $(CFLAGS) -o test_login test_gestione_login.o gestione_login.o

test_gestione_login.o: tests/test_gestione_login.c include/gestione_login.h include/booking_system_struct.h
	$(CC) $(CFLAGS) -c tests/test_gestione_login.c -o test_gestione_login.o

gestione_login.o: src/gestione_login.c include/gestione_login.h include/booking_system_struct.h
	$(CC) $(CFLAGS) -c src/gestione_login.c -o gestione_login.o

clean:
	rm -f *.o mio_programma test_login