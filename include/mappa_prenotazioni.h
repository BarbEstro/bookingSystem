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

typedef bool (*predicato_prenotazione_t)(const prenotazione_t*, void*);

// Alloca la mappa dinamicamente in base agli ID aule caricati
mappa_t* crea_mappa(const risorsa_aula_t* aule, size_t num_aule);

// Inserisce una nuova prenotazione nel bucket dell'aula corrispondente
bool mappa_inserisci_prenotazione(mappa_t* mappa, int id_aula, prenotazione_t p);

// Ritorna il puntatore alla testa della lista di prenotazioni per una determinata aula
nodo_prenotazione_t* mappa_ottieni_lista(mappa_t* mappa, int id_aula);

// Ritorna true se 'id_aula' corrisponde a un'aula effettivamente presente nella mappa
bool mappa_esiste_aula(mappa_t* mappa, int id_aula);

// Predicato usato da mappa_filtra_prenotazioni: ritorna true se 'p' soddisfa il criterio di ricerca
typedef bool (*prenotazione_predicato_t)(const prenotazione_t* p, void* contesto);

prenotazione_t* mappa_filtra_prenotazioni(mappa_t* mappa, 
                                            predicato_prenotazione_t predicato, 
                                            void* contesto, 
                                            size_t* count);
                                            
// Distrugge l'intera mappa e libera tutta la memoria allocata (nodi + bucket)
void libera_mappa(mappa_t* mappa);

// Costruisce in 'buffer' il percorso del file dedicato alle prenotazioni di una specifica aula
void mappa_path_file_aula(int id_risorsa, char* buffer, size_t size);

// Appende su file la prenotazione, nel file dedicato alla sua aula
bool mappa_salva_prenotazione_su_file(prenotazione_t p);

// Carica in una mappa gia' allocata tutte le prenotazioni salvate su file per le sue aule
bool mappa_carica_da_file(mappa_t* mappa);


#endif // MAPPA_PRENOTAZIONI_H