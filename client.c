#include <stdio.h>
#include <stdlib.h>
#include "comunicazioneSocket.h"

int main(){
    SocketInfo clientSock = inizializzaSocketClient();

    if(connect(clientSock.socketfd, (struct sockaddr*)&clientSock.address, sizeof(clientSock.address)) < 0){
        perror("Errore nella connessione al server");
        exit(EXIT_FAILURE);
    }


    return 0;
}