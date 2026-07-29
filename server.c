#include <stdio.h>
#include <stdlib.h>
#include "comunicazioneSocket.h"
#include <unistd.h>

int main(){

    printf("Inizio server...\n");
    SocketInfo serverSock = inizializzaSocketServer();
    struct sockaddr_in clientAddress;
    int currentSocketfd;

    while(1){
        printf("In attesa di connessioni...\n");
        socklen_t clientAddressLength = sizeof(clientAddress);
        currentSocketfd = accept(serverSock.socketfd, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if(currentSocketfd < 0){
            perror("Errore nell'accept del socket");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if(pid < 0){
            perror("Errore nella creazione del processo figlio");
            exit(EXIT_FAILURE);
        }
        if(pid == 0){
            // Processo figlio
            close(serverSock.socketfd); // Chiudiamo il socket del server nel processo figlio
            printf("Nuova connessione accettata, creando processo figlio...\n");

            //TODO login
            //1) ricevi username e password dal client
            //2) verifica le credenziali
            //3) invia la risposta al client (successo o fallimento)

            //TODO gestione della comunicazione con il client (menù delle operazioni, ecc.)


            close(currentSocketfd); // Chiudiamo il socket del server nel processo figlio
            printf("Chiusura del processo figlio...\n");
            exit(EXIT_SUCCESS); // Terminiamo il processo figlio dopo aver gestito la comunicazione
        } else {
            // Processo padre
            printf("Connessione accettata, processo padre continua ad ascoltare...\n");
            close(currentSocketfd); // Chiudiamo il socket del client nel processo padre
        }

    }
}