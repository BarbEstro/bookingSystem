#ifndef GESTIONE_OPERAZIONI_SERVER_H
#define GESTIONE_OPERAZIONI_SERVER_H

#include "booking_system_struct.h"

void operazione_login(int client_sock, richiesta_t richiesta);
void operazione_registrazione(int client_sock, richiesta_t richiesta);

// Gestione Aule e Prenotazioni
void operazione_invia_catalogo_aule(int client_sock, richiesta_t richiesta);
void operazione_salva_prenotazione(int client_sock, richiesta_t richiesta);

// Carica risorse da file: restituisce un array terminato con id_risorsa == -1
risorsa_aula_t* carica_risorse_da_file(const char* filename);

#endif // GESTIONE_OPERAZIONI_SERVER_H