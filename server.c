#include <stdio.h>
#include <stdlib.h>
#include "comunicazioneSocket.h"
#include <unistd.h>

int main(){

    SocketInfo serverSock = inizializzaSocketServer();
    struct sockaddr_in clientAddress;
    int currentSocketfd;

    while(1){
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
            

            //TODO login
            //1) ricevi username e password dal client
            //2) verifica le credenziali
            //3) invia la risposta al client (successo o fallimento)

            //TODO gestione della comunicazione con il client (menù delle operazioni, ecc.)


            close(currentSocketfd); // Chiudiamo il socket del server nel processo figlio

            exit(EXIT_SUCCESS); // Terminiamo il processo figlio dopo aver gestito la comunicazione
        } else {
            // Processo padre
            close(currentSocketfd); // Chiudiamo il socket del client nel processo padre
        }

    }
}