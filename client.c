#include <stdio.h>
#include <stdlib.h>
#include "comunicazioneSocket.h"
#include <string.h>

#define MAX_USER_LEN  10
#define MAX_PASS_LEN  20

#define PACKET_SIZE   (1 + MAX_USER_LEN + MAX_PASS_LEN + 2 + 1)

void loginOrRegistrazione(SocketInfo clientSock, int scelta);

int main(){
    SocketInfo clientSock = inizializzaSocketClient();

    printf("BUONGIORNO, BENVENUTO NEL SISTEMA DI PRENOTAZIONE!\n");
    printf("1) Login\n");
    printf("2) Registrazione\n");
    printf("3) Esci\n");


    printf("Inserisci la tua scelta: ");
    int scelta;
    scanf("%d", &scelta);
    while(scelta < 1 || scelta > 3){
        printf("Scelta non valida! Inserisci la tua scelta: ");
        scanf("%d", &scelta);
    }

    switch(scelta){
        case 1:
            printf("Bentornato! Effettua il login.\n");
            loginOrRegistrazione(clientSock, scelta);
            break;
        case 2:
            printf("Benvenuto! Effettua la registrazione.\n");
            loginOrRegistrazione(clientSock, scelta);
            break;
        case 3:
            printf("Uscita dal programma.\n");
            close(clientSock.socketfd);
            exit(EXIT_SUCCESS);
    }
    
    return 0;
}

void loginOrRegistrazione(SocketInfo clientSock, int scelta){
    char pacchetto[PACKET_SIZE];
    char username[MAX_USER_LEN + 1];
    char password[MAX_PASS_LEN + 1];

    printf("Inserisci il tuo username [Max 10 caratteri]: ");
    scanf("%s", username);
    printf("Inserisci la tua password [Max 20 caratteri]: ");
    scanf("%s", password);

    snprintf(pacchetto, sizeof(pacchetto), "%d-%s-%s", scelta, username, password);

    // Invia i dati al server
    //TODO inviare il pacchetto al server utilizzando send() o write()

}