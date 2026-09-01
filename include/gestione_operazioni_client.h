#ifndef GESTIONE_OPERAZIONI_CLIENT_H
#define GESTIONE_OPERAZIONI_CLIENT_H

#include "booking_system_struct.h"
#include "comunicazioneSocket.h"

// Smistatore principale delle risposte/azioni cliente
void gestisci_operazione_cliente(SocketInfo clientSock, op_cliente_t scelta, utente_t utente);

// Funzioni specifiche per singola operazione
void gestisci_operazione_catalogo_aule(SocketInfo clientSock, utente_t utente);
void gestisci_mie_prenotazioni(SocketInfo clientSock, utente_t utente);

#endif