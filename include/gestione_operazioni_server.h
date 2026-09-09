#ifndef GESTIONE_OPERAZIONI_SERVER_H
#define GESTIONE_OPERAZIONI_SERVER_H

#include "booking_system_struct.h"
#include "mappa_prenotazioni.h"

void operazione_login(int client_sock, richiesta_t richiesta);
void operazione_registrazione(int client_sock, richiesta_t richiesta);

// Gestione Aule e Prenotazioni
void operazione_invia_catalogo_aule(int client_sock, richiesta_t richiesta, risorsa_aula_t* out_aule, size_t num_aule);
void operazione_salva_prenotazione(int client_sock, richiesta_t richiesta, mappa_t* mappa_prenotazioni);
void operazione_lista_mie_prenotazioni(int client_sock, richiesta_t richiesta, mappa_t* mappa_prenotazioni);
void operazione_lista_attesa_prenotazioni(int client_sock, richiesta_t richiesta, mappa_t* mappa_prenotazioni);
void operazione_gestisci_prenotazione(int client_sock, richiesta_t richiesta, mappa_t* mappa_prenotazioni);

#endif // GESTIONE_OPERAZIONI_SERVER_H