#include <stdio.h>
#include <stdlib.h>
#include "comunicazioneSocket.h"
#include <unistd.h>
#include "booking_system_struct.h"
#include "gestione_login.h"
#include "gestione_operazioni_server.h"

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

            richiesta_login_registrazione_t richiesta;
            read(currentSocketfd, &richiesta, sizeof(richiesta));
            risposta_server_t risposta;

            switch(richiesta.operazione) {
                case OP_CLI_LOGIN:
                    printf("Gestione operazione di login...\n");
                    risposta = operazione_login(richiesta);
                    break;
                case OP_CLI_REGISTRAZIONE:
                    printf("Gestione operazione di registrazione...\n");
                    risposta= operazione_registrazione(richiesta);
                    break;
                default:
                    printf("Operazione non riconosciuta dal server.\n");
            }

            write(currentSocketfd, &risposta, sizeof(risposta));




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