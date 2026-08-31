#ifndef GESTIONE_OPERAZIONI_SERVER_H
#define GESTIONE_OPERAZIONI_SERVER_H

#include "booking_system_struct.h"

void operazione_login(int client_sock, richiesta_t richiesta);
void operazione_registrazione(int client_sock, richiesta_t richiesta);

#endif // GESTIONE_OPERAZIONI_SERVER_H