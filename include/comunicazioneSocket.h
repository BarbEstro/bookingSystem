#include <netinet/in.h>
#include <sys/socket.h>

#ifndef COMUNICAZIONE_SOCKET_H
#define COMUNICAZIONE_SOCKET_H

typedef struct SocketInfo {
    int socketfd;
    struct sockaddr_in address;
} SocketInfo;

SocketInfo inizializzaSocketServer();
SocketInfo inizializzaSocketClient();

#endif