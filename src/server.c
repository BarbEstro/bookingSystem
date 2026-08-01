#include <stdio.h>
#include <stdlib.h>
#include "comunicazioneSocket.h"
#include <unistd.h>
#include "protocollo_login.h"
#include "gestione_login.h"

static int gestione_utenza(const char* username, const char* password, int scelta);

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

            char buffer[PACKET_SIZE];
            read(currentSocketfd, buffer, sizeof(buffer));

            char username[MAX_USER_LEN + 1];
            char password[MAX_PASS_LEN + 1];
            int scelta;
            sscanf(buffer, "%d-%[^-]-%[^-]", &scelta, username, password);

            int risultato = gestione_utenza(username, password, scelta);
            if (risultato == 1) {
                // Login o registrazione riuscita
                write(currentSocketfd, "SUCCESS", 7);
            } else {
                // Login o registrazione fallita
                write(currentSocketfd, "FAILURE", 7);
            }

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

static int gestione_utenza(const char* username, const char* password, int scelta) {
    //TODO fare l'implementazione
}