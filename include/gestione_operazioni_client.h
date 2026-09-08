#ifndef GESTIONE_OPERAZIONI_CLIENT_H
#define GESTIONE_OPERAZIONI_CLIENT_H

#include "booking_system_struct.h"
#include "comunicazioneSocket.h"

// Smistatore principale delle risposte/azioni cliente
void gestisci_operazione_cliente(SocketInfo clientSock, op_cliente_t scelta, utente_t utente);

// Smistatore principale delle risposte/azioni amministratore
void gestisci_operazione_admin(SocketInfo clientSock, op_cliente_t scelta, utente_t utente);

#endif