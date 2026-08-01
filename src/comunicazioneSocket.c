#include "comunicazioneSocket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_PROTOCOL 0
#define PORT 8080
#define SERVER_IP "127.0.0.1" //TODO dopo vediamo come modificarlo

SocketInfo inizializzaSocketServer(){
    SocketInfo socketInfo;
    socketInfo.socketfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    if(socketInfo.socketfd < 0){
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    socketInfo.address.sin_family = AF_INET;
    socketInfo.address.sin_addr.s_addr = INADDR_ANY;
    socketInfo.address.sin_port = htons(PORT);

    if(bind(socketInfo.socketfd, (struct sockaddr*)&socketInfo.address, sizeof(socketInfo.address)) < 0){
        perror("Errore nel bind del socket");
        exit(EXIT_FAILURE);
    }

    //FIXME cosa succede se ci sono più di 5 client che vogliono connettersi? Forse dovremmo gestire meglio questa cosa
    if(listen(socketInfo.socketfd, 5) < 0){
        perror("Errore nel listen del socket");
        exit(EXIT_FAILURE);
    }

    return socketInfo;
}

SocketInfo inizializzaSocketClient(){
    SocketInfo socketInfo;
    socketInfo.socketfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    if(socketInfo.socketfd < 0){
        perror("Errore nella creazione del socket");
        exit(EXIT_FAILURE);
    }

    socketInfo.address.sin_family = AF_INET;
    socketInfo.address.sin_port = htons(PORT);

    if(inet_pton(AF_INET, SERVER_IP, &socketInfo.address.sin_addr) <= 0){
        perror("Errore nella conversione dell'indirizzo IP");
        exit(EXIT_FAILURE);
    }

    if(connect(socketInfo.socketfd, (struct sockaddr*)&socketInfo.address, sizeof(socketInfo.address)) < 0){
        perror("Errore nella connessione al server");
        exit(EXIT_FAILURE);
    }

    printf("Connessione al server avvenuta con successo!\n");

    return socketInfo;
}