#ifndef MAPPA_PRENOTAZIONI_H
#define MAPPA_PRENOTAZIONI_H

//TODO aggiunta di stampa o log per il servers

#include "booking_system_struct.h"
#include <stdbool.h>

// Nodo della lista concatenata
typedef struct nodo_prenotazione {
    prenotazione_t dato;
    struct nodo_prenotazione* next;
} nodo_prenotazione_t;

// Bucket per la singola aula
typedef struct {
    int id_risorsa;
    nodo_prenotazione_t* testa;
} bucket_aula_t;

// Struttura Contenitore della Mappa
typedef struct {
    bucket_aula_t* bucket;
    size_t num_aule;
} mappa_t;

// Alloca la mappa dinamicamente in base agli ID aule caricati
mappa_t* crea_mappa(const risorsa_aula_t* aule, size_t num_aule);

// Inserisce una nuova prenotazione nel bucket dell'aula corrispondente
bool mappa_inserisci(mappa_t* mappa, int id_aula, prenotazione_t p);

// Ritorna il puntatore alla testa della lista di prenotazioni per una determinata aula
//nodo_prenotazione_t* mappa_ottieni_lista(mappa_t* mappa, int id_aula);

// Rimuove una specifica prenotazione tramite il suo id_prenotazione
bool mappa_rimuovi_prenotazione(mappa_t* mappa, int id_aula, int id_prenotazione);

// Distrugge l'intera mappa e libera tutta la memoria allocata (nodi + bucket)
void libera_mappa(mappa_t* mappa);

#endif // MAPPA_PRENOTAZIONI_H